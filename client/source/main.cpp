#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstring>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <unistd.h>

#include "network/tcp_socket.h"

int main()
{
    auto socket = net::createTcpClient("127.0.0.1", 16491);
    char buffer[3];

    while (socket.connected()) {
        socket.write("hello", 5);
        auto const bytesRead = socket.read(buffer, 3);

        if (bytesRead > 0) {
            printf("buffer: %s\n", buffer);
        }
    }

    return 0;
}
