#ifndef NYLON_MESSAGE_READER_LOG_H
#define NYLON_MESSAGE_READER_LOG_H

#include "namespace.h"
#include "nylon_message_builder.h"

#include <deque>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <vector>

NYLON_NAMESPACE_BEGIN

template <typename MessageDefiner>
class MessageReaderLog
{
public:
    using DecoderState = typename MessageBuilder<MessageDefiner>::State;
    using MaybeMessage = std::optional<typename MessageDefiner::MessageType>;

    MessageReaderLog(unsigned maxEntriesToTrack);

    void beforeProcessing(std::span<const char> bytesBeingProcessed, DecoderState state);
    void afterProcessing(DecoderState state, MaybeMessage const & maybeMessage);
    std::string dump();

private:
    enum class EntryType
    {
        MessageReaderEntry,
        SkippedMessagesEntry
    };

    struct LogEntry
    {
        virtual ~LogEntry() = default;
        virtual bool operator==(LogEntry const & rhs) const = 0;

        virtual std::string dump() const = 0;
        virtual EntryType entryType() const = 0;
    };

    struct MessageReaderLogEntry : public LogEntry
    {
        using MaybeDecoderState = std::optional<DecoderState>;

        virtual ~MessageReaderLogEntry() = default;

        bool operator==(LogEntry const & rhs) const override;

        std::string dump() const override;
        EntryType entryType() const override;

        std::vector<unsigned char>  bytesBeingProcessed;
        DecoderState                decoderStateBeforeProcessing;
        MaybeDecoderState           decoderStateAfterProcessing;
        MaybeMessage                messageBuiltDuringProcessing;
    };

    struct SkippedMessagesEntry : public LogEntry
    {
        bool operator==(LogEntry const & rhs) const override;

        std::string dump() const override;
        EntryType entryType() const override;

        unsigned numSkippedMessages = 0;
    };

    using LogEntryPtr = std::unique_ptr<LogEntry>;

    std::optional<MessageReaderLogEntry> currentEntry_;
    std::optional<SkippedMessagesEntry> currentSkipEntry_;
    std::deque<LogEntryPtr> log_;
    unsigned maxEntriesToTrack_;
};

NYLON_NAMESPACE_END

#include "nylon_message_reader_log_inline.h"

#endif // NYLON_MESSAGE_READER_LOG_H
