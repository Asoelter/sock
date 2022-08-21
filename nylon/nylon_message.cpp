#include "nylon_message.h"

#include <cstring>
#include <stdexcept>

NYLON_NAMESPACE_BEGIN

void HeartBeat::encode(char ** buffer, size_t& size) const
{
    **buffer = static_cast<char>(messageType);
    ++(*buffer);
    --size;
}

/*static*/ HeartBeat HeartBeat::decode(char const * buffer, size_t& bufferPos)
{
    auto const decodedMessageType = *buffer;

    if (decodedMessageType != static_cast<char>(messageType)) {
        throw std::runtime_error("HeartBeat::decode asked to decode a non-HeartBeat");
    }

    ++buffer;
    ++bufferPos;

    return {};
}

void Logon::encode(char ** buffer, size_t& size) const
{
    **buffer = static_cast<char>(messageType);
    ++(*buffer);
    --size;
}

/*static*/ Logon Logon::decode(char const * buffer, size_t& bufferPos)
{
    auto const decodedMessageType = *buffer;

    if (decodedMessageType != static_cast<char>(messageType)) {
        throw std::runtime_error("Logon::decode asked to decode a non-Logon");
    }

    ++buffer;
    ++bufferPos;

    return {};
}

void LogonAccepted::encode(char ** buffer, size_t& size) const
{
    **buffer = static_cast<char>(messageType);
    ++(*buffer);
    --size;

    memcpy(*buffer, &sessionId, sizeof(uint8_t));

    *buffer += sizeof(uint8_t);
    size    -= sizeof(uint8_t);
}

/*static*/ LogonAccepted LogonAccepted::decode(char const * buffer, size_t& bufferPos)
{
    auto const decodedMessageType = *buffer;

    if (decodedMessageType != static_cast<char>(messageType)) {
        throw std::runtime_error("Logon::decode asked to decode a non-Logon");
    }

    ++buffer;
    ++bufferPos;

    auto result = LogonAccepted();

    memcpy(&result.sessionId, buffer, sizeof(uint8_t));

    buffer    += sizeof(uint8_t);
    bufferPos += sizeof(uint8_t);

    return result;
}

void Text::encode(char ** buffer, size_t& size) const
{
    assert(size >= sizeof(messageType) + text.size());

    **buffer = static_cast<char>(messageType);
    ++(*buffer);
    --size;

    // Note(asoelter): we're currently restricting text.size to
    // be 256 or less
    assert(text.size() <= std::numeric_limits<char>::max());
    if (text.size() > std::numeric_limits<char>::max()) {
        throw std::runtime_error("Attempt to encode a text message of greater than 256 bytes");
    }

    **buffer = static_cast<char>(text.size());
    ++(*buffer);
    --size;

    memcpy(*buffer, &text, text.size());

    *buffer += text.size();
    size    -= text.size();
}

/*static*/ Text Text::decode(char const * buffer, size_t& bufferPos)
{
    auto const decodedMessageType = *buffer;
    ++buffer;
    ++bufferPos;

    assert(decodedMessageType == static_cast<char>(messageType));
    if (decodedMessageType != static_cast<char>(messageType)) {
        throw std::runtime_error("Text::decode asked to decode a non-Text");
    }

    auto const textSize = static_cast<size_t>(*buffer);
    ++buffer;
    ++bufferPos;

    auto result = Text();

    result.text = std::string_view(buffer, textSize);

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
        || typeAsSizeT == static_cast<size_t>(MessageType::LogonAccepted)
        || typeAsSizeT == static_cast<size_t>(MessageType::Text));

    return static_cast<MessageType>(typeAsSizeT);
}

const char * nameOf(const Message& message)
{
    auto const type = typeOf(message);

    switch(type) {
        case MessageType::HeartBeat:
        {
            return "HeartBeat";
        } break;
        case MessageType::Logon:
        {
            return "Logon";
        } break;
        case MessageType::LogonAccepted:
        {
            return "LogonAccepted";
        } break;
        case MessageType::Text:
        {
            return "Text";
        } break;
    };

    printf("unknown message type: %u\n", static_cast<unsigned>(type));
    assert(!"unknown message type");
    return "";
}

NYLON_NAMESPACE_END
