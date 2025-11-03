#include "gpio.h"
#include "global.h"


void CGPIO::map(uint32_t address)
{
    // Map the MMIO into userspace
    if (!mmio_.map(address, 0x1000))
    {
        throwRuntime
        (
            "Failed to map GPIO 0x%04X_%04X",
            address >> 16,
            address & 0xFFFF
        );        
    }

}

void CGPIO::write(uint32_t value)
{
    uint32_t& reg = *(uint32_t*)mmio_.ptr;
    reg = value;
};



uint32_t CGPIO::read()
{
    uint32_t& reg = *(uint32_t*)mmio_.ptr;
    return reg;
};

