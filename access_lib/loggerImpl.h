#pragma once
#include <string>
#include <cstdarg>
#include <memory>
#include "logger.h"
#include "udp_msg_sender.h"

namespace log {


class CLogger::Impl
{
public:

    // Initialize once, just after program startup
    bool    init(const std::string& moduleName);
    
    // Convenience method for writing a fatal log entry
    void    fatal(const char* fmt, ...);

    // Convenience method for writing a trace log entry
    void    trace(const char* fmt, ...);

    // This is the underlying "log()" mechanism that all the other logging methods call
    void    log(Severity sev, const char* file, uint32_t line, const char* fmt, va_list& ap);

protected:

    // The name of the module that the logger is running in
    std::string  module_;

    // This is the UDP log-message sender
    UdpSocket::Sender sender_;
};


} // End of namespace
