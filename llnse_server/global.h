#pragma once
#include <string>
#include "gpio.h"

void throwRuntime(const char* fmt, ...);

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


    CGPIO   leds;
    CGPIO   switches;
};
