#include "nylon_pretty_printer.h"

NYLON_NAMESPACE_BEGIN

template <typename MessageDefiner>
MessageReaderLog<MessageDefiner>::MessageReaderLog(unsigned maxEntriesToTrack)
    : maxEntriesToTrack_(maxEntriesToTrack)
{

}

template <typename MessageDefiner>
void MessageReaderLog<MessageDefiner>::beforeProcessing(std::span<const char> bytesBeingProcessed, DecoderState state)
{
    assert(!currentEntry_);

    currentEntry_ = MessageReaderLogEntry();
    currentEntry_->bytesBeingProcessed.assign(bytesBeingProcessed.begin(), bytesBeingProcessed.end());
    currentEntry_->decoderStateBeforeProcessing = state;
}

template <typename MessageDefiner>
void MessageReaderLog<MessageDefiner>::afterProcessing(DecoderState state, MaybeMessage const & maybeMessage)
{
    assert(currentEntry_);

    if (log_.size() > maxEntriesToTrack_) {
        log_.pop_front();
    }

    currentEntry_->decoderStateAfterProcessing = state;
    currentEntry_->messageBuiltDuringProcessing = maybeMessage;

    LogEntry* lastEntry = nullptr;

    if (!log_.empty()) {
        lastEntry = log_.back().get();
    }

    if (lastEntry && *lastEntry == currentEntry_.value()) {
        // skip this entry 
        if (!currentSkipEntry_) {
            currentSkipEntry_ = SkippedMessagesEntry();
        }

        ++currentSkipEntry_->numSkippedMessages;
    }
    else {
        if (currentSkipEntry_) {
            log_.push_back(std::make_unique<SkippedMessagesEntry>(currentSkipEntry_.value()));
        }

        log_.push_back(std::make_unique<MessageReaderLogEntry>(currentEntry_.value()));
    }

    currentEntry_.reset();
}

template <typename MessageDefiner>
std::string MessageReaderLog<MessageDefiner>::dump()
{
    using namespace std::string_literals;

    auto result = "Message Reader Log:\n"s;

    for (auto const & entry : log_) {
        result += "--- Begin Entry ---\n";
        result += entry->dump();
        result += "--- End Entry ---\n\n";
    }

    return result;
}

template <typename MessageDefiner>
bool MessageReaderLog<MessageDefiner>::MessageReaderLogEntry::operator==(LogEntry const & rhs) const
{
    if (rhs.entryType() != entryType()) {
        return false;
    }

    auto const & r = static_cast<MessageReaderLogEntry const &>(rhs);

    return decoderStateBeforeProcessing == r.decoderStateBeforeProcessing
        && decoderStateAfterProcessing  == r.decoderStateAfterProcessing
        && bytesBeingProcessed          == r.bytesBeingProcessed
        /*&& messageBuiltDuringProcessing == r.messageBuiltDuringProcessing*/; // TODO: message equality operator
}

template <typename MessageDefiner>
std::string MessageReaderLog<MessageDefiner>::MessageReaderLogEntry::dump() const
{
    auto const toString = [](typename MessageReaderLogEntry::MaybeDecoderState state) {
        if (!state) {
            return "None";
        }

        switch(state.value()) {
            case DecoderState::NotStarted: return "NotStarted";
            case DecoderState::Building:   return "Building";
            case DecoderState::Finished:   return "Finished";
        }

        throw std::runtime_error("new, unhandled enum was added");
    };

    auto result = std::string();

    result += "Bytes being processed: ";
    result += std::string(bytesBeingProcessed.begin(), bytesBeingProcessed.end());
    result += '\n';

    result += "State before processing: ";
    result += toString(decoderStateBeforeProcessing);
    result += '\n';

    result += "State after processing: ";
    result += toString(decoderStateAfterProcessing);
    result += '\n';

    if (messageBuiltDuringProcessing) {
        result += "Message:";
        result += prettyPrint<MessageDefiner>(messageBuiltDuringProcessing.value());
        result += "\n";
    }

    return result;
}

template <typename MessageDefiner>
typename MessageReaderLog<MessageDefiner>::EntryType
MessageReaderLog<MessageDefiner>::MessageReaderLogEntry::entryType() const
{
    return EntryType::MessageReaderEntry;
}

template <typename MessageDefiner>
bool MessageReaderLog<MessageDefiner>::SkippedMessagesEntry::operator==(LogEntry const & rhs) const
{
    if (rhs.entryType() != entryType()) {
        return false;
    }

    auto const & r = static_cast<SkippedMessagesEntry const &>(rhs);

    return numSkippedMessages == r.numSkippedMessages;
}

template <typename MessageDefiner>
std::string MessageReaderLog<MessageDefiner>::SkippedMessagesEntry::dump() const
{
    using namespace std::string_literals;

    return "... Skipped "s + std::to_string(numSkippedMessages) + " Messages (No Change) ...";
}

template <typename MessageDefiner>
typename MessageReaderLog<MessageDefiner>::EntryType
MessageReaderLog<MessageDefiner>::SkippedMessagesEntry::entryType() const
{
    return EntryType::SkippedMessagesEntry;
}

NYLON_NAMESPACE_END
