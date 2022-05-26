#include <gtest/gtest.h>

#include "message_reader_test_fixture.h"

#define MESSAGE_TEST(name) TEST(NylonMessage, name)

MESSAGE_TEST(testHeartBeatEncoding)
{
    nylon::HeartBeat hb;

    char output[nylon::HeartBeat::size + 1]; // +1 for null terminator
    auto size = nylon::HeartBeat::size;

    hb.encode(output, size);

    EXPECT_EQ(output[0], static_cast<char>(nylon::HeartBeat::messageType));
    EXPECT_EQ(size, 0);
}

MESSAGE_TEST(testLogonEncoding)
{
    nylon::Logon l;

    char output[nylon::Logon::size + 1]; // +1 for null terminator
    auto size = nylon::Logon::size;

    l.encode(output, size);

    EXPECT_EQ(output[0], static_cast<char>(nylon::Logon::messageType));
    EXPECT_EQ(size, 0);
}

MESSAGE_TEST(testLogonAcceptedEncoding)
{
    nylon::LogonAccepted la;
    la.sessionId = 5;

    char output[nylon::LogonAccepted::size + 1]; // +1 for null terminator
    auto encodeSize = nylon::LogonAccepted::size;

    la.encode(output, encodeSize);

    EXPECT_EQ(output[0], static_cast<char>(nylon::LogonAccepted::messageType));
    EXPECT_EQ(output[1], la.sessionId);
    EXPECT_EQ(encodeSize, 0);

    auto decodeSize = nylon::LogonAccepted::size;
    auto const decodedMessage = nylon::LogonAccepted::decode(output, decodeSize);

    EXPECT_EQ(decodedMessage.sessionId, output[1]);
}

#undef MESSAGE_TEST

#define MESSAGE_READER_TEST(name) TEST_F(MessageReaderTestFixture, name)

MESSAGE_READER_TEST(testBasicHeartBeatMessage)
{
    // Send a heartbeat message to the tcp client
    pushInputEvent(HeartBeatInput());

    // Read that message from the tcp client in
    // the message reader
    pushInputEvent(ReadInput());

    // verify the message was decoded correctly
    pushOutputValidator(HeartBeatOutput());
}

MESSAGE_READER_TEST(testBasicLogonMessage)
{
    // Send a Logon message to the tcp client
    pushInputEvent(LogonInput());

    // Read that message from the tcp client in
    // the message reader
    pushInputEvent(ReadInput());

    // verify the message was decoded correctly
    pushOutputValidator(LogonOutput());
}

MESSAGE_READER_TEST(testBasicLogonAcceptedMessage)
{
    // Send a Logon message to the tcp client
    pushInputEvent(LogonAcceptedInput(67));

    // Read that message from the tcp client in
    // the message reader
    pushInputEvent(ReadInput());

    // verify the message was decoded correctly
    pushOutputValidator(LogonAcceptedOutput(67));
}

// Send in enough messages to rollover the MessageReader
// ring-buffer. The test fixutre's ring buffer has a size
// of 3 * maxMessageSize. This test uses 4 max-sized messages
// to test the scenario where there is no remainder in the
// ring buffer when it rolls over
MESSAGE_READER_TEST(testRollover_NoRemainder)
{
    EXPECT_EQ(nylon::maxMessageSize, nylon::LogonAccepted::size) << "A new, larger message has been added and this test needs updating";

    // Send first message, 1/3 of ring buffer
    pushInputEvent(LogonAcceptedInput(1));
    pushInputEvent(ReadInput());
    pushOutputValidator(LogonAcceptedOutput(1));

    // Send second message, 2/3 of ring buffer
    pushInputEvent(LogonAcceptedInput(2));
    pushInputEvent(ReadInput());
    pushOutputValidator(LogonAcceptedOutput(2));

    // Send third message, 3/3 of ring buffer -> rollover
    pushInputEvent(LogonAcceptedInput(3));
    pushInputEvent(ReadInput());
    pushOutputValidator(LogonAcceptedOutput(3));

    // Send fourth message, validate we can still read messages
    // after rollover
    pushInputEvent(LogonAcceptedInput(4));
    pushInputEvent(ReadInput());
    pushOutputValidator(LogonAcceptedOutput(4));
}

