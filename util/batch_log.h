#ifndef BATCH_LOG_H  
#define BATCH_LOG_H

#include "../nylon/namespace.h"

#include <array>
#include <cstddef>

NYLON_NAMESPACE_BEGIN

// Utility class who's initial use case is to keep
// a log of the last N operations that have happened
// in the message writer. It will store up to 1.5 * N
// values. Once someone tries to push more than the
// max values it will discard the first 0.5 * N values
template <typename T, size_t N>
class batch_log
{
public:
    void push(T const & value);

private:
    // Probably worth using a deque if the
    // size is large enough. But for a first
    // pass, knowing I intend to use this for
    // a small number of small objects, this
    // will be fine
    static constexpr auto capacity = (3 * N) / 2;
    std::array<T, capacity> data_;
};

NYLON_NAMESPACE_END

#include "batch_log_inline.h"

#endif // BATCH_LOG_H
