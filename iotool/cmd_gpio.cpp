#include "helpers.h"

void cmd_gpio(int c)
{
    uint32_t value;

    // The first keyword is our command
    const string& cmd = getKeyword(c++);

    // Is the user asking for the state of the GPIO DIP switches?
    if (cmd == "switches" || cmd == "dips")
    {   
        value = fpga.getSwitches();
        cout << value << "\n";
        exit(0);
    }

    // Is the user setting the GPIO LEDs?
    if (cmd == "leds")
    {   
        // Fetch a numeric value
        value = getu32(c++);

        // And drive the LEDS to the specified value
        fpga.setLeds(value);
        exit(0);
    }

    // If we get here, we didn't recognize the command
    cerr << "syntax-error\n";
    exit(1);

}

