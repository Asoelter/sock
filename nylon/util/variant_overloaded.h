#ifndef VARIANT_OVERLOADED_H
#define VARIANT_OVERLOADED_H

template <typename ... Types>
struct Overloaded : Types...
{
    using Types::operator()...;
};

template <typename ... Types>
Overloaded(Types...) -> Overloaded<Types...>;

#endif // VARIANT_OVERLOADED_H
