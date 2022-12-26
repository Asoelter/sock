#ifndef MESSAGE_WRITER_TEST_FIXTURE_H
#define MESSAGE_WRITER_TEST_FIXTURE_H

#include "../nylon/nylon_message_writer.h"
#include "../nylon/nylon_test_messages.h"

#include <gtest/gtest.h>

#include <vector>
#include <queue>

// Leverage Message reader
class MessageWriterTestFixture : public ::testing::Test
{
public: 
    MessageWriterTestFixture()
        : socket_(*this)
        , messageWriter_({ .socket = &socket_, .logFileName = std::nullopt})
    {

    }

    struct InputEvent;
    struct OutputValidator;
    using Packet = std::vector<char>;
    using Packets = std::queue<Packet>;

    void pushInputEvent_(InputEvent const & input)
    {
        input.fire(*this);
    }

    void pushOutputValidator_(const char * file, unsigned line, OutputValidator const & output)
    {
        if (packets_.empty()) {
            ADD_FAILURE_AT(file, line) << "Unexecpted output event";
            return;
        }

        output.validate(file, line, packets_.front());
        packets_.pop();
    }

    struct InputEvent
    {
        InputEvent() = default;
        virtual ~InputEvent() = default;

        virtual void fire(MessageWriterTestFixture& fixture) const = 0;
    };

    struct HeartBeatInput : public InputEvent
    {
        HeartBeatInput() = default;

        void fire(MessageWriterTestFixture& fixture) const
        {
            fixture.messageWriter_.write(nylon::HeartBeat());
        }
    };

    struct LogonInput : public InputEvent
    {
        LogonInput() = default;

        void fire(MessageWriterTestFixture& fixture) const
        {
            fixture.messageWriter_.write(nylon::Logon());
        }
    };

    struct LogonAcceptedInput : public InputEvent
    {
        LogonAcceptedInput(uint8_t sessionId)
            : sessionId(sessionId)
        {

        }

        void fire(MessageWriterTestFixture& fixture) const
        {
            auto payload = nylon::LogonAccepted();
            payload.sessionId = sessionId;
            fixture.messageWriter_.write(payload);
        }

        uint8_t sessionId;
    };

    struct TextInput : public InputEvent
    {
        TextInput(std::string_view text)
            : text(text)
        {

        }

        void fire(MessageWriterTestFixture& fixture) const
        {
            auto payload = nylon::Text();
            payload.text = text;
            fixture.messageWriter_.write(payload);
        }

        std::string text;
    };

    struct OutputValidator
    {
        OutputValidator() = default;
        virtual ~OutputValidator() = default;

        virtual void validate(const char * file, unsigned line, Packet const & packet) const = 0;
    };

    struct HeartBeatOutput : public OutputValidator
    {
        HeartBeatOutput() = default;

        void validate(const char * file, unsigned line, Packet const & packet) const override
        {
            if (packet.size() != 1) {
            ADD_FAILURE_AT(file, line)
                << "expected packet of size 1, received packet of size " << packet.size();
            }
        }
    };

    struct LogonOutput : public OutputValidator
    {
        LogonOutput() = default;

        void validate(const char * file, unsigned line, Packet const & packet) const override
        {
            if (packet.size() != 1) {
            ADD_FAILURE_AT(file, line)
                << "expected packet of size 1, received packet of size " << packet.size();
            }
        }
    };

    struct LogonAcceptedOutput : public OutputValidator
    {
        LogonAcceptedOutput(uint8_t sessionId)
            : sessionId(sessionId)
        {

        }

        void validate(const char * file, unsigned line, Packet const & packet) const override
        {
            if (packet.size() != 2) {
                ADD_FAILURE_AT(file, line)
                    << "expected packet of size 2, received packet of size " << packet.size();
            }

            if (packet[1] != sessionId) {
                ADD_FAILURE_AT(file, line)
                    << "expected sessionId = " << sessionId << ", received sessionId = " << packet[1];
            }
        }

        uint8_t sessionId;
    };

    struct TextOutput : public OutputValidator
    {
        TextOutput(std::string_view text)
            : text(text)
        {

        }

        void validate(const char * file, unsigned line, Packet const & packet) const override
        {
            auto const expectedPacketSize = 1 + 1 + text.size(); // +1 for msgType, +1 for textSize

            if (packet.size() != expectedPacketSize) {
                ADD_FAILURE_AT(file, line)
                    << "expected packet of size " << expectedPacketSize << ", received packet of size " << packet.size();
            }

            if (static_cast<size_t>(packet[1]) != text.size()) { //-1 because the string size doesn't count itself
                ADD_FAILURE_AT(file, line)
                    << "expected textSize of size " << text.size() << ", received textSize of size " << static_cast<size_t>(packet[1]);
            }

            auto constexpr textOffset = 1;

            auto const textReceived = std::string_view(&packet[0] + 2, packet.size() - 2);

            if (text != textReceived) {
                ADD_FAILURE_AT(file, line)
                    << "expected text=" << text << ", received text=" << textReceived;
            }
        }

        std::string text;
    };

private:
    struct FakeTcpSocket
    {
        FakeTcpSocket(MessageWriterTestFixture& owner)
            : owner_(owner)
        {

        }

        bool connected() { return true; }

        long write(char const * buffer, size_t size)
        {
            auto packet = std::vector<char>();
            packet.insert(packet.end(), buffer, buffer + size);
            owner_.packets_.push(packet);

            return size;
        }

        MessageWriterTestFixture& owner_;
    };

    using TestMessageWriter = nylon::MessageWriter<nylon::TestMessageDefiner, FakeTcpSocket>;

    FakeTcpSocket     socket_;
    TestMessageWriter messageWriter_;
    Packets           packets_;
};

#endif // MESSAGE_WRITER_TEST_FIXTURE_H

