NYLON_NAMESPACE_BEGIN

template <typename Derived>
constexpr char const * StringField<Derived>::name() const noexcept
{
    return static_cast<Derived const &>(*this).name();
}

template <typename Derived>
std::string StringField<Derived>::value() const noexcept
{
    return static_cast<Derived const &>(*this).value();
}

template <typename Derived>
std::string& StringField<Derived>::value() noexcept
{
    return static_cast<Derived &>(*this).value();
}

template <typename Derived>
size_t StringField<Derived>::size() const noexcept
{
    return value().size();
}

template <typename Derived>
size_t StringField<Derived>::encodeSize() const noexcept
{
    return value().size() + 1; // we send the size at the start of this field
}

NYLON_NAMESPACE_END
