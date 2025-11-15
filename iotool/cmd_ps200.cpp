#include "helpers.h"

void cmd_ps200(int c)
{
    int32_t  reading;
    bool     b1;
    string   s;

    // The first keyword is our command
    const string& cmd = getKeyword(c++);

    // Is the user setting the direction of the pins on PX0?
    if (cmd.empty())
    {
        // Fetch a numeric value
        reading = fpga.getPs200Reading();
        cout << reading << "\n";                        
        exit(0);
    }

    if (cmd == "emulate")
    {
        s = getKeyword(c++);
        if (s.empty())
        {
            auto t  = fpga.getPs200Emu();
            b1      = std::get<0>(t);
            reading = std::get<1>(t);
            printf("%i %i\n", b1, reading);
            exit(0);
        }

        // Find out whether the user wants to turn it on
        b1 = (s == "true" || s == "on" || touint32(s) == 1);

        // Fetch the simulated inputs
        if (b1)
            reading = get32(c++);
        else
            reading = 0;

        // And set the emulation parameters
        fpga.setPs200Emu(b1, reading);
        exit(0);
    }

    // If we get here, we didn't recognize the command
    cerr << "syntax-error\n";
    exit(1);
}

