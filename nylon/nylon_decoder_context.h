#ifndef NYLON_DECODER_CONTEXT_H
#define NYLON_DECODER_CONTEXT_H

#include "namespace.h"
#include "nylon_char_field.h"
#include "nylon_field.h"
#include "nylon_string_field.h"
#include "nylon_uint8_field.h"

#include "../util/typelist.h"

#include <span>

NYLON_NAMESPACE_BEGIN

template <typename M>
concept MessageConcept = requires(M t)
{
    M::messageType;
    typename M::Fields;
};

// Template class to store extra data while decoding
// a meessage. Most fields don't need any extra data
// so the default will be a blank struct. But, fields
// like strings do, because they need to be able to
// remember their size across calls to decode. So,
// class like this can specialize this context and
// add any extra data they need
template <typename Field>
struct FieldDecoderContext
{
};

template <typename Derived>
struct FieldDecoderContext<StringField<Derived>>
{
    size_t sizeRemaining = 0;
    bool sizeRemainingIsSet = false; //< Use seperate bool instead of optional to avoid the optional include. May just use optional later
};

// Utility for the MessageDecoderContext to use
// to inherit from every field decoder
template <ListType Fields>
struct FieldDecoderInheriter
    : FieldDecoderContext<typename Head<Fields>::Archetype>
    , FieldDecoderInheriter<Tail<Fields>>
{

};

template <>
struct FieldDecoderInheriter<EmptyList>
{

};

template <MessageConcept Message>
class MessageDecoderContext : FieldDecoderInheriter<typename Message::Fields>
{
public:
    MessageDecoderContext(std::span<const char> buffer);

    template <typename Field>
    requires std::is_base_of_v<FieldDecoderContext<typename Field::Archetype>, MessageDecoderContext<Message>>
    FieldDecoderContext<typename Field::Archetype>& asContextFor()
    {
        return static_cast<FieldDecoderContext<typename Field::Archetype> &>(*this);
    }

    template <typename Field>
    requires std::is_base_of_v<FieldDecoderContext<typename Field::Archetype>, MessageDecoderContext<Message>>
    FieldDecoderContext<typename Field::Archetype> const & asContextFor() const
    {
        return static_cast<FieldDecoderContext<typename Field::Archetype> const &>(*this);
    }

    Message & message();
    Message const & message() const;
    size_t bytesAlreadyEncoded() const noexcept;
    size_t currentFieldIndex() const noexcept;
    void advanceCurrentFieldIndex();
    bool bufferEmpty() const noexcept;
    bool hasBytesAvailable() const noexcept;
    char nextByte();
    void reset(std::span<const char> newBuffer);

private:
    Message message_;
    std::span<const char> buffer_;
    size_t bytesAlreadyEncoded_ = 0;
    size_t currentFieldIndex_ = 0;
    size_t currentBufferIndex_ = 0;
};

NYLON_NAMESPACE_END

#include "nylon_decoder_context_inline.h"

#endif // NYLON_DECODER_CONTEXT_H
