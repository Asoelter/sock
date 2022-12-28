NYLON_NAMESPACE_BEGIN

bool operator==(SurveyMessageData const & lhs, SurveyMessageData const & rhs)
{
    return lhs.messageType == rhs.messageType;
}

template <typename MessageDefiner>
void Surveyor<MessageDefiner>::survey(std::span<const char> buffer)
{
    auto bytesSurveyed = 0lu;
    auto const bufferSize = buffer.size(); // cache, otherwise the size will change with each subspan

    while (bytesSurveyed < bufferSize) {
        auto const & [maybeMessage, bytesDecoded] = messageBuilder_.build(buffer);

        bytesSurveyed += bytesDecoded;
        buffer = buffer.subspan(bytesDecoded);

        if (maybeMessage) {
            // This is so clunky. Need to wrap the message variant in a proper
            // class next so I can have a member function for this
            auto messageType = MessageTypeT();
            auto messageName = std::string();

            std::visit([&messageType, &messageName](auto const & msg) {
                    messageType = msg.messageType;
                    messageName = msg.name();
            }, maybeMessage.value());

            auto [it, inserted] = messageCounts_.try_emplace({.messageType = messageType, .messageName = messageName}, 1 /*count*/);

            if (!inserted) {
                ++it->second; // increment count of message
            }
        }
    }
}

template <typename MessageDefiner>
typename Surveyor<MessageDefiner>::MessageCounts const &
Surveyor<MessageDefiner>::messageCounts() const noexcept
{
    return messageCounts_;
}

template <typename MessageDefiner>
template <typename MsgType>
size_t Surveyor<MessageDefiner>::messageCount() const
{
    // Kind of hacky, messageName isn't used in the equality comparison so it doesn't really matter
    // what value we give for it here
    auto const it = messageCounts_.find({.messageType = MsgType::messageType, .messageName = ""});

    if (it == messageCounts_.end()) {
        return 0;
    }

    return it->second;
}

NYLON_NAMESPACE_END
