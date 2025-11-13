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

    // This is the underlying "log()" mechanism that all the other logging methods call
    void    log(Severity sev, const char* file, uint32_t line, const std::string& fmt, va_list& ap);

protected:

    // The name of the module that the logger is running in
    std::string  module_;

    // This is the UDP log-message sender
    UdpSocket::Sender sender_;
};


} // End of namespace
