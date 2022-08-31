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

#include "nylon/nylon_server.h"

//#define USE_OLD

#ifdef USE_OLD
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

        for (auto* socket : sockets) {
            socket->write("\0", 1);
        }
    }

    return 0;
}

#else

int main()
{
    auto server = nylon::Server(15 * nylon::maxMessageSize);

    server.listen(16492);

    server.connectHandler = [](nylon::Server::Socket*) {
        printf("connected!\n");
    };

    server.readHandler = [](nylon::Message&& msg) {
        if (std::holds_alternative<nylon::HeartBeat>(msg)) {
            printf("MessageType: HeartBeat\n");
        }
        else if (std::holds_alternative<nylon::Logon>(msg)) {
            printf("MessageType: Logon\n");
        }
        else if (std::holds_alternative<nylon::LogonAccepted>(msg)) {
            auto la = std::get<nylon::LogonAccepted>(msg);
            printf("MessageType: LogonAccepted\n");
            printf("\tsessionId: %u\n", la.sessionId);
        }
        else if (std::holds_alternative<nylon::Text>(msg)) {
            auto const tm = std::get<nylon::Text>(msg);
            printf("messageType: Text\n");
            printf("\ttextSize: %u\n", static_cast<unsigned>(tm.textSize));
            printf("\ttext    : %s\n", tm.text.c_str());
        }
        else {
            assert(!"unrecognized message type");
        }

        printf("\n");
    };

    server.closeHandler = [](nylon::Server::Socket*) {
        printf("close handler called\n");
    };

    while (true) {
        server.poll();
    }

    return 0;
}
#endif
