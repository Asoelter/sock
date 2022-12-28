#include "nylon_file_writer.h"

NYLON_NAMESPACE_BEGIN

template <typename MessageDefiner, typename SocketType>
MessageReader<MessageDefiner, SocketType>::MessageReader(SocketType* socket, size_t bufferSize)
    : socket_(socket)
    , buffer_(bufferSize)
    , log_(1000 /*maxEntriesToTrack*/)
    , readOffset_(0)
    , decodeOffset_(0)
{
}

template <typename MessageDefiner, typename SocketType>
typename MessageReader<MessageDefiner, SocketType>::MessageTypeOptional
MessageReader<MessageDefiner, SocketType>::read()
{
    if (!socket_ || !socket_->connected()) {
        return std::nullopt;
    }

    if (readOffset_ >= buffer_.size()) {
        // Make sure there's space to read into
        assert(readOffset_ == buffer_.size() && "we read past the end of the buffer");
        rollover();
    }

    auto const bytesRead = socket_->read(buffer_.data() + readOffset_, buffer_.size() - readOffset_);

    if (bytesRead == net::TcpSocket::badRead ||  bytesRead == net::TcpSocket::socketClosed) {
        // Either something went wrong or the client requested a close. In either case the
        // correct thing to do here is just to return nothing
        return std::nullopt;
    }
    if (bytesRead == net::TcpSocket::wouldBlock) {
        // No problem, just no data to read. Only continue processing if there's data stored
        // in our internal buffer that still needs processing.
        // NOTE: we specifically don't want to increment the readOffset_ by the bytesRead here.
        // No data was read and bytesRead == -2 so we'd be decrementing the readOffset_
        if (decodeOffset_ == readOffset_) {
            return std::nullopt;
        }
    }
    else {
        readOffset_ += bytesRead;
    }

    auto const decodeBegin = buffer_.data() + decodeOffset_;
    auto const decodeSize = readOffset_ - decodeOffset_;

    std::optional<typename MessageDefiner::MessageType> maybeMessage;
    unsigned bytesDecoded = 0;

    try {
        log_.beforeProcessing({decodeBegin, decodeSize}, messageBuilder_.state());

        auto const [msg, bytes] = messageBuilder_.build(std::span<const char>(decodeBegin, decodeSize));

        log_.afterProcessing(messageBuilder_.state(), maybeMessage);

        maybeMessage = msg;
        bytesDecoded = bytes;
    }
    catch(std::runtime_error const & e) {
        auto fileWriter = FileWriter("message_reader_error_log.txt");
        fileWriter.write(log_.dump());
        throw;
    }

    decodeOffset_ += bytesDecoded;
    return maybeMessage;
}

template <typename MessageDefiner, typename SocketType>
void MessageReader<MessageDefiner, SocketType>::rollover()
{
    auto const srcBegin  = buffer_.begin() + decodeOffset_;
    auto const srcEnd    = buffer_.end();
    auto const destBegin = buffer_.begin();

    auto const destEnd = std::copy(srcBegin, srcEnd, destBegin);

    decodeOffset_ = 0;
    readOffset_ = std::distance(buffer_.begin(), destEnd);
}

template <typename MessageDefiner, typename SocketType>
template <typename List>
typename MessageReader<MessageDefiner, SocketType>::HandleMessageResult
MessageReader<MessageDefiner, SocketType>::handleMessage(std::span<const char> buffer)
{
    using CurrentType = Head<List>;

    auto const messageType = buffer[0];
    auto const isBuilding = messageBuilder_.template isBuilding<CurrentType>();

    if (messageType == static_cast<char>(CurrentType::messageType) || isBuilding) {
        assert(messageBuilder_.state() != MessageBuilder<MessageDefiner>::State::Finished);
        auto const [bytesDecoded, messageState] = messageBuilder_.template buildImpl<CurrentType>(buffer);

        if (messageState != MessageBuilder<MessageDefiner>::State::Finished) {
            return { .maybeMessage = std::nullopt, .bytesDecoded = bytesDecoded};
        }

        return { .maybeMessage = messageBuilder_.finalizeMessage(), .bytesDecoded = bytesDecoded };
    }

    using NextTypes = Tail<List>;

    if constexpr (IsEmptyList<NextTypes>) {
        // TODO(asoelter): Log instead of printf
        throw std::runtime_error(std::string("Unknown message type") + messageType);
    }
    else {
        return handleMessage<Tail<List>>(buffer);
    }
}

NYLON_NAMESPACE_END
