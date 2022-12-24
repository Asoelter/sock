#ifndef NYLON_TEST_MESSAGES_H
#define NYLON_TEST_MESSAGES_H

#include "nylon_message_builder.h"
#include "nylon_message_definer.h"
#include "namespace.h"

#include <variant>

NYLON_NAMESPACE_BEGIN

// Fields
UINT8_FIELD(sessionId);
UINT8_FIELD(textLength);
STRING_FIELD(text);

// Messages
NYLON_MESSAGE_BEGIN(HeartBeat, 0)
NYLON_MESSAGE_END

NYLON_MESSAGE_BEGIN(Logon, 1)
NYLON_MESSAGE_END

NYLON_MESSAGE_BEGIN(LogonAccepted, 2)
    NYLON_MESSAGE_FIELD(sessionId)
NYLON_MESSAGE_END

NYLON_MESSAGE_BEGIN(Text, 3)
    NYLON_MESSAGE_FIELD(text)
NYLON_MESSAGE_END

using TestMessageDefiner = MessageDefiner<HeartBeat, Logon, LogonAccepted, Text>;
using Message = typename TestMessageDefiner::MessageType;
//using TestMessageBuilder = typename TestMessageDefiner::MessageBuilder;
using TestMessageBuilder = MessageBuilder<TestMessageDefiner>;

NYLON_NAMESPACE_END

#endif // NYLON_TEST_MESSAGES_H
