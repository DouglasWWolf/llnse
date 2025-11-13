//========================================================================
// This is a tool for generating log entries from the command line
//========================================================================
#include <unistd.h>
#include <cstdio>
#include <iostream>
#include "logger.h"

using namespace log;

CLogger logger;
Severity sev = INFO;
std::string message;


//========================================================================
// Parse the command-line tokens into a log message
//========================================================================
void parse_command_line(const char** argv)
{
    int i=1;

    // Loop through the command line arguments to build the log message
    while (argv[i])
    {
        // Fetch the next token from the command line
        std::string token = argv[i++];

        // Ignore zero-length tokens
        if (token.empty()) continue;

        // Allow the user to specify the message severity
        if (token == "-trace") {sev = TRACE; continue;}
        if (token == "-debug") {sev = DEBUG; continue;}
        if (token == "-info" ) {sev = INFO ; continue;}        
        if (token == "-warn" ) {sev = WARN ; continue;}
        if (token == "-error") {sev = ERROR; continue;}
        if (token == "-fatal") {sev = FATAL; continue;}

        // If this is an unrecognized command line option, complain
        if (token[0] == '-')
        {
            std::cerr << "Unrecognized option " << token << "\n";
            exit(1);
        }

        // Append this token to our log message
        if (!message.empty()) message += " ";
        message += token;
    }
}
//========================================================================


//========================================================================
// Execution starts here
//========================================================================
int main(int argc, const char** argv)
{
    // If no command-line arguments, do nothing
    if (argv[1] == nullptr) exit(0);

    // Parse the command line
    parse_command_line(argv);

    // Initialize the logging API
    logger.init("log");

    // Create the log entry
    logger.log(sev, "", 0, message.c_str());

    // Tell the outside world that all is well
    exit(0);
}
//========================================================================


