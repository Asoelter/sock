#ifndef NYLON_MESSAGE_WRITER_H
#define NYLON_MESSAGE_WRITER_H

#include "../network/tcp_socket.h"

#include "namespace.h"
#include "nylon_file_writer.h"

#include <optional>
#include <string>
#include <vector>

NYLON_NAMESPACE_BEGIN

template<typename MessageDefiner, typename SocketType = net::TcpSocket>
class MessageWriter
{
public:
    struct Params
    {
        SocketType * socket;
        std::optional<std::string> logFileName;
    };

    MessageWriter(Params const & params);

    template <typename MessageType>
    void write(MessageType const & msg);

private:
    void growBuffer(size_t amount);
    void shiftForward();

    template<typename MessageType>
    void handleMessage(MessageType const & msg);

private:
    template <typename T>
    using Deferred = std::optional<T>;

    std::vector<char>    buffer_;
    char*                begin_;  //< begin of ring buffer
    char*                end_;    //< end of ring buffer
    SocketType*          socket_;
    Deferred<FileWriter> fileWriter_;
};

std::optional<FileWriter> maybeInitFileWriter(std::optional<std::string> const & logFileName);

NYLON_NAMESPACE_END

#include "nylon_message_writer_inline.h"

#endif // NYLON_MESSAGE_WRITER_H
