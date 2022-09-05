#ifndef WHILE_DO_H
#define WHILE_DO_H

#include "typelist.h"

#include <utility>

template <typename Condition, typename ... Types>
class While
{
public:
    While(Condition && condition)
        : condition(std::forward<Condition>(condition))
    {

    }

    template <typename Task>
    void meta_do(Task && task)
    {
        if constexpr (!IsEmptyList<Types...>) {

        }
    }

private:

    Condition condition;
};

// meta_while<List>([](auto const & field) {return field.value != 5})
//  .meta_do([&count](auto const & field){count += field.value;})

template <typename Condition, typename ... Types>
While<Condition, Types...> meta_while(Condition&& condition)
{
    return While<Condition, Types...>(condition);
}

#endif // WHILE_DO_H
