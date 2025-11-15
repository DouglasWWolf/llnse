#pragma once
#include <tuple>
#include "i2cio.h"

class CPS200
{
public:

    // Called once at startup
    void init(CI2CIO& bus, uint8_t device_id);

    // Fetch a reading from the pressure sensor
    int32_t get_reading();

    // Turn emulation on/off, and set the value of the emulated inputs
    void set_emulation(bool flag, int32_t reading);

    // Fetch the current emulation settings
    std::tuple<bool, int32_t> get_emulation();

protected:

    // Pointer to the I2C bus
    CI2CIO* bus_;

    // The device ID of the port expander
    uint8_t device_id_;

    // If this is true, we're emulating the hardware
    bool    emulate_;

    // This is the emulated reading
    int32_t emu_reading_;
};  
