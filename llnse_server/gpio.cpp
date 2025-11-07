#include "gpio.h"
#include "global.h"


void CGPIO::write(uint32_t value)
{
    uint32_t& reg = *(uint32_t*)iobase_;
    reg = value;
};



uint32_t CGPIO::read()
{
    uint32_t& reg = *(uint32_t*)iobase_;
    return reg;
};

