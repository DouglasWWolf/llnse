#include <iostream>
#include "gpioAccessApi.h"


void execute();

//=============================================================================
// The folder where the llnse FIFOs live depends on what architecture we're on
//=============================================================================
#ifdef __aarch64__
    std::string fifo_folder = "/fs";
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
        std::cerr << e.file_ << "(" << e.line_ << "): " << e.what() << "\n";
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
    std::cout << "The ping response was " << pong << "\n";

    // Last but not least, let's have the llnse server generate an 
    // exception to make sure we handle it properly
    gpio.fault(42);

}
//=============================================================================
