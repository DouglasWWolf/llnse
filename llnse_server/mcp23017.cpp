#include "global.h"

// These are the only MCP23017 registers that we care about
enum px_reg_t
{
   PX_IODIR = 0x00,
   PX_GPPU  = 0x0C,
   PX_GPIO  = 0x12
};



//============================================================================
// Swap the two bytes in a 16-bit integer.  We do this because the order of
// the chip registers is such that PORTB is the low-order 8 bits of the 16
// bit wide register, but our callers want PORTA to be the low-order 8-bits
//============================================================================
static uint16_t swap(uint16_t value)
{
    return (value >> 8) | (value << 8);
}
//============================================================================


//============================================================================
// init() - Save a pointer to the bus and the device-ID
//============================================================================
void CMCP23017::init(CI2CIO& bus, uint8_t device_id)
{
    // Save a pointer to the I2C bus
    bus_ = &bus;
    
    // Save the device ID
    device_id_ = device_id;

    // By default, we're not in emulation mode
    emulate_ = false;

    // When the chip powers up, all I/O pins are inputs
    emu_iodir_ = 0xFFFF;

    // At powerup, all output latches are off
    emu_outputs_ = 0x0000;

    // At powerup, emulate all inputs being off
    emu_inputs_ = 0x0000;

}
//============================================================================


//============================================================================
// When we're in hardware emulation mode, this handles the clearing of the
// solenoid-fault input pin
//============================================================================
void CMCP23017::auto_clear_solenoid_fault()
{
    // When this output bit is 0, the SOLENOID_FAULT input goes to 0
    const uint16_t SOLENOID_FAULT_CLEAR = (1<<7);
    
    // This is cleared by driving SOLENOID_FAULT_CLEAR to 0
    const uint16_t SOLENOID_FAULT       = (1<<6);

    // Find  out which output pins are on
    uint16_t true_outputs = emu_outputs_ & ~emu_iodir_;
    
    // When the SOLENOID_FAULT_CLEAR output is 0, clear
    // the SOLENOID_FAULT input
    if ((true_outputs & SOLENOID_FAULT_CLEAR) == 0)
    {
        emu_inputs_ &= ~SOLENOID_FAULT;
    }
}
//============================================================================



//============================================================================
// set_dir() - Sets the directions of the 16 pins.  A '1' in a bit position
//             means that pin is an input
//============================================================================
void CMCP23017::set_dir(uint16_t input_pins)
{
    // Save the direction of the I/O pins for emulation
    emu_iodir_ = input_pins;
    
    // If we're in hardware emulation mode...
    if (emulate_)
    {
        auto_clear_solenoid_fault();
        LOG_INFO("MCP23017 IODIR set to 0x%04X", input_pins);
        return;
    }

    // Write to the 16-bit IODIR register of the chip
    bus_->write_register(device_id_, 1, PX_IODIR, 2, swap(input_pins));
}
//============================================================================



//============================================================================
// set_pullup() - Enables pullups for the defined bits
//============================================================================
void CMCP23017::set_pullup(uint16_t pins)
{
    // If we're in emulation mode...
    if (emulate_)
    {
        LOG_INFO("MCP23017 GPPU set to 0x%04X", pins);
        return;
    }

    bus_->write_register(device_id_, 1, PX_GPPU, 2, swap(pins));
}
//============================================================================


//============================================================================
// set_gpio() - Sets the value of GPIO output bits
//============================================================================
void CMCP23017::set_gpio(uint16_t pins)
{
    // Save emulation settings
    emu_outputs_ = pins;

    // If we're in emulation mode...
    if (emulate_)
    {
        auto_clear_solenoid_fault();
        LOG_INFO("MCP23017 GPIO set to 0x%04X", pins);
        return;
    }

    bus_->write_register(device_id_, 1, PX_GPIO, 2, swap(pins));
}
//============================================================================



//============================================================================
// get_gpio() - Gets the value of GPIO input bits
//============================================================================
uint16_t CMCP23017::get_gpio()
{
    // If we're in emulation mode...
    if (emulate_)
    {
        uint16_t true_inputs  = emu_inputs_  &  emu_iodir_;
        uint16_t true_outputs = emu_outputs_ & ~emu_iodir_; 
        return true_inputs | true_outputs;
    }

    uint16_t pins = bus_->read_register(device_id_, 1, PX_GPIO, 2, false);
    return swap(pins);
}
//============================================================================


//============================================================================
// set_emulation() - Turn emulation on/off and set the value of the emulated
//                   input pins
//============================================================================
void CMCP23017::set_emulation(bool flag, uint16_t input_pins)
{
    emulate_ = flag;
    emu_inputs_ = input_pins; 
    if (flag)
        LOG_INFO("MCP23017 emulation set to 0x%04X", input_pins); 
    else
        LOG_INFO("MCP23017 emulation off"); 
}
//============================================================================


//============================================================================
// get_emulation() - Fetches the current emulation settings
//============================================================================
std::tuple<bool, uint16_t> CMCP23017::get_emulation()
{
    return std::make_tuple(emulate_, emu_inputs_);
}
//============================================================================

