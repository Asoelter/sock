NYLON_NAMESPACE_BEGIN

template <MessageBaseDerivable Derived, typename ... Fs>
size_t MessageBase<Derived, Fs...>::size() const noexcept
{
    return sizeof(MessageTypeT) + (static_cast<Fs const&>(*this).size() + ... + 0);
}

template <MessageBaseDerivable Derived, typename ... Fs>
size_t MessageBase<Derived, Fs...>::encodeSize() const noexcept
{
    return sizeof(MessageTypeT) + (static_cast<Fs const&>(*this).encodeSize() + ... + 0);
}

template <MessageBaseDerivable Derived, typename ... Fs>
/*static*/ const char * MessageBase<Derived, Fs...>::name() noexcept
{
    return Derived::name();
}

template <MessageBaseDerivable Derived, typename ... Fs>
template <typename MemberField>
MemberField& MessageBase<Derived, Fs...>::field()
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

template <MessageBaseDerivable Derived, typename ... Fs>
template <typename MemberField>
MemberField const & MessageBase<Derived, Fs...>::field() const
{
    static_assert(
        std::is_base_of_v<Field<typename MemberField::ValueType, typename MemberField::Archetype>, MemberField>,
        "Asked for a type that is not a field"
    );

    static_assert(
        Contains<Fields, MemberField>,
        "Asked for a field that does not belong to this message"
    );

    return static_cast<MemberField const&>(*this);
}

template <MessageBaseDerivable Derived, typename ... Fs>
template <typename Visitor>
void MessageBase<Derived, Fs...>::forEachField(Visitor const & visitor) const
{
    // There really never should be any empty messages, but
    // this could save some annoying debuggin in the future
    if constexpr (!std::is_same_v<TypeList<Fs...>, EmptyList>) {
        forEachFieldImpl<Visitor, TypeList<Fs...>>(visitor);
    }
}

template <MessageBaseDerivable Derived, typename ... Fs>
template <typename Visitor, typename ListOfFields>
void MessageBase<Derived, Fs...>::forEachFieldImpl(Visitor const& visitor) const
{
    using CurrentField = Head<ListOfFields>;

    auto const & currentField = static_cast<CurrentField const&>(*this);

    visitor(currentField);

    using RestOfFields = Tail<ListOfFields>;

    if constexpr (!std::is_same_v<RestOfFields, EmptyList>) {
        forEachFieldImpl<Visitor, RestOfFields>(visitor);
    }
}

NYLON_NAMESPACE_END
