#pragma once
#include <stdint.h>
#include <string>
#include <memory>
#include "llnseApi.h"

namespace gpioAccess
{

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

        // Call this to set the 4 on-board LEDs 
        void        setLeds(uint32_t leds);
        
        // Call this to retreive the four on-board DIP switchesa
        uint32_t    getSwitches();


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
