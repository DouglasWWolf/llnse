#pragma once
#include <string>
#include "gpio.h"
#include "rtlid.h"
#include "i2cio.h"
#include "mcp23017.h"
#include "logger.h"

void throwRuntime(const std::string& fmt, ...);

std::string format(const char* fmt, ...);

struct global_t
{
    #ifdef __aarch64__
        std::string fifo_folder = "/etc/fifos";    
    #else
        std::string fifo_folder = "/home/wolf/fifos";
    #endif

    #ifdef __aarch64__
        std::string memmap = "/etc/memory.map";    
    #else
        std::string memmap = "/home/wolf/memory.map";
    #endif


    CGPIO        leds;
    CGPIO        switches;
    CRtlId       rtlid;
    CI2CIO       i2c_0;
    CMCP23017    px0;
    log::CLogger Logger;
};

extern global_t g;

#define LOG_FATAL(fmt, ...) g.Logger.log(log::FATAL, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
