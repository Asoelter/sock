#ifndef MESSAGE_READER_TEST_FIXTURE_H
#define MESSAGE_READER_TEST_FIXTURE_H

#include "../nylon/nylon_message_reader.h"

#include <gtest/gtest.h>

#include <queue>

class MessageReaderTestFixture : public ::testing::Test
{
public:
    MessageReaderTestFixture()
        : messageReader_(&messageSender_, 3 * nylon::maxMessageSize)
    {

    }

    struct InputEvent;
    struct OutputValidator;

    void pushInputEvent_(InputEvent const & input)
    {
        input.fire(*this);
    }

    void pushOutputValidator_(const char * file, unsigned line, OutputValidator const & output)
    {
        if (outputMessages_.empty()) {
            ADD_FAILURE_AT(file, line) << "Unexpected output event";
            return;
        }

        auto const & message = outputMessages_.front();
        outputMessages_.pop();

        output.validate(file, line, message);
    }


private:
    class FakeTcpSocket
    {
    public:
        bool connected() { return true; }

        template <typename InputIt>
        void pushInput(InputIt begin, InputIt end)
        {
            assert(writeOffset_ <= input_.size());

            size_t const bytesNeeded = std::distance(begin, end);

            if (input_.size() - writeOffset_ < bytesNeeded) {
                auto const bytesAvailable = input_.size() - writeOffset_;
                auto const growthNeeded = bytesNeeded - bytesAvailable;
                input_.resize(growthNeeded);
            }

            for(; begin < end; ++begin) {
                input_[writeOffset_++] = *begin;
            }
        }

        long read(char * const buffer, size_t size)
        {
            size_t index = 0;

            // writeOffset_ is the end of readable data
            while (readOffset_ < writeOffset_ && index < size) {
                buffer[index++] = input_[readOffset_++];
            }

            assert(readOffset_ <= input_.size());
            if (readOffset_ == input_.size()) {
                // we've sent all data we have, can
                // start reading/writing from the
                // start of the buffer
                readOffset_  = 0;
                writeOffset_ = 0;
            }

            return index;
        }

    private:
        std::vector<char> input_;
        size_t readOffset_  = 0;
        size_t writeOffset_ = 0;
    };

public:
    using TestMessageReader = nylon::MessageReader<FakeTcpSocket>;

    struct InputEvent
    {
        InputEvent() = default;
        virtual ~InputEvent() = default;

        virtual void fire(MessageReaderTestFixture& fixture) const = 0;
    };

    struct HeartBeatInput : public InputEvent
    {
        HeartBeatInput() = default;

        void fire(MessageReaderTestFixture& fixture) const override
        {
            // TODO(asoelter): just call the message type's encode
            // on the payload
            char payload[nylon::HeartBeat::size + 1]; // messageType
            payload[0] = 0;
            fixture.messageSender_.pushInput(payload, payload + nylon::HeartBeat::size);
        }
    };

    struct LogonInput : public InputEvent
    {
        LogonInput() = default;

        void fire(MessageReaderTestFixture& fixture) const override
        {
            // TODO(asoelter): just call the message type's encode
            // on the payload
            char payload[nylon::Logon::size + 1]; // messageType
            payload[0] = 1;
            fixture.messageSender_.pushInput(payload, payload + nylon::Logon::size);
        }
    };

    struct LogonAcceptedInput : public InputEvent
    {
        LogonAcceptedInput(uint8_t sessionId)
            : sessionId(sessionId)
        {

        }

        void fire(MessageReaderTestFixture& fixture) const override
        {
            // TODO(asoelter): just call the message type's encode
            // on the payload
            char payload[nylon::LogonAccepted::size + 1];
            payload[0] = 2; // messageType
            payload[1] = sessionId; 
            fixture.messageSender_.pushInput(payload, payload + nylon::LogonAccepted::size);
        }

        uint8_t sessionId;
    };

    struct ReadInput : public InputEvent
    {
        void fire(MessageReaderTestFixture& fixture) const override
        {
            auto const maybeMessage = fixture.messageReader_.read();

            if (maybeMessage) {
                fixture.outputMessages_.push(maybeMessage.value());
            }
        }
    };

    struct OutputValidator
    {
        OutputValidator() = default;
        virtual ~OutputValidator() = default;

        virtual void validate(const char * file, unsigned line, nylon::Message const & message) const = 0;
    };

    struct HeartBeatOutput : public OutputValidator
    {
        void validate(const char * file, unsigned line, nylon::Message const & message) const override
        {
            if (!std::holds_alternative<nylon::HeartBeat>(message)) {
                // probably should be able to switch between FAIL and ADD_FAILURE
                // via program arg
                ADD_FAILURE_AT(file, line) << "Expected HeartBeat message but received other type of message";
            }
        }
    };

    struct LogonOutput : public OutputValidator
    {
        void validate(const char * file, unsigned line, nylon::Message const & message) const override
        {
            if (!std::holds_alternative<nylon::Logon>(message)) {
                // probably should be able to switch between FAIL and ADD_FAILURE
                // via program arg
                ADD_FAILURE_AT(file, line) << "Expected Logon message but received other type of message";
            }
        }
    };

    struct LogonAcceptedOutput : public OutputValidator
    {
        LogonAcceptedOutput(uint8_t sessionId)
            : sessionId(sessionId)
        {

        }

        void validate(const char * file, unsigned line, nylon::Message const & message) const override
        {
            if (!std::holds_alternative<nylon::LogonAccepted>(message)) {
                // probably should be able to switch between FAIL and ADD_FAILURE
                // via program arg
                ADD_FAILURE_AT(file, line) << "Expected LogonAccepted message but received other type of message: " << static_cast<char>(nylon::typeOf(message));
            }

            auto & la = std::get<nylon::LogonAccepted>(message);

            if (la.sessionId != sessionId) {
                ADD_FAILURE_AT(file, line)
                    << "Mismatched sessionsIds. Received: "
                    << static_cast<int>(la.sessionId)
                    << " expected "
                    << static_cast<int>(sessionId);
            }
        }

        uint8_t sessionId;
    };

private:

    FakeTcpSocket               messageSender_;
    TestMessageReader           messageReader_;
    std::queue<nylon::Message>  outputMessages_;
};

// macro just in case we want to pass __LINE__ and __FILE__ in eventually
#define pushInputEvent(input)       pushInputEvent_(input)
#define pushOutputValidator(output) pushOutputValidator_(__FILE__, __LINE__, output)

#endif // MESSAGE_READER_TEST_FIXTURE_H
