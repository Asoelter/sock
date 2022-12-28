NYLON_NAMESPACE_BEGIN

template <typename ValueT, typename Derived>
typename Field<ValueT, Derived>::Archetype & Field<ValueT, Derived>::asArchetype()
{
    return static_cast<Archetype&>(*this);
}

template <typename ValueT, typename Derived>
typename Field<ValueT, Derived>::Archetype const & Field<ValueT, Derived>::asArchetype() const
{
    return static_cast<Archetype const &>(*this);
}

NYLON_NAMESPACE_END
