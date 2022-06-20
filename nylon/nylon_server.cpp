#include "nylon_server.h"

NYLON_NAMESPACE_BEGIN

Server::Server(size_t startingBufferSize)
    : startingBufferSize_(startingBufferSize)
{

}

Server::Socket::Socket(net::TcpServer::Socket* tcpSocket, size_t startingBufferSize)
    : tcpSocket_(tcpSocket)
    , messageReader_(tcpSocket_, startingBufferSize)
    , messageWriter_(tcpSocket_)
{

}

NYLON_NAMESPACE_END
