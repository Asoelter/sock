#ifndef NYLON_MESSAGE_WRITER_H
#define NYLON_MESSAGE_WRITER_H

#include "../network/tcp_socket.h"

#include "nylon_message.h"

#include "namespace.h"

#include <string>
#include <vector>

NYLON_NAMESPACE_BEGIN

template<typename SocketType = net::TcpSocket>
class MessageWriter
{
public:
    MessageWriter(SocketType* socket);

    void write(Message const & msg);

private:
    void growBuffer(size_t amount);
    void shiftForward();

    template<typename MsgType>
    void handleMessage(Message const & msg);

private:
    std::vector<char>   buffer_;
    char*               begin_;  //< begin of ring buffer
    char*               end_;    //< end of ring buffer
    SocketType*         socket_;
};

#include "nylon_message_writer_inline.h"

NYLON_NAMESPACE_END

#endif // NYLON_MESSAGE_WRITER_H
