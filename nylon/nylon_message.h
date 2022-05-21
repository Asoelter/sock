#ifndef NYON_MESSAGE_H
#define NYON_MESSAGE_H

#include "namespace.h"

#include <cstddef>
#include <cstdint>
#include <variant>

NYLON_NAMESPACE_BEGIN

enum class MessageType : uint8_t
{
    HeartBeat,
    Logon,
    LogonAccepted
};

struct HeartBeat
{
    static constexpr auto messageType = MessageType::HeartBeat;
    static constexpr auto size = sizeof(uint8_t);

    void encode(char * buffer, size_t& size);
    static HeartBeat decode(char const * buffer, size_t& size);
};

struct Logon
{
    static constexpr auto messageType = MessageType::Logon;
    static constexpr auto size = sizeof(uint8_t);

    void encode(char * buffer, size_t& size);
    static Logon decode(char const * buffer, size_t& size);
};

struct LogonAccepted
{
    static constexpr auto messageType = MessageType::LogonAccepted;
    static constexpr auto size = sizeof(uint8_t) + sizeof(uint8_t);

    void encode(char * buffer, size_t& size);
    static LogonAccepted decode(char const * buffer, size_t& size);

    uint8_t sessionId;
};

using Message = std::variant<
    HeartBeat,
    Logon,
    LogonAccepted
>;

constexpr auto maxMessageSize = std::max({HeartBeat::size, Logon::size, LogonAccepted::size});

NYLON_NAMESPACE_END

#endif // NYON_MESSAGE_H
