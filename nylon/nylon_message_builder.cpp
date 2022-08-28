#include "nylon_message_builder.h"

#include "nylon_message.h"

NYLON_NAMESPACE_BEGIN

template <typename MessageType>
MessageBuilder::State MessageBuilder::build(char const * buffer, size_t& bufferPos, size_t bufferSize)
{
    assert(state_ != MessageBuilder::State::Finished && "there is still a message to extract");

    switch (state_) {
        case MessageBuilder::State::NotStarted:
        {
            message_ = MessageType(); // reset message
            bytesAlreadyEncoded_ = 0;
            state_ = detail::MessageBuilderImpl<MessageType>::build(message_.value(), buffer, bufferPos, bufferSize, bytesAlreadyEncoded_);
        } break;
        case MessageBuilder::State::Building:
        {
            assert(message_.has_value());

            state_ = detail::MessageBuilderImpl<MessageType>::build(message_.value(), buffer, bufferPos, bufferSize, bytesAlreadyEncoded_);
        } break;
        case MessageBuilder::State::Finished:
        {
            throw std::runtime_error("there is still a message to extract, we're losing messages!");
        } break;
    }

    return state_;
}

bool MessageBuilder::isBuilding(MessageType messageType)
{
    if (!message_) {
        return false;
    }

    return typeOf(message_.value()) == messageType;
}

Message MessageBuilder::finalizeMessage()
{
    assert(message_.has_value());
    assert(state_ == State::Finished && "attempt to finalize a message that is still being build");

    if (!message_) {
        throw std::runtime_error("attempt to finalize a message when one hasn't been built yet");
    }

    if (state_ != State::Finished) {
        throw std::runtime_error("attempt to finalize a message that is still being build");
    }

    state_ = State::NotStarted; // prepare for next message

    auto const result = message_.value();
    message_ = std::nullopt;

    return result;
}

MessageBuilder::State MessageBuilder::state() const noexcept
{
    return state_;
}

namespace detail {

template <>
struct MessageBuilderImpl<HeartBeat>
{
    static MessageBuilder::State build(Message& message,
                                       char const * buffer,
                                       size_t& bufferPos,
                                       size_t bufferSize,
                                       unsigned& bytesAlreadyEncoded)
    {
        assert(bytesAlreadyEncoded < HeartBeat::size);
        assert(bytesAlreadyEncoded == HeartBeat::messageTypeOffset);
        assert(message.index() == static_cast<size_t>(HeartBeat::messageType));

        if (bytesAlreadyEncoded != HeartBeat::messageTypeOffset) {
            throw std::runtime_error("HeartBeat::decode has invalid state");
        }

        auto const decodedMessageType = *buffer;

        if (decodedMessageType != static_cast<char>(HeartBeat::messageType)) {
            throw std::runtime_error("HeartBeat::decode asked to decode a non-HeartBeat");
        }

        ++buffer;
        ++bytesAlreadyEncoded;
        ++bufferPos;

        return MessageBuilder::State::Finished;
    }
};

template <>
struct MessageBuilderImpl<Logon>
{
    static MessageBuilder::State build(Message& message,
                                       char const * buffer,
                                       size_t& bufferPos,
                                       size_t bufferSize,
                                       unsigned& bytesAlreadyEncoded)
    {
        assert(bytesAlreadyEncoded < Logon::size);
        assert(bytesAlreadyEncoded == Logon::messageTypeOffset);
        assert(message.index() == static_cast<size_t>(Logon::messageType));

        if (bytesAlreadyEncoded != Logon::messageTypeOffset) {
            throw std::runtime_error("Logon::decode has invalid state");
        }

        auto const decodedMessageType = *buffer;

        if (decodedMessageType != static_cast<char>(Logon::messageType)) {
            throw std::runtime_error("Logon::decode asked to decode a non-HeartBeat");
        }

        ++buffer;
        ++bytesAlreadyEncoded;
        ++bufferPos;

        return MessageBuilder::State::Finished;
    }
};

template <>
struct MessageBuilderImpl<LogonAccepted>
{
    static MessageBuilder::State build(Message& message,
                                       char const * buffer,
                                       size_t& bufferPos,
                                       size_t bufferSize,
                                       unsigned& bytesAlreadyEncoded)
    {
        assert(bytesAlreadyEncoded < LogonAccepted::size);
        assert(message.index() == static_cast<size_t>(LogonAccepted::messageType));
        auto newState = MessageBuilder::State::Building;

        if (bytesAlreadyEncoded == LogonAccepted::messageTypeOffset) {
            auto constexpr bytesNeededForThisField = 1;

            if (bufferSize < bytesNeededForThisField) {
                return newState;
            }

            auto const decodedMessageType = *buffer;

            if (decodedMessageType != static_cast<char>(LogonAccepted::messageType)) {
                throw std::runtime_error("LogonAccepted::decode asked to decode a non-HeartBeat");
            }

            ++buffer;
            ++bytesAlreadyEncoded;
            ++bufferPos;
            --bufferSize;
        }

        auto& la = std::get<LogonAccepted>(message);

        if (bytesAlreadyEncoded == LogonAccepted::sessionIdOffset) {
            auto constexpr bytesNeededForThisField = 1;

            if (bufferSize < bytesNeededForThisField) {
                return newState;
            }

            memcpy(&la.sessionId, buffer, sizeof(uint8_t));

            buffer              += sizeof(la.sessionId);
            bytesAlreadyEncoded += sizeof(la.sessionId);
            bufferPos           += sizeof(la.sessionId);
            bufferSize          -= sizeof(la.sessionId);
            newState             = MessageBuilder::State::Finished;
        }

        return newState;
    }
};

} // namespace detail

template MessageBuilder::State MessageBuilder::build<HeartBeat>(char const * buffer, size_t& bufferPos, size_t bufferSize);
template MessageBuilder::State MessageBuilder::build<Logon>(char const * buffer, size_t& bufferPos, size_t bufferSize);
template MessageBuilder::State MessageBuilder::build<LogonAccepted>(char const * buffer, size_t& bufferPos, size_t bufferSize);

NYLON_NAMESPACE_END
