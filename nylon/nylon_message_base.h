#ifndef NYLON_MESSAGE_BASE_H
#define NYLON_MESSAGE_BASE_H

#include "namespace.h"
#include "nylon_field.h"

#include "util/typelist.h"

#include <concepts>
#include <cstdint>

NYLON_NAMESPACE_BEGIN

using MessageTypeT = uint8_t;

template <typename T>
concept MessageBaseDerivable = requires(T t)
{
    T::messageType;
    T::name();
};

template <MessageBaseDerivable Derived, typename ... Fs>
struct MessageBase : Fs...
{
    using Fields = TypeList<Fs...>;

    static constexpr auto messageType = Derived::messageType;
    static constexpr auto fieldCount = sizeof...(Fs);

    size_t size() const noexcept;
    size_t encodeSize() const noexcept;
    static const char * name() noexcept;

    template <typename MemberField>
    MemberField& field();

    template <typename MemberField>
    MemberField const & field() const;

    // TODO(asoelter): overload for non-const
    template <typename Visitor>
    void forEachField(Visitor const & visitor) const;

private:
    template <typename Visitor, typename ListOfFields>
    void forEachFieldImpl(Visitor const& visitor) const;
};


NYLON_NAMESPACE_END

#include "nylon_message_base_inline.h"

#endif // NYLON_MESSAGE_BASE_H
