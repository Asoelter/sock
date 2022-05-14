#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "namespace.h"
#include "tcp_socket.h"

#include <poll.h>

#include <functional>
#include <memory>
#include <vector>

NETWORK_NAMESPACE_BEGIN

class TcpServer
{
public:
    class Socket
    {
    public:
        long read(char * const buffer, size_t size);
        void write(char const * const buffer, size_t size);

    private:
        friend class TcpServer;
        Socket(TcpServer* owner, int fd, const char * address, unsigned port);

    private:
        TcpServer* owner_;
        TcpSocket socket_;
    };

    using ConnectHandler = std::function<void(Socket * const)>;
    using ReadHandler    = std::function<void(Socket * const)>;
    using CloseHandler   = std::function<void(Socket * const)>;

    TcpServer() = default;

    void listen(unsigned port); //< establish a listening socket so clients can connect to us
    void poll();                //< look for ready sockets and let socket manage know

    ConnectHandler connectHandler;
    ReadHandler    readHandler;
    CloseHandler   closeHandler;

private:
    friend Socket;
    void stopPollingFor(Socket* socket);

private:
    using Sockets = std::vector<std::unique_ptr<Socket>>;

    Sockets sockets_;
    std::vector<struct pollfd> pollfds_; //< pollfds for the sockets pollfds[i] is for sockets_[i - 1] because pollfds[i] is for the listenSocket
    int listenFileDescriptor_ = TcpSocket::badSocketDescriptor;
};

NETWORK_NAMESPACE_END

#endif // TCP_SERVER_H
