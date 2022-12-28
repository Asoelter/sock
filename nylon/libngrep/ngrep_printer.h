#ifndef NYLON_NGREP_PRINTER_H
#define NYLON_NGREP_PRINTER_H

#include "namespace.h"

#include "nylon/nylon_message_builder.h"

#include <functional>
#include <span>
#include <string>

NYLON_NAMESPACE_BEGIN

template <typename MessageDefiner>
class Printer
{
public:
    using OnMessageEvent = std::function<void(std::string&&)>;

    void print(std::span<const char> buffer);

    OnMessageEvent onMessage;

private:
    using MessageBuilder = MessageBuilder<MessageDefiner>;

    MessageBuilder messageBuilder_;
};

NYLON_NAMESPACE_END

#include "ngrep_printer_inline.h"

#endif // NYLON_NGREP_PRINTER_H
