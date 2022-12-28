#include "ngrep_printer.h"
#include "ngrep_surveyor.h"

#include "nylon/nylon_message_builder.h"
#include "nylon/util/file_ptr.h"

#include <string_view>
#include <iostream>

NYLON_NAMESPACE_BEGIN

template <typename MessageDefiner>
Ngrep<MessageDefiner>::Ngrep(std::string const & protocolName)
    : protocolName_(protocolName)
    , primaryArgument_()
    , action_(Action::Unknown)
{

}

template <typename MessageDefiner>
void Ngrep<MessageDefiner>::init(int argc, char ** argv)
{
    if (argc < 2) {
        action_ = Action::PrintHelp;
    }

    primaryArgument_ = std::string_view(argv[1]);

    if (primaryArgument_ == "--survey" || primaryArgument_ == "-s") {
        action_ = parseSurveyArgs(argc, argv);
    }
    else if (primaryArgument_ == "--print" || primaryArgument_ == "-p") {
        action_ = parsePrintArgs(argc, argv);
    }
    else if (primaryArgument_ == "--help" || primaryArgument_ == "-h") {
        action_ = Action::PrintHelp;
    }
}

template <typename MessageDefiner>
void Ngrep<MessageDefiner>::run() const
{
    switch(action_) {
        case Action::Survey:    survey();               break;
        case Action::Print:     print();                break;
        case Action::PrintHelp: printHelp();            break;
        case Action::Unknown:   printUnknownArgument(); break;
    }
}

template <typename MessageDefiner>
typename Ngrep<MessageDefiner>::Action Ngrep<MessageDefiner>::parseSurveyArgs(int argc, char** argv)
{
    // Survey takes exactly three args. ${executableName}, --survey, and ${filename}.
    // We don't parse the executable name. We've already parsed --survey in init().

    if (argc < 3) {
        // if we don't have at least 3 args just print the help message
        return Action::PrintHelp;
    }

    fileName_ = std::string(argv[2]);

    // For now ignore the fact that we would accept more than 3 arguments and just
    // ignore any extra args, even though that's almost certainly a mistake on the
    // users part

    return Action::Survey;
}

template <typename MessageDefiner>
typename Ngrep<MessageDefiner>::Action Ngrep<MessageDefiner>::parsePrintArgs(int argc, char** argv)
{
    // Print takes at least 3 args. ${executableName}, --print, and ${filename}.
    // We don't parse the exectuable name. We've already parsed --print in init().

    if (argc < 3) {
        // if we don't have at least 3 args just print the help message
        return Action::PrintHelp;
    }

    fileName_ = std::string(argv[2]);

    return Action::Print;
}

template <typename MessageDefiner>
void Ngrep<MessageDefiner>::survey() const
{
    if (fileName_.empty()) {
        assert(!"Unexpected state: Asked to survey but did not receive a file name");
        printf("Unexpected state: Asked to survey but did not receive a file name");
        return;
    }

    constexpr auto bufferSize = 1024;
    char buffer[bufferSize];
    auto file = FilePtr(fopen(fileName_.c_str(), "r"));

    auto surveyor = Surveyor<MessageDefiner>();

    while (true) {
        auto const bytesRead = fread(buffer, sizeof(char), bufferSize, file.get());

        if (bytesRead <= 0) {
            break;
        }

        surveyor.survey({buffer, bytesRead});
    }

    for (auto const & [messageData, messageCount] : surveyor.messageCounts()) {
        printf("%s : %lu\n", messageData.messageName.c_str(), messageCount);
    }
}

template <typename MessageDefiner>
void Ngrep<MessageDefiner>::print() const
{
    if (fileName_.empty()) {
        assert(!"Unexpected state: Asked to print but did not receive a file name");
        printf("Unexpected state: Asked to print but did not receive a file name");
        return;
    }

    constexpr auto bufferSize = 1024;
    char buffer[bufferSize];
    auto file = FilePtr(fopen(fileName_.c_str(), "r"));

    auto printer = Printer<MessageDefiner>();
    printer.onMessage = [](std::string&& msg) { std::cout << msg << '\n'; };

    while (true) {
        auto const bytesRead = fread(buffer, sizeof(char), bufferSize, file.get());

        if (bytesRead <= 0) {
            break;
        }

        printer.print({buffer, bytesRead});
    }
}

template <typename MessageDefiner>
void Ngrep<MessageDefiner>::printHelp() const
{
    auto const tab = '\t';
    auto const newline = '\n';

    std::cout << "Ngrep: Explore " << protocolName_ << " Files"                                  << newline
                                                                                                 << newline
              << "Available arguments:"                                                          << newline
              << tab << "survey (s): print the number of each message type in the file."         << newline
              << tab << tab << "Format: "                                                        << newline
              << tab << tab << tab << "ngrep --survey ${filename}"                               << newline
              << tab << "print (p): pretty print the messages using the following sub arguments" << newline
              << tab << tab << "all: default behavior. Print all messages"                       << newline
              << tab << "help (h): print this help messages"                                     << newline
              ;
}

template <typename MessageDefiner>
void Ngrep<MessageDefiner>::printUnknownArgument() const
{
    std::cout << "Unrecognized argument: " << primaryArgument_ << '\n';
}

NYLON_NAMESPACE_END
