#pragma once
#include <cstdint>
#include <mutex>
#include "mmio.h"
class CI2CIO : public CMMIO
{
public:
    
    // Default constructor
    CI2CIO() : CMMIO() {};

public:

// Writes up to 4 bytes to the specified device                       
    void write(uint8_t deviceID, uint8_t dataBytes, uint32_t dataValue);

    // Writes up to 4 bytes to a specified register if a specified device
    void writeRegister(uint8_t deviceID,
                       uint8_t addrBytes, uint32_t addrValue, 
                       uint8_t dataBytes, uint32_t dataValue);

    // Reads up to 4 bytes from the specified register of a specified device
    uint32_t readRegister(uint8_t deviceID,
                          uint8_t addrBytes, uint32_t addrValue, 
                          uint8_t dataBytes, bool stopBeforeRead);

    // This is publicly accessible in order to give the outside world 
    // the ability to make multi-transaction operations atomic.
    std::recursive_mutex mutex_;

protected:

    uint8_t receiveByte(uint32_t microseconds);
};
