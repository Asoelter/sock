#ifndef NYLON_MESSAGE_ENCODER_H
#define NYLON_MESSAGE_ENCODER_H

#include "namespace.h"
#include "nylon_message_definer.h"

#include <type_traits>

NYLON_NAMESPACE_BEGIN

// Type to encode each field. It must define
// a static member function named encode that
// takes a char** and a FieldType. It needs
// to encode each field into the buffer. It
// can (currently) be assumed that the caller
// of the encoder has assured there is enough
// room in the buffer to encode the message
template <typename FieldType>
struct FieldEncoder;

// Class type for encoding messages. It is
// the responsibility of the caller to ensure
// there's enough space in the buffer to encode
// the message. There also must be a field
// encoder specialized for every archetypal
// field the message is made of
struct MessageEncoder
{
    template <typename MessageType>
    static void encode(char ** buffer, MessageType const & msg)
    {
        **buffer = static_cast<char>(MessageType::messageType);
        ++(*buffer);

        msg.forEachField(
            [buffer](auto const& field) {
                using T = std::remove_cvref_t<decltype(field)>;
                using Archetype = typename T::Archetype;

                FieldEncoder<Archetype>::encode(buffer, field);
            }
        );
    }
};

template <typename Derived>
struct FieldEncoder<CharField<Derived>>
{
    static void encode(char ** buffer, CharField<Derived> const & field)
    {
        **buffer = field.value();
        ++(*buffer);
    }
};

template <typename Derived>
struct FieldEncoder<StringField<Derived>>
{
    static void encode(char ** buffer, StringField<Derived> const & field)
    {
        **buffer = static_cast<char>(field.value().length());
        ++(*buffer);

        for (auto c : field.value()) {
            **buffer = c;
            ++(*buffer);
        }
    }
};

template <typename Derived>
struct FieldEncoder<UInt8Field<Derived>>
{
    static void encode(char ** buffer, UInt8Field<Derived> const & field)
    {
        **buffer = field.value();
        ++(*buffer);
    }
};

NYLON_NAMESPACE_END

#endif // NYLON_MESSAGE_ENCODER_H
