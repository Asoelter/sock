NYLON_NAMESPACE_BEGIN

template <typename MessageType>
DecodeResult MessageDecoder::decode(MessageDecoderContext<MessageType>& context)
{
    auto bytesEncoded = 0u;

    if (context.bytesAlreadyEncoded() == 0) {
        auto const messageType = context.nextByte();
        ++bytesEncoded;

        if (messageType != MessageType::messageType) {
            throw std::runtime_error("MessageBuilderImpl given a buffer that does not contain the requested message type");
        }
    }

    using Fields = typename MessageType::Fields;

    if constexpr (!IsEmptyList<Fields>) {
        auto const [state, restOfBytesEncoded] = decodeImpl<MessageType, Fields>(context, 0 /*fieldIndex*/);
        return { .state = state, bytesEncoded = bytesEncoded + restOfBytesEncoded };
    }

    // If we get here the message has no fields, just a header, which we've
    // taken care of above. So, we're done and can return finshed
    return { .state = DecoderState::Success, .bytesEncoded = 1 };
}

template <typename MessageType, typename Fields>
DecodeResult MessageDecoder::decodeImpl(MessageDecoderContext<MessageType>& context, size_t fieldIndex)
{
    using CurrentField = Head<Fields>;
    using NextFields = Tail<TypeList<Fields>>;

    if (context.currentFieldIndex() > fieldIndex) {
        // we've already decoded this field but didn't finish the entire message,
        // so decode got called again. Move on to the next field
        if constexpr (!IsEmptyList<NextFields>) {
            return decodeImpl<MessageType, NextFields>(context, fieldIndex + 1);
        }

        // This really shouldn't happen, decode got called again which implies
        // the message wasn't fully decoded, but the field index is greater than
        // the number of fields which implies the whole message was decoded. But,
        // we need the if constexpr to make the compiler happy and this is the
        // best thing to do in this case
        assert(!"we're in a funky state bois");
        return { .state = DecoderState::Success, .bytesEncoded = 0 };
    }

    auto& field = context.message().template field<CurrentField>();

    auto const [state, bytesEncoded] = FieldDecoder<CurrentField>::decode(field, context);

    if (state == DecoderState::NotStarted || state == DecoderState::InProgress) {
        // probably not enough space in the buffer to read everything. We'll
        // try again after the next read
        return { .state = state, .bytesEncoded = bytesEncoded };
    }

    assert(state == DecoderState::Success && "A new value was added that hasn't been handled");

    context.advanceCurrentFieldIndex();

    if constexpr (!IsEmptyList<NextFields>) {
        // there are still more fields to parse and we haven't run out of items in the buffer
        // to parse yet, continue on to the next fields
        auto const [newState, restOfBytesEncoded] = decodeImpl<MessageType, NextFields>(context, fieldIndex + 1) + bytesEncoded;
        return { .state = newState, .bytesEncoded = bytesEncoded + restOfBytesEncoded };
    }

    // we've succeeded at parsing every field so far, and there are no more fields to parse,
    // so just return state, which should be success
    return { .state = state, .bytesEncoded = bytesEncoded };
}

NYLON_NAMESPACE_END
