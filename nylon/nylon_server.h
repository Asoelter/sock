#ifndef NYLON_SERVER_H
#define NYLON_SERVER_H

#include "../network/tcp_server.h"

#include "namespace.h"
#include "nylon_message_reader.h"
#include "nylon_message_writer.h"

#include <vector>

NYLON_NAMESPACE_BEGIN

class Server
{
public:
    class Socket
    {
    public:
        using Id = net::TcpServer::Socket::Id;

        Socket(net::TcpServer::Socket* tcpSocket, size_t startingBufferSize);

        std::optional<Message> read();
        bool connected() const;
    private:
        net::TcpServer::Socket*               tcpSocket_;
        MessageReader<net::TcpServer::Socket> messageReader_;
        MessageWriter<net::TcpServer::Socket> messageWriter_;
    };

    using ConnectHandler = std::function<void(Socket*)>;
    using CloseHandler = std::function<void(Socket*)>;
    using ReadHandler = std::function<void(Message&&)>;

    Server(size_t startingBufferSize);

    void poll();
    void listen(unsigned port);

    ConnectHandler  connectHandler;
    ReadHandler     readHandler;
    CloseHandler    closeHandler;

private:
    void connectHandlerForwarder(net::TcpServer::Socket * socket);
    void readHandlerForwarder(net::TcpServer::Socket * socket);
    void closeHandlerForwarder(net::TcpServer::Socket * socket);

private:
    using SocketMap = std::unordered_map<Socket::Id, Socket>;

    net::TcpServer  tcpServer_;
    SocketMap       socketMap_;
    size_t          startingBufferSize_;
    unsigned        nextId_;
};

NYLON_NAMESPACE_END

#endif // NYLON_SERVER_H

