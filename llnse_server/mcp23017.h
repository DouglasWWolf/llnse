#pragma once
#include "i2cio.h"

class CMCP23017
{
public:

    // Called once at startup
    void init(CI2CIO& bus, uint8_t device_id);

    // Set the direction of the 16 GPIO pins
    void set_dir(uint16_t input_pins);

    // Enables pullup resistors on the specified pins
    void set_pullup(uint16_t pins);

    // Set the value of GPIO output pins
    void set_gpio(uint16_t pins);

    // Fetch the value of GPIO input pins
    uint16_t get_gpio();

protected:

    // Pointer to the I2C bus
    CI2CIO* bus_;

    // The device ID of the port expander
    uint8_t device_id_;
};  
