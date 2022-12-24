NYLON_NAMESPACE_BEGIN

template <typename Derived>
constexpr char * CharField<Derived>::name() const noexcept
{
    return static_cast<Derived const &>(*this).name();
}

template <typename Derived>
char CharField<Derived>::value() const noexcept
{
    return static_cast<Derived const &>(*this).value();
}

template <typename Derived>
char& CharField<Derived>::value() noexcept
{
    return static_cast<Derived &>(*this).value();
}

template <typename Derived>
size_t CharField<Derived>::size() const noexcept
{
    return sizeof(char);
}

template <typename Derived>
size_t CharField<Derived>::encodeSize() const noexcept
{
    return sizeof(char);
}

NYLON_NAMESPACE_END
