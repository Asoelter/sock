#include "nylon_message_decoder.h"

#include <algorithm> // just for std::min :(

NYLON_NAMESPACE_BEGIN

void MessageDecoder<Text>::decode(char const * buffer,
                                  size_t & bufferPos,
                                  size_t spaceLeftInBuffer)
{
    if (!haveDecodedMessageType_) {
        state_ = DecoderState::InProgress; // must be at start of first if
        auto const decodedMessageType = *buffer;
        ++buffer;
        ++bufferPos;

        assert(decodedMessageType == static_cast<char>(Text::messageType));
        if (decodedMessageType != static_cast<char>(Text::messageType)) {
            state_ = DecoderState::Failure;
            throw std::runtime_error("Text::decode asked to decode a non-Text");
        }

        haveDecodedMessageType_ = true;
    }

    if (msg_.textSize == 0) {
        msg_.textSize = static_cast<size_t>(*buffer);
        ++buffer;
        ++bufferPos;
    }

    if (msg_.text.size() < static_cast<size_t>(msg_.textSize)) {
        auto const sizeStillNeeded = msg_.textSize - msg_.text.size();
        auto const sizeToAppend = std::min<size_t>(spaceLeftInBuffer, sizeStillNeeded);
        msg_.text.append(buffer, sizeToAppend);
    }

    if (msg_.text.size() == msg_.textSize) {
        state_ = DecoderState::Success;
    }
}

Text MessageDecoder<Text>::message() const
{
    assert(state_ == DecoderState::Success); // don't return partially formed messages
    return msg_;
}

NYLON_NAMESPACE_END
