#pragma once
#include <tuple>
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

    // Turn emulation on/off, and set the value of the emulated inputs
    void set_emulation(bool flag, uint16_t input_pins);

    // Fetch the current emulation settings
    std::tuple<bool, uint16_t> get_emulation();

protected:

    // Pointer to the I2C bus
    CI2CIO* bus_;

    // The device ID of the port expander
    uint8_t device_id_;

    // If this is true, we're emulating the hardware
    bool    emulate_;

    // This tracks the value of the most recent set_gpio()
    uint16_t emu_outputs_;

    // When we're in emulation mode, this contains the
    // values of the input pins    
    uint16_t emu_inputs_;

    // This tracks the value of the most recent set_dir()
    uint16_t emu_iodir_;

    // This routine handles the clearing of the solenoid fault bit
    // when we're in hardware emulation mode
    void auto_clear_solenoid_fault();

};  
