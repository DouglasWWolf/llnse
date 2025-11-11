#include <time.h>
#include <cstring>
#include "loggerImpl.h"

using namespace log;

//=============================================================================
// init() - Initialize the logger.  Call once, soon after program start
//=============================================================================
bool CLogger::init(const std::string& moduleName)
{
    return p_impl->init(moduleName);
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
    p_impl->log(sev, file, line, fmt, ap);
    va_end(ap);
}
//=============================================================================



//=============================================================================
// Constructor and destructor
//=============================================================================
CLogger::CLogger() : p_impl(std::make_unique<CLogger::Impl>()) {};
CLogger::~CLogger() = default;
//=============================================================================

