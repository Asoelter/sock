#include "nylon_client.h"

NYLON_NAMESPACE_BEGIN

NylonClient::NylonClient(size_t bufferSize)
    : bufferSize_(bufferSize)
    , sendBuffer_(bufferSize)
    , tcpSocket_()
{

}

void NylonClient::connect(const char * address, unsigned port)
{
    tcpSocket_ = net::createTcpClient(address, port);

    messageReader_ = MessageReader(&*tcpSocket_, bufferSize_);
    messageWriter_ = MessageWriter(&*tcpSocket_);
}

void NylonClient::poll()
{
    if (!tcpSocket_ && closeHandler) {
        closeHandler();
        return;
    }

    if (tcpSocket_ && !tcpSocket_->connected() && closeHandler) {
        closeHandler();
        return;
    }

    if (!messageReader_) {
        // TODO(asoelter): log not print
        printf("poll called without message reader\n");
        return;
    }

    while (true) { // read until read fails (no more messages)
        auto msg = messageReader_->read();

        if (msg) {
            messageHandler(std::move(msg.value()));
        }
        else {
            break;
        }
    }
}

void NylonClient::send(Message const & message)
{
    if (!messageWriter_) {
        // TODO(asoelter): log not print
        printf("send called without message reader\n");
        return;
    }

    messageWriter_->write(message);
}

NYLON_NAMESPACE_END
