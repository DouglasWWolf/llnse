#pragma once
#include <stdint.h>
#include <string>
#include <memory>
#include "llnseApi.h"

namespace gpioAccess
{
    // Returned by "getRtl()"
    struct rtl_t
    {
        char     version[16];
        char     date[16];
        char     time[16];
        char     hash[64];
        uint32_t type;        
        uint32_t subtype;
    };

    //=========================================================================
    // Access layer for on-board GPIO leds and switches
    //=========================================================================
    class CGpioAccess
    {   
    
    public:

        // Call this to ping the server
        uint32_t    ping(uint32_t value);

        // Call this to force the server to generate a fault response
        void        fault(uint32_t error);

        // Call this to set the four on-board LEDs 
        void        setLeds(uint32_t leds);
        
        // Call this to retreive the four on-board DIP switchesa
        uint32_t    getSwitches();

        // Fetch identify information about the RTL design
        rtl_t       getRtl();

        // Set the directions of the 16 pins of PX0
        void        setPx0Iodir(uint16_t inputs);

        // Enable the optional pullup resistors of the 16 input pins of PX0
        void        setPx0Pullup(uint16_t pins);

        // Set the GPIO output values of the 16 pins of PX0
        void        setPx0Gpio(uint16_t pins);

        // Read the value of the 16 pins of PX0
        uint16_t    getPx0Gpio();


    public:
         CGpioAccess(llnse::Connection& conn); 
        ~CGpioAccess();

        // Objects of this class can't be copied;
        CGpioAccess(const CGpioAccess&) = delete;
        CGpioAccess& operator=(const CGpioAccess&) = delete;

    protected:

        // This is a connection to the llnse server
        llnse::Connection& conn_;

    };
    //=========================================================================

}
