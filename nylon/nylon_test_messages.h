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
struct HeartBeat : MessageBase<0> {};
struct Logon : MessageBase<1> {};
struct LogonAccepted : MessageBase<2, sessionIdField> {};
struct Text : MessageBase<3, textField> {};

using TestMessageDefiner = MessageDefiner<HeartBeat, Logon, LogonAccepted, Text>;
using Message = typename TestMessageDefiner::MessageType;
//using TestMessageBuilder = typename TestMessageDefiner::MessageBuilder;
using TestMessageBuilder = MessageBuilder<TestMessageDefiner>;

NYLON_NAMESPACE_END

#endif // NYLON_TEST_MESSAGES_H
