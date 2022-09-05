#include <gtest/gtest.h>

#include "message_reader_test_fixture.h"
#include "message_writer_test_fixture.h"

#include "../nylon/nylon_message_encoder.h"

#define MESSAGE_BUILDER_TEST(name) TEST(NylonMessageBuilder, name)

MESSAGE_BUILDER_TEST(testHeartBeatBuilding)
{
    nylon::TestMessageBuilder builder;
    const char * buffer = "\0";
    size_t bufferPos = 0;

    builder.build<nylon::HeartBeat>(buffer, bufferPos, sizeof(buffer));

    EXPECT_EQ(builder.state(), nylon::TestMessageBuilder::State::Finished);
    EXPECT_EQ(bufferPos, 1);
}

MESSAGE_BUILDER_TEST(testLogonBuilding)
{
    nylon::TestMessageBuilder builder;
    char buffer[] = "\0";
    buffer[0] = static_cast<char>(nylon::Logon::messageType);
    size_t bufferPos = 0;

    builder.build<nylon::Logon>(buffer, bufferPos, sizeof(buffer));
    EXPECT_EQ(builder.state(), nylon::TestMessageBuilder::State::Finished);
    EXPECT_EQ(bufferPos, 1);
}

MESSAGE_BUILDER_TEST(testLogonAcceptedBuilding)
{
    nylon::TestMessageBuilder builder;
    char buffer[] = "\0C"; // C is ascii 67
    buffer[0] = static_cast<char>(nylon::LogonAccepted::messageType);
    size_t bufferPos = 0;

    builder.build<nylon::LogonAccepted>(buffer, bufferPos, sizeof(buffer));
    EXPECT_EQ(builder.state(), nylon::TestMessageBuilder::State::Finished);
    EXPECT_EQ(bufferPos, 2);

    auto message = builder.finalizeMessage();

    EXPECT_EQ(message.index(), static_cast<size_t>(nylon::LogonAccepted::messageType));

    auto la = std::get<nylon::LogonAccepted>(message);

    EXPECT_EQ(la.sessionId, 67);
}

MESSAGE_BUILDER_TEST(testTextBuilding)
{
    nylon::TestMessageBuilder builder;
    char buffer[] = "\0\0hello world";
    buffer[0] = static_cast<char>(nylon::Text::messageType);
    buffer[1] = static_cast<char>(sizeof(buffer) - 3); // subtract out message type, length and null terminator
    size_t bufferPos = 0;

    builder.build<nylon::Text>(buffer, bufferPos, sizeof(buffer));

    EXPECT_EQ(builder.state(), nylon::TestMessageBuilder::State::Finished);
    EXPECT_EQ(bufferPos, sizeof(buffer) - 1); // subtract out null terminator

    auto const message = builder.finalizeMessage();

    EXPECT_EQ(message.index(), static_cast<size_t>(nylon::Text::messageType));

    auto const tm = std::get<nylon::Text>(message);

    EXPECT_EQ(tm.text, "hello world");
}

#undef MESSAGE_BUILDER_TEST

#define MESSAGE_TEST(name) TEST(NylonMessage, name)

MESSAGE_TEST(testHeartBeatEncoding)
{
    nylon::HeartBeat hb;

    auto constexpr heartBeatSize = 1;

    char output[heartBeatSize + 1]; // + 1 for null terminator
    char * outPtrBefore = output;
    char * outPtrAfter  = outPtrBefore;

    nylon::MessageEncoder::encode(&outPtrAfter, hb);

    EXPECT_EQ(output[0], static_cast<char>(nylon::HeartBeat::messageType));
    EXPECT_EQ(outPtrAfter, outPtrBefore + heartBeatSize);
}

MESSAGE_TEST(testLogonEncoding)
{
    nylon::Logon l;

    auto constexpr logonSize = 1;

    char output[logonSize + 1]; // + 1 for null terminator
    char * outPtrBefore = output;
    char * outPtrAfter  = outPtrBefore;

    nylon::MessageEncoder::encode(&outPtrAfter, l);

    EXPECT_EQ(output[0], static_cast<char>(nylon::Logon::messageType));
    EXPECT_EQ(outPtrAfter, outPtrBefore + logonSize);
}

