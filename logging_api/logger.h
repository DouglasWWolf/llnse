#pragma once
#include <string>
#include <memory>

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
    bool    init(const std::string& moduleName);

    // Call this to write a message to the log
    void    log(Severity sev, const char* file, uint32_t line, const std::string& fmt, ...);

    // Convenience method for writing a trace log entry
    void    trace(const std::string& fmt, ...);

public:

    // Regular constructor/destructor
     CLogger();
    ~CLogger();

    // Objects of this class can't be copied;
    CLogger(const CLogger&) = delete;
    CLogger& operator=(const CLogger&) = delete;

protected:
    class Impl;
    std::unique_ptr<Impl> impl_;

};


} // End of namespace
