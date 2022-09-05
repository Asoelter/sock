NYLON_NAMESPACE_BEGIN

template <typename MessageDefiner>
NylonClient<MessageDefiner>::NylonClient(size_t bufferSize)
    : bufferSize_(bufferSize)
    , sendBuffer_(bufferSize)
    , tcpSocket_()
{

}

template <typename MessageDefiner>
void NylonClient<MessageDefiner>::connect(const char * address, unsigned port)
{
    tcpSocket_ = net::createTcpClient(address, port);

    messageReceiver_ = MessageReceiver(&*tcpSocket_, bufferSize_);
    messageSender_ = MessageSender(&*tcpSocket_);
}

template <typename MessageDefiner>
void NylonClient<MessageDefiner>::poll()
{
    if (!tcpSocket_ && closeHandler) {
        closeHandler();
        return;
    }

    if (tcpSocket_ && !tcpSocket_->connected() && closeHandler) {
        closeHandler();
        return;
    }

    if (!messageReceiver_) {
        // TODO(asoelter): log not print
        printf("poll called without message reader\n");
        return;
    }

    while (true) { // read until read fails (no more messages)
        auto msg = messageReceiver_->read();

        if (msg) {
            messageHandler(std::move(msg.value()));
        }
        else {
            break;
        }
    }
}

template <typename MessageDefiner>
template <typename MessageType>
void NylonClient<MessageDefiner>::send(MessageType const & message)
{
    if (!messageSender_) {
        // TODO(asoelter): log not print
        printf("send called without message reader\n");
        return;
    }

    messageSender_->write(message);
}

NYLON_NAMESPACE_END
