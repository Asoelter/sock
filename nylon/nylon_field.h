#ifndef NYLON_FIELD_H
#define NYLON_FIELD_H

#include "namespace.h"

NYLON_NAMESPACE_BEGIN

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

    Archetype& asArchetype();
    Archetype const & asArchetype() const;
};

NYLON_NAMESPACE_END

#include "nylon_field_inline.h"

#endif // NYLON_FIELD_H
