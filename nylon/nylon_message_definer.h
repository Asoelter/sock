#ifndef NYLON_MESSAGE_DEFINER
#define NYLON_MESSAGE_DEFINER

#include "namespace.h"
#include "nylon_char_field.h"
#include "nylon_decoder_context.h"
#include "nylon_field.h"
#include "nylon_field_macros.h"
#include "nylon_message_base.h"
#include "nylon_message_macros.h"
#include "nylon_string_field.h"
#include "nylon_uint8_field.h"

#include "../util/type_traits.h"
#include "../util/typelist.h"

#include <concepts>
#include <cstdint>
#include <span>
#include <string>
#include <type_traits>
#include <variant>

NYLON_NAMESPACE_BEGIN

template <typename ... MessageTs>
struct MessageDefiner
{
    // TODO(asoelter): Probably worth coming up with
    // a better naming scheme for these
    using MessageType  = std::variant<MessageTs...>;
    using MessageTypes = TypeList<MessageTs...>;
    using DecoderContext = std::variant<MessageDecoderContext<MessageTs>...>;
};

NYLON_NAMESPACE_END

#endif // NYLON_MESSAGE_DEFINER