MESSAGE_READER_TEST(testRollover_Remainder)
{
    EXPECT_EQ(nylon::maxMessageSize, nylon::LogonAccepted::size) << "A new, larger message has been added and this test needs updating";

    // Send first, byte sized, message, 1/6 of ring buffer
    pushInputEvent(LogonInput());
    pushInputEvent(ReadInput());
    pushOutputValidator(LogonOutput());

    // Send second message, 3/6 of ring buffer
    pushInputEvent(LogonAcceptedInput(1));
    pushInputEvent(ReadInput());
    pushOutputValidator(LogonAcceptedOutput(1));

    // Send third message, 5/6 of ring buffer
    pushInputEvent(LogonAcceptedInput(2));
    pushInputEvent(ReadInput());
    pushOutputValidator(LogonAcceptedOutput(2));

    // Send fourth message, 7/6 of ring buffer -> rollover, can't read this message
    pushInputEvent(LogonAcceptedInput(3));
    pushInputEvent(ReadInput());

    // Send fifth message, there should be two messages in the ringbuffer now and
    // read should return the first
    pushInputEvent(LogonAcceptedInput(4));
    pushInputEvent(ReadInput());
    pushOutputValidator(LogonAcceptedOutput(3)); //< previous message

    // There's still a message in the buffer, validate read returns it
    pushInputEvent(ReadInput());
    pushOutputValidator(LogonAcceptedOutput(4));
}

// Same as testRollover_NoRemainder but don't call
// read until all of the messages are in
MESSAGE_READER_TEST(testRollover_BuildUpNoRemainder)
{
    EXPECT_EQ(nylon::maxMessageSize, nylon::LogonAccepted::size) << "A new, larger message has been added and this test needs updating";

    pushInputEvent(LogonAcceptedInput(1));
    pushInputEvent(LogonAcceptedInput(2));
    pushInputEvent(LogonAcceptedInput(3));
    pushInputEvent(LogonAcceptedInput(4));

    // Read first message, 1/3 of ring buffer
    pushInputEvent(ReadInput());
    pushOutputValidator(LogonAcceptedOutput(1));

    // Read second message, 2/3 of ring buffer
    pushInputEvent(ReadInput());
    pushOutputValidator(LogonAcceptedOutput(2));

    // Read third message, 3/3 of ring buffer -> rollover
    pushInputEvent(ReadInput());
    pushOutputValidator(LogonAcceptedOutput(3));

    // Read fourth message, validate we can still read messages
    // after rollover
    pushInputEvent(ReadInput());
    pushOutputValidator(LogonAcceptedOutput(4));
}

MESSAGE_READER_TEST(testRollover_BuildUpRemainder)
{
    EXPECT_EQ(nylon::maxMessageSize, nylon::LogonAccepted::size) << "A new, larger message has been added and this test needs updating";

    pushInputEvent(LogonInput());
    pushInputEvent(LogonAcceptedInput(1));
    pushInputEvent(LogonAcceptedInput(2));
    pushInputEvent(LogonAcceptedInput(3));
    pushInputEvent(LogonAcceptedInput(4));

    // Read first, byte sized, message, 1/6 of ring buffer
    pushInputEvent(ReadInput());
    pushOutputValidator(LogonOutput());

    // Read second message, 3/6 of ring buffer
    pushInputEvent(ReadInput());
    pushOutputValidator(LogonAcceptedOutput(1));

    // Read third message, 5/6 of ring buffer
    pushInputEvent(ReadInput());
    pushOutputValidator(LogonAcceptedOutput(2));

    // Read fourth message, 7/6 of ring buffer -> rollover, can't read this message
    pushInputEvent(ReadInput());

    // Read fifth message, there should be two messages in the ringbuffer now and
    // read should return the first
    pushInputEvent(ReadInput());
    pushOutputValidator(LogonAcceptedOutput(3)); //< previous message

    // There's still a message in the buffer, validate read returns it
    pushInputEvent(ReadInput());
    pushOutputValidator(LogonAcceptedOutput(4));
}

#undef MESSAGE_READER_TEST
