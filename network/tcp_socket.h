#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H

#include <cstddef>

#include "namespace.h"

NETWORK_NAMESPACE_BEGIN

class TcpSocket;

TcpSocket createTcpClient(const char * address, unsigned port);

// Meant to be created via factory functions.
// This is because TcpSockets can either be
// created as servers or clients and giving
// the socket the functionality to create
// itself for either scenario would be error
// prone
class TcpSocket
{
public:
    friend TcpSocket createTcpClient(const char * address, unsigned port);

    size_t read(char * const buffer, size_t size);
    void write(char const * const buffer, size_t size);

    bool connected() const;

private:
    TcpSocket(int socketFileDescriptor,
              const char * address,
              unsigned port);

    void markDisconnected();

private:

    static constexpr auto badSocketDescriptor = -1;
    static constexpr auto badRead             = -1;

    int socketFileDescriptor_;
    const char * address_;
    unsigned port_;
};

NETWORK_NAMESPACE_END

void sayHello();

#endif // TCP_SOCKET_H
