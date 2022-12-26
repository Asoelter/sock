#include "nylon_message_encoder.h"

#include <span>

NYLON_NAMESPACE_BEGIN

template<typename MessageDefiner, typename SocketType>
MessageWriter<MessageDefiner, SocketType>::MessageWriter(Params const & params)
    : socket_(params.socket)
    , fileWriter_(maybeInitFileWriter(params.logFileName))
{

}

template<typename MessageDefiner, typename SocketType>
template <typename MessageType>
void MessageWriter<MessageDefiner, SocketType>::write(MessageType const & msg)
{
    handleMessage<MessageType>(msg);
}

template<typename MessageDefiner, typename SocketType>
void MessageWriter<MessageDefiner, SocketType>::growBuffer(size_t size)
{
    auto const wasEmpty      = buffer_.empty();
    auto const beginDistance = std::distance(&*buffer_.begin(), begin_);
    auto const endDistance   = std::distance(&*buffer_.begin(), end_);
    assert(beginDistance >= 0); // would return negative if begin_ < buffer_.begin()
    assert(endDistance >= 0);   // would return negative if begin_ < buffer_.begin()

    buffer_.resize(buffer_.size() + size);

    if (wasEmpty) {
        begin_ = &*buffer_.begin();
        end_   = &*buffer_.begin();
    }
    else {
        // Don't keep original invalidated iterators
        // after resize
        begin_ = &*buffer_.begin() + beginDistance;
        end_   = &*buffer_.begin() + endDistance;
    }
}

template<typename MessageDefiner, typename SocketType>
void MessageWriter<MessageDefiner, SocketType>::shiftForward()
{
    auto const occupiedSize = std::distance(begin_, end_);
    std::copy(&*buffer_.begin(), begin_, end_);
    begin_ = &*buffer_.begin();
    end_ = begin_ + occupiedSize;
}

template<typename MessageDefiner, typename SocketType>
template<typename MessageType>
void MessageWriter<MessageDefiner, SocketType>::handleMessage(MessageType const & msg)
{
    if (buffer_.empty()) {
        // Don't do pointer arithmetic with empty vector begin and end
        growBuffer(msg.encodeSize());
    }

    size_t spaceAtBackOfBuffer = buffer_.size() - (end_ - &*buffer_.begin());

    if (spaceAtBackOfBuffer < msg.encodeSize()) {
        size_t const spaceAtFrontOfBuffer = begin_ - (&*buffer_.begin());

        if (spaceAtFrontOfBuffer > msg.encodeSize()) {
            shiftForward();
        }
        else {
            growBuffer(msg.encodeSize());
        }

        spaceAtBackOfBuffer = buffer_.size() - (end_ - &*buffer_.begin()); // recalculated for assert below
    }

    MessageEncoder::encode(&end_, msg);

    assert(socket_->connected());
    long const bytesWritten = socket_->write(begin_, end_ - begin_);

    if (fileWriter_) {
        fileWriter_->write(std::span<const char>(begin_, end_));
    }

    begin_ += bytesWritten;
}

std::optional<FileWriter> maybeInitFileWriter(std::optional<std::string> const & logFileName)
{
    if (logFileName) {
        return FileWriter(logFileName.value());
    }

    return {};
}

NYLON_NAMESPACE_END
