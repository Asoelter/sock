#ifndef NGREP_SURVEYOR_H
#define NGREP_SURVEYOR_H

#include "namespace.h"

#include "nylon/nylon_message_base.h"
#include "nylon/nylon_message_builder.h"

#include <span>
#include <string>
#include <unordered_map>

NYLON_NAMESPACE_BEGIN

// Defined out of class so it isn't templated and
// we can define a hash for it.
struct SurveyMessageData
{
    MessageTypeT messageType;
    std::string messageName;
};

bool operator==(SurveyMessageData const & lhs, SurveyMessageData const & rhs);

NYLON_NAMESPACE_END

namespace std
{
    template <>
    struct hash<NYLON_NAMESPACE::SurveyMessageData>
    {
        using ValueType = NYLON_NAMESPACE::SurveyMessageData;

        std::size_t operator()(ValueType const & v) const {
            return hash<nylon::MessageTypeT>()(v.messageType);
        }
    };
}; // namespace std

NYLON_NAMESPACE_BEGIN

template <typename MessageDefiner>
class Surveyor
{
public:
    using MessageCounts = std::unordered_map<SurveyMessageData, size_t>;

public:
    Surveyor() = default;

    void survey(std::span<const char> buffer);

    MessageCounts const & messageCounts() const noexcept;

    template <typename MsgType>
    size_t messageCount() const;

private:
    using MessageBuilder = MessageBuilder<MessageDefiner>;

    MessageBuilder messageBuilder_;
    MessageCounts messageCounts_;
};

NYLON_NAMESPACE_END

#include "ngrep_surveyor_inline.h"

#endif // NGREP_SURVEYOR_H
