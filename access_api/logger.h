#pragma once
#include <string>
#include <cstdarg>
namespace log {

enum Severity
{
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    CRITICAL,
    FATAL = CRITICAL,
    OFF
};

class CLogger
{
public:

    // Initialize once, just after program startup
    void    init(const std::string& moduleName);

    // Call this to write a message to the log
    void    log(Severity sev, const char* file, uint32_t line, const char* fmt, ...);
    
    // Convenience method for writing a fatal log entry
    void    fatal(const char* fmt, ...);

    // Convenience method for writing a trace log entry
    void    trace(const char* fmt, ...);

protected:

    // This is the underlying "log()" mechanism that all the other logging methods call
    void    log(Severity sev, const char* file, uint32_t line, const char* fmt, va_list& ap);

    // The name of the module that the logger is running in
    std::string  module_;
};



} // End of namespace
