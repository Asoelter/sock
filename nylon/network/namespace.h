#ifndef NETWORK_NAMESPACE_H
#define NETWORK_NAMESPACE_H

#ifndef NETWORK_NAMESPACE
#   define NETWORK_NAMESPACE net
#endif

#ifndef NETWORK_NAMESPACE_BEGIN
#   ifndef NETWORK_NAMESPACE
#       error "network namespace not defined"
#   endif
#
#   define NETWORK_NAMESPACE_BEGIN namespace NETWORK_NAMESPACE {
#endif

#ifndef NETWORK_NAMESPACE_END
#   ifndef NETWORK_NAMESPACE
#       error "network namespace not defined"
#   endif
#
#   ifndef NETWORK_NAMESPACE_BEGIN
#       error "network namespace begin not defined"
#   endif
#
#   define NETWORK_NAMESPACE_END }
#endif

#endif // NETWORK_NAMESPACE_H
