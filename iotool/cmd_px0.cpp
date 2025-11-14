#include "helpers.h"

void cmd_px0(int c)
{
    uint32_t value;
    string   s;
    bool     b1;
    uint16_t u161;

    // The first keyword is our command
    const string& cmd = getKeyword(c++);

    // Is the user setting the direction of the pins on PX0?
    if (cmd == "iodir")
    {
        // Fetch a numeric value
        value = getu32(c++);

        // Set the directions of the 16 pins on PX0
        fpga.setPx0Iodir(value);
        exit(0);
    }


    // Is the user enabling pullup resistors on PX0?
    if (cmd == "pullup")
    {
        // Fetch a numeric value
        value = getu32(c++);

        // Enable/disable optional pullup resistors on PX0
        fpga.setPx0Pullup(value);
        exit(0);
    }

    // Is the user setting / getting GPIO outputs on PX0?
    if (cmd == "gpio")
    {
        s = getKeyword(c++);

        if (s.empty())
        {
            value = fpga.getPx0Gpio();
            cout << value << "\n";                        
        }
        else
        {
            fpga.setPx0Gpio(touint32(s));
        }
        exit(0);
    }


    if (cmd == "emulate")
    {
        s = getKeyword(c++);
        if (s.empty())
        {
            auto t  = fpga.getPx0Emu();
            b1    = std::get<0>(t);
            u161  = std::get<1>(t);
            printf("%i %i 0x%04X\n", b1, u161, u161);
            exit(0);
        }

        // Find out whether the user wants to turn it on
        b1 = (s == "true" || s == "on" || touint32(s) == 1);

        // Fetch the simulated inputs
        if (b1)
            u161 = getu32(c++);
        else
            u161 = 0;

        // And set the emulation parameters
        fpga.setPx0Emu(b1, u161);
        exit(0);
    }

    // If we get here, we didn't recognize the command
    cerr << "syntax-error\n";
    exit(1);
}

