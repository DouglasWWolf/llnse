#pragma once
#include <cstdint>
#include <mutex>
#include "mmio.h"
class CI2CIO : public CMMIO
{
public:
    
    // Default constructor
    CI2CIO() : CMMIO() {}

public:

    // Saves a bus identifer for error message
    void        init(uint32_t bus_id) {bus_id_ = bus_id;}

    // Writes up to 4 bytes to the specified device                       
    void        write(uint8_t device_id, uint8_t data_bytes, uint32_t data_value);

    // Writes up to 4 bytes to a specified register if a specified device
    void        write_register(uint8_t device_id,
                               uint8_t addr_bytes, uint32_t addr_value, 
                               uint8_t data_bytes, uint32_t data_value);

    // Reads up to 4 bytes from the specified register of a specified device
    uint32_t    read_register(uint8_t device_id,
                              uint8_t addr_bytes, uint32_t addr_value, 
                              uint8_t data_bytes, bool stop_before_read);

    // This is publicly accessible in order to give the outside world 
    // the ability to make multi-transaction operations atomic.
    std::recursive_mutex mutex_;

protected:

    // Waits for a byte to arrive on RX_FIFO and fetches it
    uint8_t receive_byte(uint32_t microseconds);

    // An integer for distinguish one I2C bus from another in error messages
    uint32_t bus_id_;

    // The device ID for the currently executing read or write
    uint8_t  device_id_;
};
