NYLON_NAMESPACE_BEGIN

template <typename Derived>
constexpr char * UInt8Field<Derived>::name() const noexcept
{
    return static_cast<Derived const &>(*this).name();
}

template <typename Derived>
uint8_t UInt8Field<Derived>::value() const noexcept
{
    return static_cast<Derived const&>(*this).value();
}

template <typename Derived>
uint8_t& UInt8Field<Derived>::value() noexcept
{
    return static_cast<Derived&>(*this).value();
}

template <typename Derived>
size_t UInt8Field<Derived>::size() const noexcept
{
    return sizeof(uint8_t);
}

template <typename Derived>
size_t UInt8Field<Derived>::encodeSize() const noexcept
{
    return sizeof(uint8_t);
}

NYLON_NAMESPACE_END
