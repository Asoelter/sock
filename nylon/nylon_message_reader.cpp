#include "nylon_message_reader.h"

NYLON_NAMESPACE_BEGIN

MessageReader::MessageReader(TcpSocket* tcpSocket)
    : tcpSocket_(tcpSocket)
    , readOffset_(0)
    , decodeOffset_(0)
{

}

std::optional<Message> MessageReader::read()
{
    if (!tcpSocket_ || !tcpSocket_->connected()) {
        return std::nullopt;
    }

    auto const bytesRead = tcpSocket_->read(buffer_.data(), buffer_.size() - readOffset_);
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

    assert(!"Unknown message type");
    return std::nullopt;
}

void MessageReader::rollover()
{
    auto const srcBegin  = buffer_.begin() + readOffset_;
    auto const srcEnd    = buffer_.end();
    auto const destBegin = buffer_.begin();

    auto const destEnd = std::copy(srcBegin, srcEnd, destBegin);

    decodeOffset_ = 0;
    readOffset_ = std::distance(buffer_.begin(), destEnd);
}

NYLON_NAMESPACE_END
