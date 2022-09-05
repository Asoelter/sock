NYLON_NAMESPACE_BEGIN

template <typename MessageDefiner>
Server<MessageDefiner>::Socket::Socket(net::TcpServer::Socket* tcpSocket, size_t startingBufferSize)
    : tcpSocket_(tcpSocket)
    , messageReader_(tcpSocket_, startingBufferSize)
    , messageWriter_(tcpSocket_)
{
}

template <typename MessageDefiner>
std::optional<typename Server<MessageDefiner>::MessageType>
Server<MessageDefiner>::Socket::read()
{
    return messageReader_.read();
}

template <typename MessageDefiner>
bool Server<MessageDefiner>::Socket::connected() const
{
    return tcpSocket_ && tcpSocket_->connected();
}

template <typename MessageDefiner>
Server<MessageDefiner>::Server(size_t startingBufferSize)
    : startingBufferSize_(startingBufferSize)
{
    tcpServer_.connectHandler = [this](net::TcpServer::Socket * s) {
        this->connectHandlerForwarder(s);
    };

    tcpServer_.readHandler = [this](net::TcpServer::Socket * s) {
        this->readHandlerForwarder(s);
    };

    tcpServer_.closeHandler = [this](net::TcpServer::Socket * s) {
        this->closeHandlerForwarder(s);
    };
}

template <typename MessageDefiner>
void Server<MessageDefiner>::poll()
{
    tcpServer_.poll();
}

template <typename MessageDefiner>
void Server<MessageDefiner>::listen(unsigned port)
{
    tcpServer_.listen(port);
}

template <typename MessageDefiner>
void Server<MessageDefiner>::connectHandlerForwarder(net::TcpServer::Socket * socket)
{
    if (!connectHandler) {
        assert(!"missing connect handler!");
        throw std::runtime_error("missing connect handler!");
    }

    auto [newSocketIt, inserted] = socketMap_.emplace(socket->id(), Socket(socket, startingBufferSize_));
    assert(inserted);
    connectHandler(&newSocketIt->second);
}

template <typename MessageDefiner>
void Server<MessageDefiner>::readHandlerForwarder(net::TcpServer::Socket * socket)
{
    if (!readHandler) {
        assert(!"missing read handler!");
        throw std::runtime_error("missing read handler!");
    }

    auto nylonSocketIt = socketMap_.find(socket->id());

    if (nylonSocketIt == socketMap_.end()) {
        assert(!"readHandlerForwarder called on unknown socket\n");
        throw std::runtime_error("readHandlerForwarder called on unknown socket\n");
    }

    while (nylonSocketIt->second.connected()) { // read until read fails (no more messages)
        auto msg = nylonSocketIt->second.read();

        if (!msg) {
            break;
        }

        readHandler(std::move(msg.value()));
    }
}

template <typename MessageDefiner>
void Server<MessageDefiner>::closeHandlerForwarder(net::TcpServer::Socket * socket)
{
    auto socketIt = socketMap_.find(socket->id());
    assert(socketIt != socketMap_.end());

    if (closeHandler) {
        closeHandler(&socketIt->second);
    }
    else {
        assert(!"closeHandlerForwarder called on unknown socket\n");
        throw std::runtime_error("closeHandlerForwarder called on unknown socket\n");
    }

    socketMap_.erase(socketIt);
}

NYLON_NAMESPACE_END
