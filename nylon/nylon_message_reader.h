#ifndef NYLON_MESSAGE_READER_H
#define NYLON_MESSAGE_READER_H

#include "../network/tcp_socket.h"

#include "namespace.h"
#include "nylon_message.h"
#include "nylon_message_builder.h"

#include <array>
#include <optional>
#include <vector>

NYLON_NAMESPACE_BEGIN

// Use ring buffer to store data read from
// the TCP socket and then decode that data
// into Nylon messages
template <typename SocketType = net::TcpSocket> //< templated so we can use a debug socket for testing
class MessageReader
{
public:
    MessageReader(SocketType* socket, size_t bufferSize);

    std::optional<Message> read();

private:
    void rollover();        //< move reading space to front of buffer

private:
    SocketType* socket_;
    std::vector<char> buffer_;
    MessageBuilder messageBuilder_;
    size_t readOffset_;     //< how many bytes into the array we're reading at
    size_t decodeOffset_;   //< how many bytes into the array we're decoding a message at
};

#include "nylon_message_reader_inline.h"

NYLON_NAMESPACE_END

#endif // NYLON_MESSAGE_READER_H
