#include "network/tcp_server.h"

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <vector>

#include <fcntl.h>
#include <netinet/ip.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

int main()
{
    auto server = net::TcpServer();
    auto sockets = std::vector<net::TcpServer::Socket*>();

    server.connectHandler = [&sockets](net::TcpServer::Socket * const socket) {
        printf("Connected!\n");
        sockets.push_back(socket);
    };

    server.readHandler = [](net::TcpServer::Socket * const socket) {
        printf("Ready to read!\n");
        char buffer[5];
        auto const bytesRead = socket->read(buffer, 5);

        printf("readHandler: bytesRead: %li, read %s\n", bytesRead, buffer);

        if (bytesRead > 0) {
            printf("\tbytes as ints:\n");

            for (int i = 0; i < bytesRead; ++i) {
                printf("\t\tbyte[0] = %i\n", static_cast<int>(buffer[i]));
            }
        }
    };

    server.closeHandler = [&sockets](net::TcpServer::Socket * const s) {
        printf("Closing...\n");
        sockets.erase(std::remove(sockets.begin(), sockets.end(), s));
    };

    server.listen(16492);

    while (true) {
        server.poll();
    }

    return 0;
}
