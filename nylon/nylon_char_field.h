#ifndef NYLON_CHAR_FIELD_H
#define NYLON_CHAR_FIELD_H

#include "namespace.h"
#include "nylon_field.h"

#include <cstddef>

NYLON_NAMESPACE_BEGIN

template <typename Derived>
struct CharField : public Field<char, CharField<Derived>>
{
    constexpr char * name() const noexcept;
    char value() const noexcept;
    char& value() noexcept;
    size_t size() const noexcept;
    size_t encodeSize() const noexcept;
};

NYLON_NAMESPACE_END

#include "nylon_char_field_inline.h"

#endif // NYLON_CHAR_FIELD_H
