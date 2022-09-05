#ifndef NYLON_FIELD_DECODER
#define NYLON_FIELD_DECODER

#include "namespace.h"
#include "nylon_message_definer.h"

#include <span>

NYLON_NAMESPACE_BEGIN

enum class DecoderState
{
    NotStarted,
    Success,
    InProgress
};

struct DecodeResult
{
    DecoderState state;
    size_t bytesEncoded;
};

template <typename Field>
struct FieldDecoder
{
    // Generic class, forward the field onto a decoder specific to the
    // archetype of the field
    template <typename MessageContext>
    static DecodeResult decode(Field& field, MessageContext & context);
};

template <typename Derived>
struct FieldDecoder<CharField<Derived>>
{
    template <typename MessageContext>
    static DecodeResult decode(CharField<Derived>& field, MessageContext & context);
};

template <typename Derived>
struct FieldDecoder<StringField<Derived>>
{
    template <typename MessageContext>
    static DecodeResult decode(StringField<Derived>& field, MessageContext & context);
};

template <typename Derived>
struct FieldDecoder<UInt8Field<Derived>>
{
    template <typename MessageContext>
    static DecodeResult decode(UInt8Field<Derived>& field, MessageContext & context);
};

NYLON_NAMESPACE_END

#include "nylon_field_decoder_inline.h"

#endif // NYLON_FIELD_DECODER
