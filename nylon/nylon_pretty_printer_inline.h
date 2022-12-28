NYLON_NAMESPACE_BEGIN

template <typename MessageDefiner>
std::string prettyPrint(typename MessageDefiner::MessageType const & msg)
{
    std::string result;

    std::visit([&result](auto const & m) {
        result = prettyPrintMessage(m);
    }, msg);

    return result;
}

template <typename MessageType>
std::string prettyPrintMessage(MessageType const & msg)
{
    std::string result = ConsoleColor::boldBlue + std::string(msg.name()) + "(" + std::to_string(msg.messageType) + ")" + ConsoleColor::reset;

    result += '\n';

    msg.forEachField([&result]<typename Field>(Field const & field) {
        result += std::string("\t") + PrettyPrinter<typename Field::Archetype>()(field.asArchetype());
    });

    if constexpr (MessageType::fieldCount > 0) {
        // don't add two tabs for messages without fields
        result += '\n';
    }

    return result;
}

inline std::string prettyField(std::string const & text)
{
    return ConsoleColor::cyan + text + ConsoleColor::reset;
}

inline std::string prettyField(const char * text)
{
    return ConsoleColor::cyan + std::string(text) + ConsoleColor::reset;
}

inline std::string prettyString(std::string const & text)
{
    return ConsoleColor::green + std::string("\"") + std::string(text) + std::string("\"") + ConsoleColor::reset;
}

inline std::string prettyChar(char c)
{
    return ConsoleColor::green + std::string("'") + std::to_string(c) + std::string("'") + ConsoleColor::reset;
}

inline std::string prettyNumber(auto number)
{
    return ConsoleColor::magenta + std::to_string(number) + ConsoleColor::reset;
}

inline std::string prettyEqualsSign()
{
    return ConsoleColor::yellow + std::string("=") + ConsoleColor::reset;
}

inline std::string prettyError(std::string const & text)
{
    return ConsoleColor::boldRed + text + ConsoleColor::reset;
}

template <typename Field>
std::string PrettyPrinter<Field>::operator()(Field const & /*field*/)
{
    return prettyError("Unknown=???");
}

template <typename Derived>
std::string PrettyPrinter<CharField<Derived>>::operator()(CharField<Derived> const & field)
{
    return prettyField(field.name()) + prettyEqualsSign() + prettyChar(field.value());
}

template <typename Derived>
std::string PrettyPrinter<StringField<Derived>>::operator()(StringField<Derived> const & field)
{
    return prettyField(field.name()) + prettyEqualsSign() + prettyString(field.value());
}

template <typename Derived>
std::string PrettyPrinter<UInt8Field<Derived>>::operator()(UInt8Field<Derived> const & field)
{
    return prettyField(field.name()) + prettyEqualsSign() + prettyNumber(field.value());
}

NYLON_NAMESPACE_END
