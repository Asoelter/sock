#ifndef NYLON_MESSAGE_DECODER_H
#define NYLON_MESSAGE_DECODER_H

#include "namespace.h"
#include "nylon_field_decoder.h"
#include "nylon_message.h"

#include "../util/typelist.h"

#include <cassert>
#include <cstddef>
#include <stdexcept>
#include <span>

NYLON_NAMESPACE_BEGIN

struct MessageDecoder
{
public:
    template <typename MessageType>
    static DecodeResult decode(MessageDecoderContext<MessageType>& context);

private:
    template <typename MessageType, typename Fields>
    static DecodeResult decodeImpl(MessageDecoderContext<MessageType>& context, size_t fieldIndex);
};

NYLON_NAMESPACE_END

#include "nylon_message_decoder_inline.h"

#endif // NYLON_MESSAGE_DECODER_H
