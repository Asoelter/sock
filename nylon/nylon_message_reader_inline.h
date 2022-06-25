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

    if (buffer_.size() - readOffset_ == 0) {
        rollover();
    }

    auto const bytesRead = socket_->read(buffer_.data() + readOffset_, buffer_.size() - readOffset_);
    readOffset_ += bytesRead;

    auto const messageType = buffer_[decodeOffset_];
    auto const readableBytes = readOffset_ - decodeOffset_;

    if (messageType == static_cast<char>(HeartBeat::messageType)) {
        if (readableBytes < HeartBeat::size) {
            auto const bytesLeft = buffer_.size() - decodeOffset_;

            // Only roll over if there isn't enough room. If it was
            // a partial read there could be enough room in our buffer,
            // but we just need to read again to get the rest of the message
            if (bytesLeft < HeartBeat::size) {
                rollover();
            }

            return std::nullopt;
        }

        auto const result = HeartBeat::decode(buffer_.data() + decodeOffset_, decodeOffset_);

        if (decodeOffset_ == buffer_.size()) {
            // catch edge case where the buffer is exactly full
            rollover();
        }

        return result;
    }
    else if (messageType == static_cast<char>(Logon::messageType)) {
        if (readableBytes < Logon::size) {
            auto const bytesLeft = buffer_.size() - decodeOffset_;

            // Only roll over if there isn't enough room. If it was
            // a partial read there could be enough room in our buffer,
            // but we just need to read again to get the rest of the message
            if (bytesLeft < Logon::size) {
                rollover();
            }

            return std::nullopt;
        }

        auto const result = Logon::decode(buffer_.data() + decodeOffset_, decodeOffset_);

        if (decodeOffset_ == buffer_.size()) {
            // catch edge case where the buffer is exactly full
            rollover();
        }

        return result;
    }
    else if (messageType == static_cast<char>(LogonAccepted::messageType)) {
        if (readableBytes < LogonAccepted::size) {
            auto const bytesLeft = buffer_.size() - decodeOffset_;

            // Only roll over if there isn't enough room. If it was
            // a partial read there could be enough room in our buffer,
            // but we just need to read again to get the rest of the message
            if (bytesLeft < LogonAccepted::size) {
                rollover();
            }

            return std::nullopt;
        }

        auto const result = LogonAccepted::decode(buffer_.data() + decodeOffset_, decodeOffset_);

        if (decodeOffset_ == buffer_.size()) {
            // catch edge case where the buffer is exactly full
            rollover();
        }

        return result;
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
