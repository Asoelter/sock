#ifndef MESSAGE_BUILDER_H
#define MESSAGE_BUILDER_H

#include "nylon_message.h"
#include "namespace.h"

#include <optional>

NYLON_NAMESPACE_BEGIN

namespace detail {
template <typename MessageType>
struct MessageBuilderImpl;
}

class MessageBuilder
{
public:
    enum class State
    {
        NotStarted,
        Building,
        Finished
    };

    MessageBuilder() = default;

    template <typename MessageType>
    State build(char const * buffer, size_t& bufferPos, size_t bufferSize);

    bool isBuilding(MessageType messageType);

    [[nodiscard]] Message finalizeMessage();

    State state() const noexcept;

private:
    using MaybeMessage = std::optional<Message>;

    MaybeMessage  message_;
    State         state_ = State::NotStarted;
    unsigned      bytesAlreadyEncoded_ = 0;
};

NYLON_NAMESPACE_END

#endif // MESSAGE_BUILDER_H
