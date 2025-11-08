#include <unistd.h>
#include <iostream>
#include "gpioAccessApi.h"
using std::cout, std::cerr;

void execute();



//=============================================================================
// The folder where the llnse FIFOs live depends on what architecture we're on
//=============================================================================
#ifdef __aarch64__
    std::string fifo_folder = "/etc/fifos";
#else
    std::string fifo_folder = "/home/wolf/fifos";
#endif
//=============================================================================



//=============================================================================
// main() - Just calls execute() and displays exceptions
//=============================================================================
int main(int argc, const char** argv)
{
    try
    {
        execute();
    }
    catch(const llnse::runtime_error& e)
    {
        cerr << e.file_ << "(" << e.line_ << "): " << e.what() << "\n";
        exit(1);        
    }
    
}
//=============================================================================


// This will be a connection to the llnse server
llnse::Connection conn;

// Acccess layer to the VPK120 on-board LEDs and DIP switches
gpioAccess::CGpioAccess gpio(conn);


//=============================================================================
// Here we demonstrate interacting with the hardware
//=============================================================================
void execute()
{
    // Connect to the llnse server
    conn.connect(fifo_folder, 3);
    
    // Ping the server and display the response
    int pong = gpio.ping(314159);
    cout << "The ping response was " << pong << "\n";

    // Fetch the RTL identity information and display it
    auto rtl = gpio.getRtl();
    cout << "RTL Type    = " << rtl.type    << "\n";
    cout << "RTL Subtype = " << rtl.subtype << "\n";
    cout << "RTL Version = " << rtl.version << "\n";
    cout << "RTL Date    = " << rtl.date    << "\n";
    cout << "RTL Time    = " << rtl.time    << "\n";
    cout << "RTL Hash    = " << rtl.hash    << "\n";

    // Blink the LEDs in a pattern
    cout << "Blinking the LEDS for a moment...\n";
    for (uint32_t i=0; i<16; ++i)
    {
        gpio.setLeds(9);
        usleep(250000);
        gpio.setLeds(6);
        usleep(250000);
    }

    // Fetch the value of the 4 DIP switches and display it
    uint32_t dipSwitches = gpio.getSwitches();
    cout << "The DIP switches are set to: " << dipSwitches << "\n";

    // Last but not least, let's have the llnse server generate an 
    // exception to make sure we handle it properly
    cout << "The next message we see had better be an exception!\n\n";
    gpio.fault(42);

}
//=============================================================================
