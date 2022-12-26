#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "namespace.h"
#include "tcp_socket.h"

#include <poll.h>

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

NETWORK_NAMESPACE_BEGIN

class TcpServer
{
public:
    class Socket
    {
    public:
        using Id = size_t;

        long read(char * const buffer, size_t size);
        void write(char const * const buffer, size_t size);
        Id id() const noexcept;
        bool connected() const;

    private:
        friend class TcpServer;
        Socket(TcpServer* owner, int fd, const char * address, unsigned port, Id id);

    private:
        TcpServer* owner_;
        TcpSocket  socket_;
        Id         id_;
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
    using Sockets   = std::vector<std::unique_ptr<Socket>>;
    using SocketMap = std::unordered_map<Socket::Id, size_t /*socket index*/>;
    using PollFds   = std::vector<struct pollfd>;

    Sockets    sockets_;
    PollFds    pollfds_; //< pollfds for the sockets. pollfds[i] is for sockets_[i - 1] because pollfds[i] is for the listenSocket
    SocketMap  socketMap_;
    int        listenFileDescriptor_ = TcpSocket::badSocketDescriptor;
    Socket::Id nextId = 1;
};

NETWORK_NAMESPACE_END

#endif // TCP_SERVER_H
