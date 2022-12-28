#ifndef NGREP_H
#define NGREP_H

#include "namespace.h"
#include "ngrep_printer.h"

#include "util/typelist.h"

#include <string>

NYLON_NAMESPACE_BEGIN

template <typename MessageDefiner>
class Ngrep
{
public:
    Ngrep(std::string const & protocolName = "nylon");

    void init(int argc, char ** argv);

    void run() const;

private:
    enum class Action
    {
        Survey,
        Print,
        PrintHelp,
        Unknown
    };

    Action parseSurveyArgs(int argc, char** argv);
    Action parsePrintArgs(int argc, char** argv);

    void survey() const;
    void print() const;
    void printHelp() const;
    void printUnknownArgument() const;

private:
    std::string protocolName_;
    std::string primaryArgument_;
    std::string fileName_;
    Action action_;
};

NYLON_NAMESPACE_END

#include "ngrep_inline.h"

#endif // NGREP_H
