#ifndef NYLON_MESSAGE_MACROS_H
#define NYLON_MESSAGE_MACROS_H

// Here's an example to hopefully make these macros make sense.
// If these macros were used to define a message like so:
//
// NYLON_MESSAGE_BEGIN(Text, 3)
//     NYLON_MESSAGE_FIELD(text)
// NYLON_MESSAGE_END
//
// The output would be:
// struct textCRTPImplementer
// {
//     static constexpr MessageTypeT messageType = 3;
//
//     static const char * name() noexcept
//     {
//         return "text";
//     }
// };
//
// struct Text : MessageBase<
//          textCRTPImplementer
//          , textField
//     > {};

#define NYLON_MESSAGE_BEGIN(nameV, messageT)                    \
    struct nameV##CRTPImplementer                               \
    {                                                           \
        static constexpr MessageTypeT messageType = messageT;   \
                                                                \
        static const char * name() noexcept                     \
        {                                                       \
            return #nameV;                                      \
        }                                                       \
    };                                                          \
                                                                \
    struct nameV : MessageBase<                                 \
        nameV##CRTPImplementer

#define NYLON_MESSAGE_FIELD(field) , field##Field

#define NYLON_MESSAGE_END > {};

#endif // NYLON_MESSAGE_MACROS_H
