NYLON_NAMESPACE_BEGIN

template <typename Field>
template <typename MessageContext>
DecodeResult FieldDecoder<Field>::decode(Field& field, MessageContext & context)
{
    using Archetype = typename Field::Archetype;

    auto& archetype = field.asArchetype();

    return FieldDecoder<Archetype>::decode(archetype, context);
}

template <typename Derived>
template <typename MessageContext>
DecodeResult FieldDecoder<CharField<Derived>>::decode(CharField<Derived>& field, MessageContext & context)
{
    if (context.bufferEmpty()) {
        return {
            .state = DecoderState::InProgress,
            .bytesEncoded = 0
        };
    }

    field.value() = context.nextByte();

    return {
        .state = DecoderState::Success,
        .bytesEncoded = 1
    };
}

template <typename Derived>
template <typename MessageContext>
DecodeResult FieldDecoder<StringField<Derived>>::decode(StringField<Derived>& field, MessageContext & context)
{
    if (context.bufferEmpty()) {
        return {
            .state = DecoderState::InProgress,
            .bytesEncoded = 0
        };
    }

    auto & fieldContext = context.template asContextFor<StringField<Derived>>();
    auto bytesWritten = 0u;

    if (!fieldContext.sizeRemainingIsSet) {
        fieldContext.sizeRemaining = context.nextByte();
        fieldContext.sizeRemainingIsSet = true;
        ++bytesWritten;
        field.value().reserve(fieldContext.sizeRemaining);
    }

    while(fieldContext.sizeRemaining && context.hasBytesAvailable()) {
        field.value().push_back(context.nextByte());
        --fieldContext.sizeRemaining;
        ++bytesWritten;
    }

    auto const state = fieldContext.sizeRemaining == 0
                     ? DecoderState::Success
                     : DecoderState::InProgress;

    return { .state = state, .bytesEncoded = bytesWritten };
}

template <typename Derived>
template <typename MessageContext>
DecodeResult FieldDecoder<UInt8Field<Derived>>::decode(UInt8Field<Derived>& field, MessageContext & context)
{
    if (context.bufferEmpty()) {
        return {
            .state = DecoderState::InProgress,
            .bytesEncoded = 0
        };
    }

    field.value() = context.nextByte();

    return {
        .state = DecoderState::Success,
        .bytesEncoded = 1
    };
}

NYLON_NAMESPACE_END
