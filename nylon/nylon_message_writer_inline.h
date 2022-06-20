template<typename SocketType>
MessageWriter<SocketType>::MessageWriter(SocketType* socket)
    : socket_(socket)
{

}

template<typename SocketType>
void MessageWriter<SocketType>::write(Message const & msg)
{
    auto const msgType = typeOf(msg);

    if (msgType == MessageType::HeartBeat) {
        handleMessage<HeartBeat>(msg);
        return;
    }
    else if(msgType == MessageType::Logon) {
        handleMessage<Logon>(msg);
        return;
    }
    else if(msgType == MessageType::LogonAccepted) {
        handleMessage<LogonAccepted>(msg);
        return;
    }

    printf("unknown message type\n");
    assert(!"unknown message type");
}

template<typename SocketType>
void MessageWriter<SocketType>::growBuffer(size_t size)
{
    auto const wasEmpty      = buffer_.empty();
    auto const beginDistance = std::distance(&*buffer_.begin(), begin_);
    auto const endDistance   = std::distance(&*buffer_.begin(), end_);
    assert(beginDistance >= 0);
    assert(endDistance >= 0);

    buffer_.resize(buffer_.size() + size);

    if (wasEmpty) {
        begin_ = &*buffer_.begin();
        end_   = &*buffer_.begin();
    }
    else {
        // Don't keep original invalidated iterators
        // after resize
        begin_ = &*buffer_.begin() + beginDistance;
        end_   = &*buffer_.begin() + endDistance;
    }
}

template<typename SocketType>
void MessageWriter<SocketType>::shiftForward()
{
    auto const occupiedSize = std::distance(begin_, end_);
    std::copy(&*buffer_.begin(), begin_, end_);
    begin_ = &*buffer_.begin();
    end_ = begin_ + occupiedSize;
}

template<typename SocketType>
template<typename MsgType>
void MessageWriter<SocketType>::handleMessage(Message const & msg)
{
    if (buffer_.empty()) {
        // Don't do pointer arithmetic with empty vector begin and end
        growBuffer(MsgType::size);
    }

    size_t const spaceAtBackOfBuffer = buffer_.size() - (end_ - &*buffer_.begin());

    if (spaceAtBackOfBuffer < MsgType::size) {

        size_t const spaceAtFrontOfBuffer = begin_ - (&*buffer_.begin());

        if (spaceAtFrontOfBuffer > MsgType::size) {
            shiftForward();
        }
        else {
            growBuffer(MsgType::size);
        }
    }

    auto const m = std::get<MsgType>(msg);

    auto encodedSize = MsgType::size;

    m.encode(&end_, encodedSize);
    assert(encodedSize == 0 && "Not all bytes were encoded");

    assert(socket_->connected());
    auto const bytesWritten = socket_->write(begin_, end_ - begin_);
    assert(bytesWritten >= 0);

    begin_ += bytesWritten;
}
