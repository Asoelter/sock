#ifndef NETWORK_UTIL_H
#define NETWORK_UTIL_H

#include "namespace.h"

NETWORK_NAMESPACE_BEGIN

template<typename T>
T zero_init()
{
    T value;
    memset(&value, 0, sizeof(T));
    return value;
}

NETWORK_NAMESPACE_END

#endif // NETWORK_UTIL_H