MESSAGE_TEST(testLogonAcceptedEncoding)
{
    nylon::LogonAccepted la;
    la.sessionId = 5;

    auto constexpr logonAcceptedSize = 2;

    char output[logonAcceptedSize + 1]; // + 1 for null terminator
    char * outPtrBefore = output;
    char * outPtrAfter  = outPtrBefore;

    nylon::MessageEncoder::encode(&outPtrAfter, la);

    EXPECT_EQ(output[0], static_cast<char>(nylon::LogonAccepted::messageType));
    EXPECT_EQ(outPtrAfter, outPtrBefore + logonAcceptedSize);
    EXPECT_EQ(output[1], la.sessionId);

#if 0 // fix this once decoding is handled again
    auto decodeSize = logonAcceptedSize;
    auto const decodedMessage = nylon::LogonAccepted::decode(output, decodeSize);

    EXPECT_EQ(decodedMessage.sessionId, output[1]);
#endif
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

MESSAGE_READER_TEST(testBasicTextMessage)
{
    pushInputEvent(TextInput("tst")); // < this is small enough a single read will be able to read it in
    pushInputEvent(ReadInput());
    pushOutputValidator(TextOutput("tst"));
}

MESSAGE_READER_TEST(testTextMessage_TextTooLargeForBuffer)
{
    // The payload of the first message is "XXhello world", 13 bytes
    pushInputEvent(TextInput("hello world"));

    // The read buffer is 6 bytes, so we'll read the first 6 with this call
    pushInputEvent(ReadInput());

    // Now we'll read another size, for a total of 12, with one more required
    pushInputEvent(ReadInput());

    // Now we should read in the last byte
    pushInputEvent(ReadInput());

    pushOutputValidator(TextOutput("hello world"));
}

// Send in enough messages to rollover the MessageReader
// ring-buffer. The test fixutre's ring buffer has a size
// of 3 * maxMessageSize. This test uses 4 max-sized messages
// to test the scenario where there is no remainder in the
// ring buffer when it rolls over
MESSAGE_READER_TEST(testRollover_NoRemainder)
{
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

// Here, we're testing that part of a message can
// be read into the end of the ring buffer in one
// read call to partially construct the LA, and
// that the rest of the message can be read in in
// the next read call
MESSAGE_READER_TEST(testPartialReads_LogonAccepted)
{
    pushInputEvent(LogonInput());           // 1/6 of buffer
    pushInputEvent(LogonAcceptedInput(1));  // 3/6 of buffer
    pushInputEvent(LogonAcceptedInput(2));  // 5/6 of buffer
    pushInputEvent(LogonAcceptedInput(3));  // fill last byte with part of message, next read should get the other part

    pushInputEvent(ReadInput());
    pushOutputValidator(LogonOutput());

    pushInputEvent(ReadInput());
    pushOutputValidator(LogonAcceptedOutput(1));

    pushInputEvent(ReadInput());
    pushOutputValidator(LogonAcceptedOutput(2));

    pushInputEvent(ReadInput());
    // no output, only partially constructed last LA

    pushInputEvent(ReadInput());
    pushOutputValidator(LogonAcceptedOutput(3));
}

#undef MESSAGE_READER_TEST

#define MESSAGE_WRITER_TEST(name) TEST_F(MessageWriterTestFixture, name)

MESSAGE_WRITER_TEST(testBasicHeartBeatMessage)
{
    pushInputEvent(HeartBeatInput());
    pushOutputValidator(HeartBeatOutput());
}

MESSAGE_WRITER_TEST(testBasicLogonMessage)
{
    pushInputEvent(LogonInput());
    pushOutputValidator(LogonOutput());
}

MESSAGE_WRITER_TEST(testBasicLogonAcceptedMessage)
{
    pushInputEvent(LogonAcceptedInput(67));
    pushOutputValidator(LogonAcceptedOutput(67));
}

MESSAGE_WRITER_TEST(testBasicTextMessage)
{
    pushInputEvent(TextInput("hello world"));
    pushOutputValidator(TextOutput("hello world"));
}

MESSAGE_WRITER_TEST(testMultipleMessages)
{
    // grows internal buffer to 1 byte
    pushInputEvent(HeartBeatInput());
    pushOutputValidator(HeartBeatOutput());

    // grows internal buffer to 2 bytes
    pushInputEvent(LogonAcceptedInput(67));
    pushOutputValidator(LogonAcceptedOutput(67));

    // send another 1 byte message
    pushInputEvent(LogonInput());
    pushOutputValidator(LogonOutput());

    pushInputEvent(TextInput("hello world"));
    pushOutputValidator(TextOutput("hello world"));
}

#undef MESSAGE_WRITER_TEST
