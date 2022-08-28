#ifndef NYLON_MESSAGE_DECODER_H
#define NYLON_MESSAGE_DECODER_H

#include "namespace.h"
#include "nylon_message.h"

#include <cassert>
#include <cstddef>

NYLON_NAMESPACE_BEGIN

enum class DecoderState
{
    NotStarted,
    Success,
    Failure,
    InProgress
};

// Common implementation for message decoders. It
// just calls the messages decoders and assumes
// that decoding is complete after one call to decode.
// This is suitable for staticly sized messages
template<typename MessageType>
class MessageDecoder
{
public:
    void decode(char const * buffer, size_t& bufferPos, size_t /*spaceLeftInBuffer*/)
    {
        assert(state_ == DecoderState::NotStarted);
        msg_ = MessageType::decode(buffer, bufferPos);
        state_ = DecoderState::Success;
    }

    MessageType message() const
    {
        return msg_;
    }

    DecoderState state() const noexcept
    {
        return state_;
    }

private:
    MessageType msg_;
    DecoderState state_ = DecoderState::NotStarted;
};

template<>
class MessageDecoder<Text>
{
public:
    void decode(char const * buffer, size_t & bufferPos, size_t spaceLeftInBuffer);
    Text message() const;
    DecoderState state() const noexcept;

private:
    Text         msg_;
    DecoderState state_          = DecoderState::NotStarted;
    bool haveDecodedMessageType_ = false;
};

NYLON_NAMESPACE_END

#endif // NYLON_MESSAGE_DECODER_H
