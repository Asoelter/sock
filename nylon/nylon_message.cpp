#include "nylon_message.h"

#include <cstring>
#include <stdexcept>

NYLON_NAMESPACE_BEGIN

void HeartBeat::encode(char * buffer, size_t& size)
{
    *buffer = static_cast<char>(messageType);
    ++buffer;
    --size;
}

/*static*/ HeartBeat HeartBeat::decode(char const * buffer, size_t& size)
{
    auto const decodedMessageType = *buffer;

    if (decodedMessageType != static_cast<char>(messageType)) {
        throw std::runtime_error("HeartBeat::decode asked to decode a non-HeartBeat");
    }

    ++buffer;
    ++size;

    return {};
}

void Logon::encode(char * buffer, size_t& size)
{
    *buffer = static_cast<char>(messageType);
    ++buffer;
    --size;
}

/*static*/ Logon Logon::decode(char const * buffer, size_t& size)
{
    auto const decodedMessageType = *buffer;

    if (decodedMessageType != static_cast<char>(messageType)) {
        throw std::runtime_error("Logon::decode asked to decode a non-Logon");
    }

    ++buffer;
    ++size;

    return {};
}

void LogonAccepted::encode(char * buffer, size_t& size)
{
    *buffer = static_cast<char>(messageType);
    ++buffer;
    --size;

    memcpy(buffer, &sessionId, sizeof(uint8_t));

    buffer += sizeof(uint8_t);
    size   -= sizeof(uint8_t);
}

/*static*/ LogonAccepted LogonAccepted::decode(char const * buffer, size_t& size)
{
    auto const decodedMessageType = *buffer;

    if (decodedMessageType != static_cast<char>(messageType)) {
        throw std::runtime_error("Logon::decode asked to decode a non-Logon");
    }

    ++buffer;
    ++size;

    auto result = LogonAccepted();

    memcpy(&result.sessionId, buffer, sizeof(uint8_t));

    buffer += sizeof(uint8_t);
    size   += sizeof(uint8_t);

    return result;
}

// Rely on the fact that message types are listed
// in the same order in the MessageType enum and
// in the Message variant. Unsafe, hence the assert
MessageType typeOf(const Message& message)
{
    auto const typeAsSizeT = message.index();

    assert(typeAsSizeT == static_cast<size_t>(MessageType::HeartBeat)
        || typeAsSizeT == static_cast<size_t>(MessageType::Logon)
        || typeAsSizeT == static_cast<size_t>(MessageType::LogonAccepted));

    return static_cast<MessageType>(typeAsSizeT);
}

NYLON_NAMESPACE_END
