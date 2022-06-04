#ifndef NYLON_CLIENT_H
#define NYLON_CLIENT_H

#include "../network/tcp_socket.h"

#include "namespace.h"
#include "nylon_message.h"
#include "nylon_message_reader.h"
#include "nylon_message_writer.h"

#include <functional>
#include <optional>
#include <vector>

NYLON_NAMESPACE_BEGIN

class NylonClient
{
public:
    using MessageHandler = std::function<void(Message&&)>;
    using CloseHandler = std::function<void()>;

    NylonClient(size_t bufferSize);

    void connect(const char * address, unsigned port);
    void poll();
    void send(Message const & message);

    MessageHandler messageHandler;
    CloseHandler closeHandler;

private:
    using Buffer = std::vector<unsigned char>;

    template<typename T>
    using Defered = std::optional<T>;

    size_t bufferSize_;
    Buffer sendBuffer_;
    Defered<net::TcpSocket> tcpSocket_; //< This blocks when constructed, so we can't do it in our constructor
    Defered<MessageReader<net::TcpSocket>> messageReader_;
    Defered<MessageWriter<net::TcpSocket>> messageWriter_;
};

NYLON_NAMESPACE_END

#endif // NYLON_CLIENT_H
