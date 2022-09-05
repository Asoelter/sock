#ifndef NYLON_MESSAGE_DEFINER
#define NYLON_MESSAGE_DEFINER

// TODO(asoelter): Rename file to just nylon_message.h
// once the original nylon_message.h file is removed

#include "namespace.h"

#include "../util/typelist.h"
#include "../util/type_traits.h"

#include <concepts>
#include <cstdint>
#include <span>
#include <string>
#include <type_traits>
#include <variant>

NYLON_NAMESPACE_BEGIN

// ------------------------------- Macros -------------------------------
#define FIELD(archetype, fieldName)                             \
    struct fieldName##Field : archetype<fieldName##Field>       \
    {                                                           \
        constexpr const char * name() const noexcept            \
        {                                                       \
            return #fieldName;                                  \
        }                                                       \
                                                                \
        ValueType const & value() const                         \
        {                                                       \
            return fieldName;                                   \
        }                                                       \
                                                                \
        ValueType & value()                                     \
        {                                                       \
            return fieldName;                                   \
        }                                                       \
                                                                \
        ValueType fieldName;                                    \
    }

#define CHAR_FIELD(name)   FIELD(CharField,   name)
#define STRING_FIELD(name) FIELD(StringField, name)
#define UINT8_FIELD(name)  FIELD(UInt8Field,  name)

// ------------------------------ Concepts ------------------------------
template <typename T>
concept FieldDerivable = requires(T t)
{
    // TODO(asoelter): check type of result when compiler
    // gets better
    { t.name() };
    { t.value() };
};

// ------------------------------- Structs -------------------------------
template <typename ValueT, typename Derived>
struct Field
{
    using ValueType = ValueT;
    using Archetype = Derived;

    Archetype& asArchetype() { return static_cast<Archetype&>(*this); }
    Archetype const & asArchetype() const { return static_cast<Archetype&>(*this); }
};

template <typename Derived>
struct CharField : public Field<char, CharField<Derived>>
{
    constexpr char * name() const noexcept
    {
        return static_cast<Derived const &>(*this).name();
    }

    char value() const noexcept
    {
        return static_cast<Derived const &>(*this).value();
    }

    char& value() noexcept
    {
        return static_cast<Derived &>(*this).value();
    }

    size_t size() const noexcept
    {
        return sizeof(char);
    }

    size_t encodeSize() const noexcept
    {
        return sizeof(char);
    }
};

template <typename Derived>
struct StringField : public Field<std::string, StringField<Derived>>
{
    constexpr char * name() const noexcept
    {
        return static_cast<Derived const &>(*this).name();
    }

    std::string value() const noexcept
    {
        return static_cast<Derived const &>(*this).value();
    }

    std::string& value() noexcept
    {
        return static_cast<Derived &>(*this).value();
    }

    size_t size() const noexcept
    {
        return value().size();
    }

    size_t encodeSize() const noexcept
    {
        return value().size() + 1; // we send the size at the start of this field
    }
};

template <typename Derived>
struct UInt8Field : public Field<uint8_t, UInt8Field<Derived>>
{
    constexpr char * name() const noexcept
    {
        return static_cast<Derived const &>(*this).name();
    }

    uint8_t value() const noexcept
    {
        return static_cast<Derived const&>(*this).value();
    }

    uint8_t& value() noexcept
    {
        return static_cast<Derived&>(*this).value();
    }

    size_t size() const noexcept
    {
        return sizeof(uint8_t);
    }

    size_t encodeSize() const noexcept
    {
        return sizeof(uint8_t);
    }
};

using MessageTypeT = uint8_t;

template <MessageTypeT MessageType, typename ... Fs>
struct MessageBase : Fs...
{
    using Fields = TypeList<Fs...>;

    // TODO(asoelter): Add length field
    static constexpr auto messageType = MessageType;

    size_t size() const noexcept
    {
        return sizeof(MessageTypeT) + (static_cast<Fs const&>(*this).size() + ... + 0);
    }

    size_t encodeSize() const noexcept
    {
        return sizeof(MessageTypeT) + (static_cast<Fs const&>(*this).encodeSize() + ... + 0);
    }

    template <typename MemberField>
    auto& field()
    {
        static_assert(
            std::is_base_of_v<Field<typename MemberField::ValueType, typename MemberField::Archetype>, MemberField>,
            "Asked for a type that is not a field"
        );

        static_assert(
            Contains<Fields, MemberField>,
            "Asked for a field that does not belong to this message"
        );

        return static_cast<MemberField&>(*this);
    }

    template <typename MemberField>
    auto const & field() const
    {
        static_assert(
            std::is_base_of_v<Field<typename MemberField::ValueType, typename MemberField::Archetype>, MemberField>,
            "Asked for a type that is not a field"
        );

        static_assert(
            Contains<Fields, MemberField>,
            "Asked for a field that does not belong to this message"
        );

        return static_cast<MemberField&>(*this);
    }

    // TODO(asoelter): overload for non-const
    template <typename Visitor>
    void forEachField(Visitor const & visitor) const
    {
        // There really never should be any empty messages, but
        // this could save some annoying debuggin in the future
        if constexpr (!std::is_same_v<TypeList<Fs...>, EmptyList>) {
            forEachFieldImpl<Visitor, TypeList<Fs...>>(visitor);
        }
    }

private:
    template <typename Visitor, typename ListOfFields>
    void forEachFieldImpl(Visitor const& visitor) const
    {
        using CurrentField = Head<ListOfFields>;

        auto const & currentField = static_cast<CurrentField const&>(*this);

        visitor(currentField);

        using RestOfFields = Tail<ListOfFields>;

        if constexpr (!std::is_same_v<RestOfFields, EmptyList>) {
            forEachFieldImpl<Visitor, RestOfFields>(visitor);
        }
    }
};

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
    MessageDecoderContext(std::span<const char> buffer)
        : buffer_(buffer)
    {

    }

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

    Message & message() { return message_; }
    Message const & message() const { return message_; }
    size_t bytesAlreadyEncoded() const noexcept { return bytesAlreadyEncoded_; }
    size_t currentFieldIndex() const noexcept { return currentFieldIndex_; }
    void advanceCurrentFieldIndex() { ++currentFieldIndex_; }
    bool bufferEmpty() const noexcept { return currentBufferIndex_ >= buffer_.size(); }
    bool hasBytesAvailable() const noexcept { return !bufferEmpty(); }

    char nextByte()
    {
        assert(currentBufferIndex_ < buffer_.size());

        auto const result = buffer_[currentBufferIndex_];

        ++bytesAlreadyEncoded_;
        ++currentBufferIndex_;

        return result;
    }

    void reset(std::span<const char> newBuffer)
    {
        buffer_ = newBuffer;
        currentBufferIndex_ = 0;
    }

private:
    Message message_;
    std::span<const char> buffer_;
    size_t bytesAlreadyEncoded_ = 0;
    size_t currentFieldIndex_ = 0;
    size_t currentBufferIndex_ = 0;
};

template <typename ... MessageTs>
struct MessageDefiner
{
    // TODO(asoelter): Probably worth coming up with
    // a better naming scheme for these
    using MessageType  = std::variant<MessageTs...>;
    using MessageTypes = TypeList<MessageTs...>;
    using DecoderContext = std::variant<MessageDecoderContext<MessageTs>...>;
};

NYLON_NAMESPACE_END

#endif // NYLON_MESSAGE_DEFINER
