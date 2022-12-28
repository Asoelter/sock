#ifndef NYLON_UINT8_FIELD_H
#define NYLON_UINT8_FIELD_H

#include "namespace.h"
#include "nylon_field.h"

#include <cstddef>
#include <cstdint>

NYLON_NAMESPACE_BEGIN

template <typename Derived>
struct UInt8Field : public Field<uint8_t, UInt8Field<Derived>>
{
    constexpr char const * name() const noexcept;
    uint8_t value() const noexcept;
    uint8_t& value() noexcept;
    size_t size() const noexcept;
    size_t encodeSize() const noexcept;
};

NYLON_NAMESPACE_END

#include "nylon_uint8_field_inline.h"

#endif // NYLON_UINT8_FIELD_H
