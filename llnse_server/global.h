#pragma once
#include <string>
#include "gpio.h"

void throwRuntime(const char* fmt, ...);

struct global_t
{
    #ifdef __aarch64__
        std::string fifo_folder = "/fs";    
    #else
        std::string fifo_folder = "/home/wolf/fifos";
    #endif

    CGPIO   leds;
    CGPIO   switches;
};
