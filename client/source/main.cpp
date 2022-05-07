#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstring>

#include <arpa/inet.h>
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

    auto const socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);

    auto serverAddress = zero_init<sockaddr_in>();

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(16491);
    inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);

    printf("address: %u\n", serverAddress.sin_addr.s_addr);

    auto const result = connect(socketFileDescriptor,
                                reinterpret_cast<sockaddr *>(&serverAddress),
                                sizeof(serverAddress));

    if (result < 0) {
        printf("error: connect failed: %i\n", errno);
        exit(-1);
    }

    for (int i = 0; i < 5; ++i) {
        write(socketFileDescriptor, "hello", 5);

        unsigned char buffer[3];
        read(socketFileDescriptor, buffer, 3);
        printf("buffer: %s\n", buffer);
    }

    return 0;
}
