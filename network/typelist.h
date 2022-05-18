#ifndef NETWORK_TYPELIST_H
#define NETWORK_TYPELIST_H

#include <functional>
#include <type_traits>

// Move this out of the network directory
// if it gets use elsewhere

template<typename ... Args>
struct TypeList
{
    static constexpr auto Size = sizeof...(Args);
};

//---------------------- HEAD ----------------------
template<typename List>
struct HeadT;

template<typename H, typename ... T>
struct HeadT<TypeList<H, T...>>
{
    using type = H;
};

template<typename List>
using Head = typename HeadT<List>::type;

//---------------------- TAIL ----------------------
template<typename List>
struct TailT;

template<typename H, typename ... T>
struct TailT<TypeList<H, T...>>
{
    using type = TypeList<T...>;
};

template<>
struct TailT<TypeList<>>
{
    using type = TypeList<>;
};

template<typename List>
using Tail = typename TailT<List>::type;

//---------------------- Empty List----------------------
using EmptyList = TypeList<>;

//---------------------- IsList----------------------
template<typename T>
struct IsList : std::false_type {};

template<typename ... Types>
struct IsList<TypeList<Types...>> : std::true_type {};

//------------------------- PolymorphicForEach ---------------------------
// Purpose: Given a class that inherrits from all classes in BaseClasses |
//          cast that class to each base class and call a function on    |
//          the result of the cast. See message.h for prime use case     |
//------------------------------------------------------------------------
template<typename BaseClasses>
class PolymorphicForEach
{
public:
    static_assert(IsList<BaseClasses>::value, "Non type list given to PolymorphicForEach");

    template<typename Derived, typename F>
    void operator()(Derived const & derived, F&& f)
    {
        impl(derived, f);
    }

private:
    template<typename Derived, typename F>
    void impl(Derived const & derived, F&& f)
    {
        implRecursive<BaseClasses>(derived, f);
    }

    template<typename BaseClassesRemaining, typename Derived, typename F>
    void implRecursive(Derived const & derived, F&& f)
    {
        using First = Head<BaseClassesRemaining>;

        static_assert(std::is_base_of_v<First, Derived>,
                "PolymorphicForEach called with non-base class");

        std::invoke(f, static_cast<First const &>(derived));

        using Rest = Tail<BaseClassesRemaining>;

        if constexpr (Rest::Size > 0) {
            implRecursive<Rest>(derived, f);
        }
    }
};

#endif // NETWORK_TYPELIST_H
