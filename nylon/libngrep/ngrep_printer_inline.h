#include "nylon/nylon_pretty_printer.h"

NYLON_NAMESPACE_BEGIN

template <typename MessageDefiner>
void Printer<MessageDefiner>::print(std::span<const char> buffer)
{
    auto bytesSurveyed = 0lu;
    auto const bufferSize = buffer.size(); // cache, otherwise the size will change with each subspan

    while (bytesSurveyed < bufferSize) {
        auto const & [maybeMessage, bytesDecoded] = messageBuilder_.build(buffer);

        bytesSurveyed += bytesDecoded;
        buffer = buffer.subspan(bytesDecoded);

        if (maybeMessage) {
            onMessage(prettyPrint<MessageDefiner>(maybeMessage.value()));
        }
    }
}

NYLON_NAMESPACE_END
