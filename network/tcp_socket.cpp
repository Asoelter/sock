#include "tcp_socket.h"

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <string>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <unistd.h>

#include "util.h"

NETWORK_NAMESPACE_BEGIN

TcpSocket createTcpClient(const char * address, unsigned port)
{
    auto const socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    auto serverAddress = zero_init<sockaddr_in>();
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    inet_pton(AF_INET, address, &serverAddress.sin_addr);

    printf("address: %u\n", serverAddress.sin_addr.s_addr);

    auto const result = ::connect(socketFileDescriptor,
                                  reinterpret_cast<sockaddr *>(&serverAddress),
                                  sizeof(sockaddr_in));
    if (result < 0) {
        throw std::runtime_error("TcpSocket::connect: Error: " + std::string(strerror(errno)));
    }

    // make the socket non-blocking.
    // wait until here so connect isn't
    // treated as non-blocking
    fcntl(socketFileDescriptor, F_SETFL, O_NONBLOCK);

    return TcpSocket(socketFileDescriptor, address, port);
}

long TcpSocket::read(char * const buffer, size_t size)
{
    errno = 0;
    auto const bytesRead = ::read(socketFileDescriptor_, buffer, size);

    if (bytesRead > 0) {
        return bytesRead;
    }
    else if (bytesRead == badRead) {
        if (errno == EWOULDBLOCK) {
            return wouldBlock;
        }
        // TODO(asoelter): log instead of print
        printf("disconnecting for unknown reason(%u): %s\n", errno, strerror(errno));
        shutdown();
    }
    else if (bytesRead == socketClosed) {
        assert(size != 0);
        printf("shutdown requested from client\n");
        shutdown();
    }

    if (errno != 0) {
        throw std::runtime_error("TcpSocket::read: Error: " + std::string(strerror(errno)));
    }

    return bytesRead;
}

long TcpSocket::write(char const * const buffer, size_t size)
{
    assert(connected());

    if (!connected()) {
        throw std::runtime_error("TcpSocket::write called on disconnected socket");
    }

    errno = 0;
    auto const bytesWritten = ::write(socketFileDescriptor_, buffer, size);

    if (bytesWritten == badWrite) {
        if (errno == EWOULDBLOCK) {
            return 0;
        }

        throw std::runtime_error("TcpSocket::write: Error: " + std::string(strerror(errno)));
    }

    assert(errno == 0);
    assert(bytesWritten >= 0);

    return bytesWritten;
}

bool TcpSocket::connected() const
{
    return socketFileDescriptor_ != badSocketDescriptor;
}

TcpSocket::TcpSocket(int socketFileDescriptor,
                     const char * address,
                     unsigned port)
    : socketFileDescriptor_(socketFileDescriptor)
    , address_(address)
    , port_(port)
{
    assert(connected());
}

void TcpSocket::shutdown()
{
    socketFileDescriptor_ = badSocketDescriptor;
}

NETWORK_NAMESPACE_END
