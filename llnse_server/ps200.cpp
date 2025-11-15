#include "global.h"


//============================================================================
// init() - Save a pointer to the bus and the device-ID
//============================================================================
void CPS200::init(CI2CIO& bus, uint8_t device_id)
{
    // Save a pointer to the I2C bus
    bus_ = &bus;
    
    // Save the device ID
    device_id_ = device_id;

    // By default, we're not in emulation mode
    emulate_ = false;

    // The emulated pressure reading at power up
    emu_reading_ = -1;
}
//============================================================================


//============================================================================
// get_reading() - Gets the 32-bit value of the pressure reading
//============================================================================
int32_t CPS200::get_reading()
{
    uint8_t octets[4];

    // If we're in emulation mode...
    if (emulate_) return emu_reading_;

    // Fetch the big-endian version of the reading
    uint32_t be = bus_->read(device_id_, 4);

    // Reverse the order of the bytes
    octets[0] = (be      ) & 0xFF;    
    octets[1] = (be >>  8) & 0xFF;
    octets[2] = (be >> 16) & 0xFF;
    octets[3] = (be >> 24) & 0xFF;

    // And return that as a signed integer
    return *(int32_t*)octets;
}
//============================================================================


//============================================================================
// set_emulation() - Turn emulation on/off and set the value of the emulated
//                   reading
//============================================================================
void CPS200::set_emulation(bool flag, int32_t reading)
{
    emulate_ = flag;
    emu_reading_ = reading;

    if (flag)
        LOG_INFO("PS200 emulation set to %i", reading); 
    else
        LOG_INFO("PS200 emulation off"); 
}
//============================================================================


//============================================================================
// get_emulation() - Fetches the current emulation settings
//============================================================================
std::tuple<bool, int32_t> CPS200::get_emulation()
{
    return std::make_tuple(emulate_, emu_reading_);
}
//============================================================================

