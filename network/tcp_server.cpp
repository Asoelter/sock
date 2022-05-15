#include "tcp_server.h"

#include "util.h"

#include <string>

#include <fcntl.h>
#include <netinet/ip.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

NETWORK_NAMESPACE_BEGIN

TcpServer::Socket::Socket(TcpServer* owner, int fd, const char * address, unsigned port)
    : owner_(owner)
    , socket_(fd, address, port)
{

}

long TcpServer::Socket::read(char * const buffer, size_t size)
{
    auto const bytesRead = socket_.read(buffer, size);

    if (bytesRead == TcpSocket::badRead || bytesRead == TcpSocket::socketClosed) {
        assert(owner_ && "socket survived longer than owning server");
        owner_->stopPollingFor(this);
    }

    return bytesRead;
}

void TcpServer::listen(unsigned port)
{
    listenFileDescriptor_ = socket(AF_INET, SOCK_STREAM, 0);

    auto serverAddress = zero_init<sockaddr_in>();

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(port);

    auto result = bind(listenFileDescriptor_,
                       reinterpret_cast<struct sockaddr *>(&serverAddress),
                       sizeof(serverAddress));

    if (result < 0) {
        throw std::runtime_error("TcpServer::listen: bind failed. errno: " + std::string(strerror(errno)));
    }

    ::listen(listenFileDescriptor_, 10 /*queue size*/);

    pollfds_.clear(); // in case this socket has listen()'d before

    pollfds_.push_back({.fd = listenFileDescriptor_, .events = POLLIN});
}

void TcpServer::poll()
{
    assert(listenFileDescriptor_ != TcpSocket::badSocketDescriptor);

    if (listenFileDescriptor_ == TcpSocket::badSocketDescriptor) {
        throw std::runtime_error("TcpServer::poll: listenFileDescriptor is bad");
    }

    auto clientAddress = zero_init<sockaddr_in>();
    auto const numReadyFds = ::poll(pollfds_.data(), pollfds_.size(), -1 /*wait forever*/);
    auto fdsServiced = 0;

    // raw loop to avoid iterator invalidation
    for (auto i = 0u; i < pollfds_.size(); ++i) {
        if (!static_cast<bool>(pollfds_[i].revents & POLLIN)) { // This fd isn't ready
            continue;
        }

        // Now we know the fd is ready to read

        if (pollfds_[i].fd == listenFileDescriptor_) {
            socklen_t clientSocketLength = sizeof(clientAddress); // can be overwritten below
            auto const connectionFileDescriptor = accept(listenFileDescriptor_,
                                                         reinterpret_cast<sockaddr *>(&clientAddress),
                                                         &clientSocketLength);

            pollfds_.push_back({.fd = connectionFileDescriptor, .events = POLLIN}); // POLLIN: tell me when they're ready to read

            // raw new because of private/friend constructor
            sockets_.push_back(std::unique_ptr<Socket>(new Socket(this, connectionFileDescriptor, nullptr, 0)));

            if (connectHandler) {
                connectHandler(sockets_.back().get());
            }
        }
        else if (readHandler) {
            assert(i <= sockets_.size());
            assert(sockets_[i - 1]);
            printf("reading socket %i\n", i);
            readHandler(sockets_[i - 1].get());
        }

        if (++fdsServiced == numReadyFds) { // serviced all ready fds. No need to keep searching
            break;
        }
    }
}

void TcpServer::stopPollingFor(Socket* socket)
{
    printf("stopped polling\n");
    // We could probably change our data structures
    // around to avoid having to loop through every
    // socket, but this is good for now

    for (auto i = 0u; i < sockets_.size(); ++i) {
        if (sockets_[i].get() == socket) {
            // before we do anything else, let the
            // client know this socket is closed
            if (closeHandler) {
                closeHandler(sockets_[i].get()); 
            }

            // Fast erase trick: We don't care about
            // the ordering of pollfds/sockets. We
            // just care that socket[i] is for pollfd[i + 1]
            // (i + 1 because pollfds[0] == listenFileDescriptor_
            // but the listenFileDescriptor_ is not present
            // in sockets_)
            // so, swap the last socket for this one
            // and pop_back, then do the same for
            // pollfds
            std::swap(sockets_[i], sockets_[sockets_.size() - 1]);
            sockets_.pop_back();

            std::swap(pollfds_[i + 1], pollfds_[pollfds_.size() - 1]);
            pollfds_.pop_back();
            return;
        }
    }

    // we shouldn't get here. It means we were called
    // on a socket that doesn't exist
    assert(false);
}

NETWORK_NAMESPACE_END
