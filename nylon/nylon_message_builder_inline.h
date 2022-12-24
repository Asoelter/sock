#include <variant>

NYLON_NAMESPACE_BEGIN

template <typename MessageDefiner>
template <typename MessageT>
typename MessageBuilder<MessageDefiner>::State
MessageBuilder<MessageDefiner>::build(char const * buffer, size_t& bufferPos, size_t bufferSize)
{
    assert(state_ != MessageBuilder::State::Finished && "there is still a message to extract");

    switch (state_) {
        case MessageBuilder::State::NotStarted:
        {
            decoderContext_ = MessageDecoderContext<MessageT>(std::span<const char>(buffer, bufferSize));
            auto const [decoderState, bytesEncoded] = MessageDecoder::decode<MessageT>(context<MessageT>());
            bufferPos += bytesEncoded;
            state_ = translateDecoderState(decoderState);
        } break;
        case MessageBuilder::State::Building:
        {
            assert(decoderContext_.has_value());

            std::visit([buffer, bufferSize](auto& context) { context.reset(std::span<const char>(buffer, bufferSize)); }, decoderContext_.value());

            auto const [decoderState, bytesEncoded] = MessageDecoder::decode<MessageT>(context<MessageT>());
            bufferPos += bytesEncoded;
            state_ = translateDecoderState(decoderState);
        } break;
        case MessageBuilder::State::Finished:
        {
            throw std::runtime_error("there is still a message to extract, we're losing messages!");
        } break;
    }

    return state_;
}

template <typename MessageDefiner>
template <typename MessageT>
bool MessageBuilder<MessageDefiner>::isBuilding()
{
    if (!decoderContext_) {
        return false;
    }

    return std::holds_alternative<MessageDecoderContext<MessageT>>(decoderContext_.value());
}

template <typename MessageDefiner>
typename MessageBuilder<MessageDefiner>::MessageType
MessageBuilder<MessageDefiner>::finalizeMessage()
{
    assert(decoderContext_.has_value());
    assert(state_ == State::Finished && "attempt to finalize a message that is still being built");

    if (!decoderContext_) {
        throw std::runtime_error("attempt to finalize a message when one hasn't been built yet");
    }

    if (state_ != State::Finished) {
        throw std::runtime_error("attempt to finalize a message that is still being built");
    }

    state_ = State::NotStarted; // prepare for next message

    auto const& contextVariant = decoderContext_.value();

    auto result = MessageType();

    std::visit([&result](auto const & context) { result = context.message(); }, contextVariant);

    decoderContext_ = std::nullopt;

    return result;
}

template <typename MessageDefiner>
typename MessageBuilder<MessageDefiner>::State
MessageBuilder<MessageDefiner>::state() const noexcept
{
    return state_;
}

template <typename MessageDefiner>
template <typename ConcreteMessageType>
MessageDecoderContext<ConcreteMessageType>& MessageBuilder<MessageDefiner>::context()
{
    assert(decoderContext_.has_value() && "attempt to extract a context that doesn't exist");

    if (!decoderContext_.has_value()) {
        throw std::runtime_error("attempt to extract a context that doesn't exist");
    }

    auto & contextVariant = decoderContext_.value();

    auto * contextPointer = std::get_if<MessageDecoderContext<ConcreteMessageType>>(&contextVariant);

    assert(contextPointer && "attempt to extract a message of the wrong type");

    if (!contextPointer) {
        throw std::runtime_error("attempt to extract a message of the wrong type");
    }

    return *contextPointer;
}

template <typename MessageDefiner>
typename MessageBuilder<MessageDefiner>::State
MessageBuilder<MessageDefiner>::translateDecoderState(DecoderState decoderState)
{
    switch (decoderState) {
        case DecoderState::NotStarted:  return State::NotStarted;
        case DecoderState::InProgress:  return State::Building;
        case DecoderState::Success:     return State::Finished;
    }

    assert(false && "hit unreachable code path");
    return State::NotStarted;
}

NYLON_NAMESPACE_END
