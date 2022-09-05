NYLON_NAMESPACE_BEGIN

template <typename MessageDefiner, typename SocketType>
MessageReader<MessageDefiner, SocketType>::MessageReader(SocketType* socket, size_t bufferSize)
    : socket_(socket)
    , buffer_(bufferSize)
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

    if (readOffset_ != buffer_.size()) {
        // only read if there is space to read into. Don't rollover or else you'll cause a ton
        // of extra rollovers. If we've decoded all the bytes, we'll roll over later on, so the
        // only time we can get here is if the buffer is full for reading, but still has space
        // to decode
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
    }

    auto const messageType = buffer_[decodeOffset_];

    return handleMessage<typename MessageDefiner::MessageTypes>(messageType);
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
typename MessageReader<MessageDefiner, SocketType>::MessageTypeOptional
MessageReader<MessageDefiner, SocketType>::handleMessage(char messageType)
{
    using CurrentType = Head<List>;

    auto const isBuilding = messageBuilder_.template isBuilding<CurrentType>();
    auto const state = messageBuilder_.state();
    (void)state;

    if (messageType == static_cast<char>(CurrentType::messageType) || isBuilding) {
        assert(messageBuilder_.state() != MessageBuilder<MessageDefiner>::State::Finished);
        auto const messageState = messageBuilder_.template build<CurrentType>(buffer_.data() + decodeOffset_, decodeOffset_, readOffset_ - decodeOffset_);

        if (decodeOffset_ == buffer_.size()) {
            // catch edge case where the buffer is exactly full
            rollover();
        }

        if (messageState != MessageBuilder<MessageDefiner>::State::Finished) {
            return std::nullopt;
        }

        return messageBuilder_.finalizeMessage();
    }

    using NextTypes = Tail<List>;

    if constexpr (IsEmptyList<NextTypes>) {
        // TODO(asoelter): Log instead of printf
        printf("Unknown message type: %i\n", messageType);
        assert(!"Unknown message type");
        return std::nullopt;
    }
    else {
        return handleMessage<Tail<List>>(messageType);
    }
}

NYLON_NAMESPACE_END
