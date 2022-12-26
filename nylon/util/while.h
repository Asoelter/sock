#ifndef META_WHILE_H
#define META_WHILE_H

#include "typelist.h"

#include <utility>

template <typename Condition>
class While
{
public:
    template <typename Message, typename ... Instructions>
    static void Do(Message const & m)
    {
        Condition condition;

        if constexpr (!condition) {
            return;
        }

        using InstructionList = TypeList<Instructions...>;

        if constexpr (!IsEmptyList<InstructionList>) {
            do_impl<InstructionList>(m);
        }
    }

private:

    template <typename Message, typename InstructionList>
    static void do_impl(Message const & m)
    {
    }
};

#endif // META_WHILE_H
