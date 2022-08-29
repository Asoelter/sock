template <typename SocketType>
MessageReader<SocketType>::MessageReader(SocketType* socket, size_t bufferSize)
    : socket_(socket)
    , buffer_(bufferSize)
    , readOffset_(0)
    , decodeOffset_(0)
{
}

template <typename SocketType>
std::optional<Message> MessageReader<SocketType>::read()
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
        readOffset_ += bytesRead;

        if (bytesRead == net::TcpSocket::wouldBlock) {
            // just try again later
            return std::nullopt;
        }
    }

    auto const messageType = buffer_[decodeOffset_];
    auto const readableBytes = readOffset_ - decodeOffset_;

    if (messageType == static_cast<char>(HeartBeat::messageType)) {
        assert(messageBuilder_.state() == MessageBuilder::State::NotStarted); //heartbeat messages should never be in partial state
        auto const messageState = messageBuilder_.build<HeartBeat>(buffer_.data() + decodeOffset_, decodeOffset_, readOffset_ - decodeOffset_);

        if (decodeOffset_ == buffer_.size()) {
            // catch edge case where the buffer is exactly full
            rollover();
        }

        if (messageState != MessageBuilder::State::Finished) {
            return std::nullopt;
        }

        return messageBuilder_.finalizeMessage();
    }
    else if (messageType == static_cast<char>(Logon::messageType)) {
        assert(messageBuilder_.state() == MessageBuilder::State::NotStarted); // Logon messages should never be in partial state
        auto const messageState = messageBuilder_.build<Logon>(buffer_.data() + decodeOffset_, decodeOffset_, readOffset_ - decodeOffset_);

        if (decodeOffset_ == buffer_.size()) {
            // catch edge case where the buffer is exactly full
            rollover();
        }

        if (messageState != MessageBuilder::State::Finished) {
            return std::nullopt;
        }

        return messageBuilder_.finalizeMessage();
    }
    else if (messageType == static_cast<char>(LogonAccepted::messageType) || messageBuilder_.isBuilding(LogonAccepted::messageType)) {
        assert(messageBuilder_.state() != MessageBuilder::State::Finished);
        auto const messageState = messageBuilder_.build<LogonAccepted>(buffer_.data() + decodeOffset_, decodeOffset_, readOffset_ - decodeOffset_);

        if (decodeOffset_ == buffer_.size()) {
            // catch edge case where the buffer is exactly full
            rollover();
        }

        if (messageState != MessageBuilder::State::Finished) {
            return std::nullopt;
        }

        return messageBuilder_.finalizeMessage();
    }
    else if (messageType == static_cast<char>(Text::messageType) || messageBuilder_.isBuilding(Text::messageType)) {
        assert(messageBuilder_.state() != MessageBuilder::State::Finished);
        auto const messageState = messageBuilder_.build<Text>(buffer_.data() + decodeOffset_, decodeOffset_, readOffset_ - decodeOffset_);

        if (decodeOffset_ == buffer_.size()) {
            // catch edge case where the buffer is exactly full
            rollover();
        }

        if (messageState != MessageBuilder::State::Finished) {
            return std::nullopt;
        }

        return messageBuilder_.finalizeMessage();
    }

    // TODO(asoelter): Log instead of printf
    printf("Unknown message type: %i\n", messageType);

    assert(!"Unknown message type");
    return std::nullopt;
}

template <typename SocketType>
void MessageReader<SocketType>::rollover()
{
    auto const srcBegin  = buffer_.begin() + decodeOffset_;
    auto const srcEnd    = buffer_.end();
    auto const destBegin = buffer_.begin();

    auto const destEnd = std::copy(srcBegin, srcEnd, destBegin);

    decodeOffset_ = 0;
    readOffset_ = std::distance(buffer_.begin(), destEnd);
}
