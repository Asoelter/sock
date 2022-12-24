#ifndef NYLON_MESSAGE_READER_H
#define NYLON_MESSAGE_READER_H

#include "../network/tcp_socket.h"
#include "../util/typelist.h"

#include "namespace.h"
#include "nylon_message_builder.h"

#include <array>
#include <optional>
#include <vector>

NYLON_NAMESPACE_BEGIN

// Use ring buffer to store data read from
// the TCP socket and then decode that data
// into Nylon messages
template <typename MessageDefiner, typename SocketType = net::TcpSocket> //< templated so we can use a debug socket for testing
class MessageReader
{
public:
    using MessageType = typename MessageDefiner::MessageType;
    using MessageTypeOptional = typename std::optional<MessageType>;

    MessageReader(SocketType* socket, size_t bufferSize);

    MessageTypeOptional read();

private:
    void rollover();        //< move reading space to front of buffer

    template <typename List>
    MessageTypeOptional handleMessage(char messageType);

private:
    SocketType* socket_;
    std::vector<char> buffer_;
    MessageBuilder<MessageDefiner> messageBuilder_;
    size_t readOffset_;     //< how many bytes into the array we're reading at
    size_t decodeOffset_;   //< how many bytes into the array we're decoding a message at
};

NYLON_NAMESPACE_END

#include "nylon_message_reader_inline.h"

#endif // NYLON_MESSAGE_READER_H
