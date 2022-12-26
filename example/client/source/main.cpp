#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstring>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <unistd.h>

#include "nylon/nylon_client.h"
#include "nylon/nylon_test_messages.h"

int main()
{
    using Client = nylon::NylonClient<nylon::TestMessageDefiner>;

    auto const clientParams = Client::Params {
        .bufferSize = 50,
        .logFileName = "test.txt"
    };

    auto client = Client(clientParams);

    client.connect("127.0.0.1", 16492);

    client.messageHandler = [](nylon::Message&& /*m*/) {
        // TODO(asoelter): give this message more info again
        printf("received a message\n");
    };

    unsigned msgCount = 1;

    while (msgCount < 5) {
        client.poll();

        if (msgCount % 4 == 0){
            auto la = nylon::LogonAccepted();
            la.sessionId = 67;
            client.send(la);
        }
        else if (msgCount % 3) {
            auto const lo = nylon::Logon();
            client.send(lo);
        }
        else {
            auto tm = nylon::Text();
            tm.text = "hello world";
            client.send(tm);
        }

        printf("sending message %u\n", msgCount++);
    }

    return 0;
}
