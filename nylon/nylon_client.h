#ifndef NYLON_CLIENT_H
#define NYLON_CLIENT_H

#include "../network/tcp_socket.h"

#include "namespace.h"
#include "nylon_message_reader.h"
#include "nylon_message_writer.h"

#include <functional>
#include <optional>
#include <vector>

NYLON_NAMESPACE_BEGIN

template <typename MessageDefiner>
class NylonClient
{
public:
    using MessageHandler = std::function<void(typename MessageDefiner::MessageType&&)>;
    using CloseHandler = std::function<void()>;

    struct Params
    {
        size_t bufferSize;
        std::optional<std::string> logFileName;
    };

    NylonClient(Params const & params);

    void connect(const char * address, unsigned port);
    void poll();

    template <typename MessageType>
    void send(MessageType const & message);

    MessageHandler messageHandler;
    CloseHandler closeHandler;

private:
    using Buffer            = std::vector<unsigned char>;
    using MessageSender     = MessageWriter<MessageDefiner, net::TcpSocket>;
    using MessageReceiver   = MessageReader<MessageDefiner, net::TcpSocket>;

    template<typename T>
    using Defered = std::optional<T>;

    size_t bufferSize_;
    Defered<net::TcpSocket>    tcpSocket_; //< This blocks when constructed, so we can't do it in our constructor
    Defered<MessageReceiver>   messageReceiver_;
    Defered<MessageSender>     messageSender_;
    std::optional<std::string> logFileName;
};

NYLON_NAMESPACE_END

#include "nylon_client_inline.h"

#endif // NYLON_CLIENT_H
