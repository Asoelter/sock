#include <variant>

NYLON_NAMESPACE_BEGIN

template <typename MessageDefiner>
typename MessageBuilder<MessageDefiner>::BuildResult
MessageBuilder<MessageDefiner>::build(std::span<const char> buffer)
{
    return buildRecursive<typename MessageDefiner::MessageTypes>(buffer);
}

template <typename MessageDefiner>
template <ListType MessageTypes>
typename MessageBuilder<MessageDefiner>::BuildResult
MessageBuilder<MessageDefiner>::buildRecursive(std::span<const char> buffer)
{
    using CurrentType = Head<MessageTypes>;

    auto const messageType = buffer[0];

    if (messageType == static_cast<char>(CurrentType::messageType) || isBuilding<CurrentType>()) {
        assert(state_ != MessageBuilder<MessageDefiner>::State::Finished);
        auto const [bytesDecoded, messageState] = buildImpl<CurrentType>(buffer);

        if (messageState != MessageBuilder<MessageDefiner>::State::Finished) {
            return { .maybeMessage = std::nullopt, .bytesDecoded = bytesDecoded};
        }

        return { .maybeMessage = finalizeMessage(), .bytesDecoded = bytesDecoded };
    }

    using NextTypes = Tail<MessageTypes>;

    if constexpr (IsEmptyList<NextTypes>) {
        throw std::runtime_error(std::string("Unknown message type: ") + messageType);
    }
    else {
        return buildRecursive<NextTypes>(buffer);
    }
}

template <typename MessageDefiner>
template <typename MessageT>
typename MessageBuilder<MessageDefiner>::BuildImplResult
MessageBuilder<MessageDefiner>::buildImpl(std::span<const char> buffer)
{
    assert(state_ != MessageBuilder::State::Finished && "there is still a message to extract");

    switch (state_) {
        case MessageBuilder::State::NotStarted:
        {
            decoderContext_ = MessageDecoderContext<MessageT>(buffer);
            auto const [decoderState, bytesDecoded] = MessageDecoder::decode<MessageT>(context<MessageT>());
            state_ = translateDecoderState(decoderState);
            return { .bytesDecoded = bytesDecoded, .state = state_ };
        } break;
        case MessageBuilder::State::Building:
        {
            assert(decoderContext_.has_value());

            std::visit([buffer](auto& context) { context.reset(buffer); }, decoderContext_.value());

            auto const [decoderState, bytesDecoded] = MessageDecoder::decode<MessageT>(context<MessageT>());
            state_ = translateDecoderState(decoderState);
            return { .bytesDecoded = bytesDecoded, .state = state_ };
        } break;
        case MessageBuilder::State::Finished:
        {
            throw std::runtime_error("there is still a message to extract, we're losing messages!");
        } break;
    }

    // shouldn't ever hit this
    assert(!"we shouldn't have hit this");
    return { .bytesDecoded = 0, .state = state_ };
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
