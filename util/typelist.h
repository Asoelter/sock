#ifndef NYLON_TYPELIST_H
#define NYLON_TYPELIST_H

#include <type_traits>

// ----------------- TypeList -----------------
template <typename ... Types>
struct TypeList
{
};

// ------------------- Head -------------------
template <typename T>
struct HeadT;

template <typename Head, typename ... Tail>
struct HeadT<TypeList<Head, Tail...>>
{
    using type = Head;
};

template <>
struct HeadT<TypeList<>>
{
    using type = TypeList<>;
};

template <typename List>
using Head = typename HeadT<List>::type;

// ------------------- Tail -------------------
template <typename T>
struct TailT;

template <typename Head, typename ... Tail>
struct TailT<TypeList<Head, Tail...>>
{
    using type = TypeList<Tail...>;
};

template <>
struct TailT<TypeList<>>
{
    using type = TypeList<>;
};

template <typename List>
using Tail = typename TailT<List>::type;

//---------------------- EmptyList----------------------
using EmptyList = TypeList<>;

//--------------------- IsEmptyList ---------------------
template <typename T>
struct IsEmptyListT : std::false_type {};

template <>
struct IsEmptyListT<EmptyList> : std::true_type {};

template <typename List>
static constexpr bool IsEmptyList = IsEmptyListT<List>::value;

//----------------------- IsList-----------------------
template<typename T>
struct IsList : std::false_type {};

template<typename ... Types>
struct IsList<TypeList<Types...>> : std::true_type {};

// ------------------- ListType -------------------
template <typename MaybeList>
concept ListType = IsList<MaybeList>::value;

// ------------------- Contains -------------------
template <ListType List, typename T>
struct ContainsCheck
{
    static constexpr bool value = std::is_same_v<T, Head<List>> ? true : ContainsCheck<Tail<List>, T>::value;
};

template <typename T>
struct ContainsCheck<EmptyList, T>
{
    static constexpr bool value = false;
};

template <ListType List, typename T>
struct ContainsT
    : std::conditional_t<
          ContainsCheck<List, T>::value,
          std::true_type,
          std::false_type
      >
{};

template <ListType List, typename T>
static constexpr bool Contains = ContainsT<List, T>::value;

#endif // NYLON_TYPELIST_H
