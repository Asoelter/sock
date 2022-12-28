#ifndef NYLON_PRETTY_PRINTER_H
#define NYLON_PRETTY_PRINTER_H

#include "namespace.h"
#include "nylon_char_field.h"
#include "nylon_message_base.h"
#include "nylon_string_field.h"
#include "nylon_uint8_field.h"

#include <string>
#include <variant>

NYLON_NAMESPACE_BEGIN

// The text style for console output is manipulated in the following way. Special
// codes are printed to stdout to manipulate the style. The codes are in the
// format:
//      \033[${control_setting};${color_setting}m
//
// Note: It's not really important, but for extra information: the \033 is the
// ascii representation of escape
//
// The possible color settings are:           The possible control settings are:
// -------------------------------------      ----------------------------
// |  color  | foreground | background |      |     setting     | number |
// |  black  |     30     |     40     |      |      reset      |    0   |
// |   red   |     31     |     41     |      |   bold/bright   |    1   |
// |  green  |     32     |     42     |      |    underline    |    4   |
// |  yellow |     33     |     43     |      |     inverse     |    7   |
// |  blue   |     34     |     44     |      | bold/bright off |   21   |
// | magenta |     35     |     45     |      |  underline off  |   24   |
// |   cyan  |     36     |     46     |      |   inverse off   |   27   |
// |  white  |     37     |     47     |

struct ConsoleColor {
    static inline constexpr auto boldBlue = "\033[1;34m";
    static inline constexpr auto cyan = "\033[36m";
    static inline constexpr auto green = "\033[32m";
    static inline constexpr auto yellow = "\033[33m";
    static inline constexpr auto boldRed = "\033[1;31m";
    static inline constexpr auto magenta = "\033[35m";
    static inline constexpr auto reset = "\033[0m";
};

template <typename MessageDefiner>
std::string prettyPrint(typename MessageDefiner::MessageType const & msg);

template <typename MessageType>
std::string prettyPrintMessage(MessageType const & msg);

// utility functions for users of this class who want to
// easily use the same color schemese library uses
inline std::string prettyField(std::string const & text);
inline std::string prettyField(const char * text);
inline std::string prettyString(std::string const & text);
inline std::string prettyChar(char c);
inline std::string prettyNumber(auto number);
inline std::string prettyEqualsSign();
inline std::string prettyError(std::string const & text);

template <typename Field>
struct PrettyPrinter
{
    std::string operator()(Field const & /*field*/);
};

template <typename Derived>
struct PrettyPrinter<CharField<Derived>>
{
    std::string operator()(CharField<Derived> const & field);
};

template <typename Derived>
struct PrettyPrinter<StringField<Derived>>
{
    std::string operator()(StringField<Derived> const & field);
};

template <typename Derived>
struct PrettyPrinter<UInt8Field<Derived>>
{
    std::string operator()(UInt8Field<Derived> const & field);
};

NYLON_NAMESPACE_END

#include "nylon_pretty_printer_inline.h"

#endif // NYLON_PRETTY_PRINTER_H
