#include "helpers.h"

void cmd_rtl(int c)
{
    gpioAccess::rtl_t rtl;

    // The first keyword is our command
    const string& cmd = getKeyword(c++);

    // Is the user asking for the RTL version?
    if (cmd == "version")
    {   
        rtl = fpga.getRtl();
        cout << rtl.version << "\n";
        exit(0);
    }


    // Is the user asking for the RTL date?
    if (cmd == "date")
    {   
        rtl = fpga.getRtl();
        cout << rtl.date << "\n";
        exit(0);
    }


    // Is the user asking for the RTL time?
    if (cmd == "time")
    {   
        rtl = fpga.getRtl();
        cout << rtl.time << "\n";
        exit(0);
    }

    // Is the user asking for the RTL type?
    if (cmd == "type")
    {   
        rtl = fpga.getRtl();
        cout << rtl.type << "\n";
        exit(0);
    }

    // Is the user asking for the RTL repo hash?
    if (cmd == "hash")
    {   
        rtl = fpga.getRtl();
        cout << rtl.hash << "\n";
        exit(0);
    }

    // If we get here, we didn't recognize the command
    cerr << "syntax-error\n";
    exit(1);
};
