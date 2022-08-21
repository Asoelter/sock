#ifndef NYON_MESSAGE_H
#define NYON_MESSAGE_H

#include "namespace.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <variant>

NYLON_NAMESPACE_BEGIN

// It's important that these are listed
// in the same order they appear in the
// variant at the end of the file
enum class MessageType : uint8_t
{
    HeartBeat,
    Logon,
    LogonAccepted,
    Text,
};

struct HeartBeat
{
    static constexpr auto messageType = MessageType::HeartBeat;
    static constexpr auto size        = sizeof(uint8_t);
    static constexpr auto fixedSize   = true;

    void encode(char ** buffer, size_t& size) const;
    [[nodiscard]] static HeartBeat decode(char const * buffer, size_t& bufferPos);
};

struct Logon
{
    static constexpr auto messageType = MessageType::Logon;
    static constexpr auto size = sizeof(uint8_t);
    static constexpr auto fixedSize   = true;

    void encode(char ** buffer, size_t& size) const;
    [[nodiscard]] static Logon decode(char const * buffer, size_t& bufferPos);
};

struct LogonAccepted
{
    static constexpr auto messageType = MessageType::LogonAccepted;
    static constexpr auto size = sizeof(uint8_t) + sizeof(uint8_t);
    static constexpr auto fixedSize   = true;

    void encode(char ** buffer, size_t& size) const;
    [[nodiscard]] static LogonAccepted decode(char const * buffer, size_t& bufferPos);

    uint8_t sessionId = 0;
};

struct Text
{
    static constexpr auto messageType = MessageType::Text;
    static constexpr auto size        = sizeof(uint8_t) + sizeof(uint8_t);
    static constexpr auto fixedSize   = false;
    static constexpr auto sizeOffset  = 1;

    void encode(char ** buffer, size_t& size) const;
    [[nodiscard]] static Text decode(char const * buffer, size_t& bufferPos);

    char        textSize = 0;
    std::string text;
};

// It's important that these are listed
// in the same order they appear in the
// enum at the top of the file
using Message = std::variant<
    HeartBeat,
    Logon,
    LogonAccepted,
    Text
>;

MessageType typeOf(const Message& message);
const char * nameOf(const Message& message);

constexpr auto maxMessageSize = std::max({HeartBeat::size, Logon::size, LogonAccepted::size});

NYLON_NAMESPACE_END

#endif // NYON_MESSAGE_H
