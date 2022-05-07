#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstring>

#include <fcntl.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <unistd.h>

template<typename T>
T zero_init()
{
    T value;
    memset(&value, 0, sizeof(T));

    return value;
}

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

            // TODO(asoelter): look at strecho
            close(connectionFileDescriptor);
            exit(0);
        }

        close(connectionFileDescriptor); // ref counted so parent must delete when done (as soon as child launched)
        printf("Parent launched child\n");
        waitpid(pid, nullptr, 0);
    }

    return 0;
}
