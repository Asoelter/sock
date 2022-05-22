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

    auto const bytesRead = socket_->read(buffer_.data(), buffer_.size() - readOffset_);
    readOffset_ += bytesRead;

    if (bytesRead < 1) {
        return std::nullopt; //connected() will be set to false next time
    }

    auto const messageType = buffer_[decodeOffset_];
    auto const readableBytes = readOffset_ - decodeOffset_;

    if (messageType == static_cast<char>(HeartBeat::messageType)) {
        if (readableBytes < HeartBeat::size) {
            auto const bytesLeft = buffer_.size() - readOffset_;

            if (bytesLeft > HeartBeat::size) {
                rollover();
            }

            return std::nullopt;
        }

        return HeartBeat::decode(buffer_.data(), decodeOffset_);
    }
    else if (messageType == static_cast<char>(Logon::messageType)) {
        if (readableBytes < Logon::size) {
            auto const bytesLeft = buffer_.size() - readOffset_;

            if (bytesLeft > Logon::size) {
                rollover();
            }

            return std::nullopt;
        }

        return Logon::decode(buffer_.data(), decodeOffset_);
    }
    else if (messageType == static_cast<char>(LogonAccepted::messageType)) {
        if (readableBytes < LogonAccepted::size) {
            auto const bytesLeft = buffer_.size() - readOffset_;

            if (bytesLeft > LogonAccepted::size) {
                rollover();
            }

            return std::nullopt;
        }

        return LogonAccepted::decode(buffer_.data(), decodeOffset_);
    }

    // TODO(asoelter): Log instead of printf
    printf("Unknown message type: %i\n", messageType);

    assert(!"Unknown message type");
    return std::nullopt;
}

template <typename SocketType>
void MessageReader<SocketType>::rollover()
{
    auto const srcBegin  = buffer_.begin() + readOffset_;
    auto const srcEnd    = buffer_.end();
    auto const destBegin = buffer_.begin();

    auto const destEnd = std::copy(srcBegin, srcEnd, destBegin);

    decodeOffset_ = 0;
    readOffset_ = std::distance(buffer_.begin(), destEnd);
}
