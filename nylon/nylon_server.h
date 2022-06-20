#ifndef NYLON_SERVER_H
#define NYLON_SERVER_H

#include "network/tcp_server.h"

#include "namespace.h"
#include "nylon_message_reader.h"
#include "nylon_message_writer.h"

#include <vector>

NYLON_NAMESPACE_BEGIN

class Server
{
public:
    using MessageHandler = std::function<void(Message&&)>;

    Server(size_t startingBufferSize);

    void poll();
    void listen(unsigned port);

    MessageHandler messageHandler;

private:
    class Socket
    {
    public:
        Socket(net::TcpServer::Socket* tcpSocket, size_t startingBufferSize);
    private:
        net::TcpServer::Socket*               tcpSocket_;
        MessageReader<net::TcpServer::Socket> messageReader_;
        MessageWriter<net::TcpServer::Socket> messageWriter_;
    };

    net::TcpServer tcpServer_;
    std::vector<Socket> sockets_;
    size_t startingBufferSize_;
    unsigned nextId_;
};

NYLON_NAMESPACE_END

#endif // NYLON_SERVER_H

