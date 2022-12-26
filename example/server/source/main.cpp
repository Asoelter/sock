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
#include "nylon/nylon_test_messages.h"

int main()
{
    auto server = nylon::Server<nylon::TestMessageDefiner>(15 * 15);

    server.listen(16492);

    server.connectHandler = [](nylon::Server<nylon::TestMessageDefiner>::Socket*) {
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
            printf("\ttextSize: %lu\n",tm.text.size());
            printf("\ttext    : %s\n", tm.text.c_str());
        }
        else {
            assert(!"unrecognized message type");
        }

        printf("\n");
    };

    server.closeHandler = [](nylon::Server<nylon::TestMessageDefiner>::Socket*) {
        printf("close handler called\n");
    };

    while (true) {
        server.poll();
    }

    return 0;
}
