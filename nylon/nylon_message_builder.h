#ifndef MESSAGE_BUILDER_H
#define MESSAGE_BUILDER_H

#include "nylon_message.h"
#include "nylon_message_decoder.h"
#include "namespace.h"

#include <optional>

NYLON_NAMESPACE_BEGIN

template <typename MessageDefiner>
class MessageBuilder
{
public:
    using MessageType = typename MessageDefiner::MessageType;

    enum class State
    {
        NotStarted,
        Building,
        Finished
    };

    MessageBuilder() = default;

    template <typename MessageT>
    State build(char const * buffer, size_t& bufferPos, size_t bufferSize);

    template <typename MessageT>
    bool isBuilding();

    [[nodiscard]] MessageType finalizeMessage();

    State state() const noexcept;

private:
    template <typename ConcreteMessageType>
    MessageDecoderContext<ConcreteMessageType>& context();

    static State translateDecoderState(DecoderState decoderState);

private:
    using MessageTypeOptional = std::optional<MessageType>;
    using DecoderContext = typename MessageDefiner::DecoderContext;
    using DecoderContextOptional = std::optional<DecoderContext>;

    DecoderContextOptional decoderContext_;
    State                  state_ = State::NotStarted;
    unsigned               bytesAlreadyEncoded_ = 0;
};

NYLON_NAMESPACE_END

#include "nylon_message_builder_inline.h"

#endif // MESSAGE_BUILDER_H
