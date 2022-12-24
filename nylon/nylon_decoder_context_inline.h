NYLON_NAMESPACE_BEGIN

template <MessageConcept Message>
MessageDecoderContext<Message>::MessageDecoderContext(std::span<const char> buffer)
    : buffer_(buffer)
{

}

template <MessageConcept Message>
Message & MessageDecoderContext<Message>::message()
{
    return message_;
}

template <MessageConcept Message>
Message const & MessageDecoderContext<Message>::message() const
{
    return message_;
}

template <MessageConcept Message>
size_t MessageDecoderContext<Message>::bytesAlreadyEncoded() const noexcept
{
    return bytesAlreadyEncoded_;
}

template <MessageConcept Message>
size_t MessageDecoderContext<Message>::currentFieldIndex() const noexcept
{
    return currentFieldIndex_;
}

template <MessageConcept Message>
void MessageDecoderContext<Message>::advanceCurrentFieldIndex()
{
    ++currentFieldIndex_;
}

template <MessageConcept Message>
bool MessageDecoderContext<Message>::bufferEmpty() const noexcept
{
    return currentBufferIndex_ >= buffer_.size();
}

template <MessageConcept Message>
bool MessageDecoderContext<Message>::hasBytesAvailable() const noexcept
{
    return !bufferEmpty();
}

template <MessageConcept Message>
char MessageDecoderContext<Message>::nextByte()
{
    assert(currentBufferIndex_ < buffer_.size());

    auto const result = buffer_[currentBufferIndex_];

    ++bytesAlreadyEncoded_;
    ++currentBufferIndex_;

    return result;
}

template <MessageConcept Message>
void MessageDecoderContext<Message>::reset(std::span<const char> newBuffer)
{
    buffer_ = newBuffer;
    currentBufferIndex_ = 0;
}

NYLON_NAMESPACE_END
