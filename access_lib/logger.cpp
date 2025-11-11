#include <time.h>
#include <cstring>
#include "logger.h"

using namespace log;

//=============================================================================
// init() - Initialize the logger.  Call once, soon after program start
//=============================================================================
void CLogger::init(const std::string& moduleName)
{
    module_ = moduleName;
}
//=============================================================================


//=============================================================================
// log() - General purpose logging routine
//=============================================================================
void CLogger::log(Severity sev, const char* file, uint32_t line,
                  const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    log(sev, file, line, fmt, ap);
    va_end(ap);
}
//=============================================================================


//=============================================================================
// log() - General purpose logging routine
//=============================================================================
void CLogger::log(Severity sev, const char* file, uint32_t line,
     const char* fmt, va_list& ap)
{
    char entry[1024], *p;
    const char* end = entry + sizeof(entry) - 1;
    char datestring[64], tz[8];
    const char* severity;
    struct timespec ts;
    struct tm tm;

    // Fetch the timestamp;
    clock_gettime(CLOCK_REALTIME, &ts);

    // Get a pointer to the string that represents our severity
    switch(sev)
    {
        case TRACE: severity = "TRACE"; break;
        case DEBUG: severity = "DEBUG"; break;
        case INFO : severity = "INFO "; break;
        case WARN : severity = "WARN "; break;
        case ERROR: severity = "ERROR"; break;
        case FATAL: severity = "FATAL"; break;
        default:
            return;
    }

    // Our log entry begins with the severity and a thread-ID of zero
    sprintf(entry, "[%s] [0] ", severity);

    // Convert the "seconds" portion of the timestamp to a tm structure
    localtime_r(&ts.tv_sec, &tm);

    // Now format that as YYYY-MM-DD HH:MM:SS
    strftime(datestring, sizeof(datestring), "%Y-%m-%d %H:%M:%S", &tm);
    
    // Fetch the timezone information
    strftime(tz, sizeof(tz), "%z", &tm);

    // Fetch the formatted date string, including milliseconds
    p = strchr(entry, 0);
    snprintf(p, end - p, "[%s.%03ld%s] ", datestring, ts.tv_nsec/1000000, tz);

    // Stuff the module, file, and line number into "entry"
    p = strchr(entry, 0);
    snprintf(p, end - p, "[%s:%s:%i] ", module_.c_str(), file, line);

    // Stuff the formatted error message into "emtry"
    p = strchr(entry, 0);    
    vsnprintf(p, end - p, fmt, ap);
   
    
    printf("%s\n", entry);

}
//=============================================================================


