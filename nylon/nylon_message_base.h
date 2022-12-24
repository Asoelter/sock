#ifndef NYLON_MESSAGE_BASE_H
#define NYLON_MESSAGE_BASE_H

#include "namespace.h"
#include "nylon_field.h"

#include "../util/typelist.h"

#include <cstdint>

NYLON_NAMESPACE_BEGIN

using MessageTypeT = uint8_t;

template <MessageTypeT MessageType, typename ... Fs>
struct MessageBase : Fs...
{
    using Fields = TypeList<Fs...>;

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


NYLON_NAMESPACE_END

#include "nylon_message_base_inline.h"

#endif // NYLON_MESSAGE_BASE_H
