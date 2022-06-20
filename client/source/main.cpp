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
#include "nylon/nylon_message.h"
#include "nylon/nylon_client.h"

int main()
{
    auto client = nylon::NylonClient(10 * nylon::maxMessageSize);

    client.connect("127.0.0.1", 16492);

    client.messageHandler = [](nylon::Message&& m) {
        printf("received %s message\n", nylon::nameOf(m));
    };

    unsigned msgCount = 1;

    while (true) {
        client.poll();
        auto payload = nylon::LogonAccepted();
        payload.sessionId = 67;
        client.send(payload);
        printf("sending message %u\n", msgCount++);
    }

    return 0;
}
