NYLON_NAMESPACE_BEGIN

template <MessageTypeT MessageType, typename ... Fs>
size_t MessageBase<MessageType, Fs...>::size() const noexcept
{
    return sizeof(MessageTypeT) + (static_cast<Fs const&>(*this).size() + ... + 0);
}

template <MessageTypeT MessageType, typename ... Fs>
size_t MessageBase<MessageType, Fs...>::encodeSize() const noexcept
{
    return sizeof(MessageTypeT) + (static_cast<Fs const&>(*this).encodeSize() + ... + 0);
}

template <MessageTypeT MessageType, typename ... Fs>
template <typename MemberField>
MemberField& MessageBase<MessageType, Fs...>::field()
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

template <MessageTypeT MessageType, typename ... Fs>
template <typename MemberField>
MemberField const & MessageBase<MessageType, Fs...>::field() const
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

template <MessageTypeT MessageType, typename ... Fs>
template <typename Visitor>
void MessageBase<MessageType, Fs...>::forEachField(Visitor const & visitor) const
{
    // There really never should be any empty messages, but
    // this could save some annoying debuggin in the future
    if constexpr (!std::is_same_v<TypeList<Fs...>, EmptyList>) {
        forEachFieldImpl<Visitor, TypeList<Fs...>>(visitor);
    }
}

template <MessageTypeT MessageType, typename ... Fs>
template <typename Visitor, typename ListOfFields>
void MessageBase<MessageType, Fs...>::forEachFieldImpl(Visitor const& visitor) const
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
