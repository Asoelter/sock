#ifndef NYLON_NAMESPACE_H
#define NYLON_NAMESPACE_H

#ifndef NYLON_NAMESPACE
#   define NYLON_NAMESPACE nylon
#endif

#ifndef NYLON_NAMESPACE_BEGIN
#   ifndef NYLON_NAMESPACE
#       error "nylon namespace not defined"
#   endif
#
#   define NYLON_NAMESPACE_BEGIN namespace NETWORK_NAMESPACE {
#endif

#ifndef NYLON_NAMESPACE_END
#   ifndef NYLON_NAMESPACE
#       error "nylon namespace not defined"
#   endif
#
#   ifndef NYLON_NAMESPACE_BEGIN
#       error "nylon namespace begin not defined"
#   endif
#
#   define NYLON_NAMESPACE_END }
#endif

#endif // NYLON_NAMESPACE_H
