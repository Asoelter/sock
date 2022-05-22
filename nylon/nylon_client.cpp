#include "nylon_client.h"

NYLON_NAMESPACE_BEGIN

NylonClient::NylonClient(size_t bufferSize)
    : sendBuffer_(bufferSize)
    , receiveBuffer_(bufferSize)
    , tcpSocket_()
{

}

void NylonClient::connect(const char * address, unsigned port)
{
    tcpSocket_ = net::createTcpClient(address, port);
}

void NylonClient::poll()
{
    // TODO(asoelter): do once we have a decoder
}

void NylonClient::send(Message const & message)
{

}

NYLON_NAMESPACE_END
