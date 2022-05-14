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

template<typename T>
T zero_init()
{
    T value;
    memset(&value, 0, sizeof(T));

    return value;
}

#if 0
int main()
{
    printf("Hello world\n");

    auto const listenFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);

    auto serverAddress = zero_init<sockaddr_in>();
    auto clientAddress = zero_init<sockaddr_in>();

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(16491);

    auto result = bind(listenFileDescriptor,
                       reinterpret_cast<struct sockaddr *>(&serverAddress),
                       sizeof(serverAddress));

    if (result < 0) {
        printf("bind failed. error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    listen(listenFileDescriptor, 10 /*queue size*/);

    for ( ; ; ) {
        socklen_t clientSocketLength = sizeof(clientAddress); // can be overwritten below
        auto const connectionFileDescriptor = accept(listenFileDescriptor,
                                                     reinterpret_cast<sockaddr *>(&clientAddress),
                                                     &clientSocketLength);

        if (connectionFileDescriptor < 0) {
            printf("accpet failed. error: %i\n", errno);
            exit(0);
        }

        // make the socket non-blocking
        fcntl(connectionFileDescriptor, F_SETFL, O_NONBLOCK);

        auto const pid = fork();
        if (pid ==  0) { // child process
            close(listenFileDescriptor); // ref counted so child must delete when done (on startup)
            unsigned char buffer[5];
            
            while (true) {
                auto const bytesRead = read(connectionFileDescriptor, buffer, 5);

                if (bytesRead == -1) {
                    if (errno == EWOULDBLOCK) {
                        // no problem, just no data to read
                        continue;
                    }
                    else {
                        printf("server: connection disconnected\n");
                        break;
                    }
                }

                printf("buffer: %s\n", buffer);

                write(connectionFileDescriptor, "ack", 3);
            }

            close(connectionFileDescriptor);
            exit(0);
        }

        close(connectionFileDescriptor); // ref counted so parent must delete when done (as soon as child launched)
        printf("Parent launched child\n");
        waitpid(pid, nullptr, 0);
    }

    return 0;
}
#endif

#if 0
int main()
{
    printf("Hello world\n");

    auto const listenFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);

    auto serverAddress = zero_init<sockaddr_in>();
    auto clientAddress = zero_init<sockaddr_in>();

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(16491);

    auto result = bind(listenFileDescriptor,
            reinterpret_cast<struct sockaddr *>(&serverAddress),
            sizeof(serverAddress));

    if (result < 0) {
        printf("bind failed. error: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    listen(listenFileDescriptor, 10 /*queue size*/);

    auto pollfds = std::vector<struct pollfd>();
    pollfds.push_back({.fd = listenFileDescriptor, .events = POLLIN}); // POLLIN: tell me when they're ready to read

    unsigned char buffer[5];

    for ( ; ; ) {
        auto readyFds = poll(pollfds.data(), pollfds.size(), -1); // -1 --> wait forever
        auto fdsServiced = 0; // number of ready fds we've handled

        for (auto & pollfd : pollfds) {
            if (!static_cast<bool>(pollfd.revents & POLLIN)) { // This fd isn't ready
                continue;
            }

            // Now we know the fd is ready to read

            if (pollfd.fd == listenFileDescriptor) {
                socklen_t clientSocketLength = sizeof(clientAddress); // can be overwritten below
                auto const connectionFileDescriptor = accept(listenFileDescriptor,
                                                             reinterpret_cast<sockaddr *>(&clientAddress),
                                                             &clientSocketLength);

                pollfds.push_back({.fd = connectionFileDescriptor, .events = POLLIN}); // POLLIN: tell me when they're ready to read
            }
            else {
                auto const bytesRead = read(pollfd.fd, buffer, 5);

                if (bytesRead == -1) {
                    if (errno == EWOULDBLOCK) {
                        // no problem, just no data to read
                        continue;
                    }
                    else {
                        printf("server: connection disconnected\n");
                        break;
                    }
                }

                printf("buffer: %s\n", buffer);

                write(pollfd.fd, "ack", 3);
            }

            if (fdsServiced == readyFds) { // we've handled all ready sockets, no need to keep searching
                break;
            }
        }
    }

    return 0;
}
#endif

#define SELECT_SECOND_ARG(first, second, ...)   second
#define LOG_NO_ARGS(fmt)                        printf("%s(%u):" fmt "\n", __PRETTY_FUNCTION__, __LINE__);
#define LOG_ARGS(fmt, ...)                      printf("%s(%u):" fmt "\n" __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__);
#define LOG(fmt, ...)                           SELECT_SECOND_ARG(..., LOG_NO_ARGS(fmt), LOG_ARGS(fmt, ...))

int main()
{
    auto server = net::TcpServer();
    auto sockets = std::vector<net::TcpServer::Socket*>();

    server.connectHandler = [&sockets](net::TcpServer::Socket * const socket) {
        LOG("Connected!\n");
        sockets.push_back(socket);
    };

    server.readHandler = [](net::TcpServer::Socket * const socket) {
        printf("Ready to read!\n");
        char buffer[5];
        auto const bytesRead = socket->read(buffer, 5);

        if (!bytesRead) {
            assert(false);
        }

        printf("%s(%u):bytesRead: %li, read %s\n", __PRETTY_FUNCTION__, __LINE__, bytesRead, buffer);
    };

    server.closeHandler = [](net::TcpServer::Socket * const socket) {
        printf("Closing...\n");
        assert(false);
    };

    server.listen(16491);

    while (true) {
        server.poll();
    }

    return 0;
}
