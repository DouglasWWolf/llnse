#include "mcp23017.h"

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
}
//============================================================================


//============================================================================
// set_dir() - Sets the directions of the 16 pins.  A '1' in a bit position
//             means that pin is an input
//============================================================================
void CMCP23017::set_dir(uint16_t input_pins)
{
    bus_->write_register(device_id_, 1, PX_IODIR, 2, swap(input_pins));
}
//============================================================================



//============================================================================
// set_pullup() - Enables pullups for the defined bits
//============================================================================
void CMCP23017::set_pullup(uint16_t pins)
{
    bus_->write_register(device_id_, 1, PX_GPPU, 2, swap(pins));
}
//============================================================================


//============================================================================
// set_gpio() - Sets the value of GPIO output bits
//============================================================================
void CMCP23017::set_gpio(uint16_t pins)
{
    bus_->write_register(device_id_, 1, PX_GPIO, 2, swap(pins));
}
//============================================================================



//============================================================================
// get_gpio() - Gets the value of GPIO input bits
//============================================================================
uint16_t CMCP23017::get_gpio()
{
    uint16_t pins = bus_->read_register(device_id_, 1, PX_GPIO, 2, false);
    return swap(pins);
}
//============================================================================
