#ifndef NYLON_STRING_FIELD_H
#define NYLON_STRING_FIELD_H

#include "namespace.h"
#include "nylon_field.h"

#include <cstddef>
#include <string>

NYLON_NAMESPACE_BEGIN

template <typename Derived>
struct StringField : public Field<std::string, StringField<Derived>>
{
    constexpr char * name() const noexcept;
    std::string value() const noexcept;
    std::string& value() noexcept;
    size_t size() const noexcept;
    size_t encodeSize() const noexcept;
};

NYLON_NAMESPACE_END

#include "nylon_string_field_inline.h"

#endif // NYLON_STRING_FIELD_H
