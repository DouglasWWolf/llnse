//=============================================================================
// iotool - An command-line tool for interacting with the low-level NSE
//          server.
//=============================================================================
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include "gpioAccessApi.h"
using std::cout, std::cerr, std::string, std::vector;

void execute();
void parseCommandLine(const char** argv);

// This is the list of keywords from the command line
vector<string> keyword;

// This will be a connection to the llnse server
llnse::Connection conn;

// Acccess layer to the VPK120 on-board LEDs and DIP switches
gpioAccess::CGpioAccess fpga(conn);


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
// getKeyword() - Returns the specified keyword or ""
//=============================================================================
string getKeyword(int index)
{
    if (index < keyword.size()) return keyword[index];
    return "";
}
//=============================================================================


//=============================================================================
// touint32() - Converts a string to a 32-bit integer
//=============================================================================
uint32_t touint32(const string& s)
{
    char buffer[1000], *out = buffer;

    const char* in = s.c_str();

    // Strip underscores from the input string
    while (*in)
    {
        if (*in == '_')
        {
            ++in;
            continue;
        }
        *out++ = *in++;      
    }
    *out = 0;

    // Return the binary value of the string
    return strtoul(buffer, nullptr, 0);
}
//=============================================================================


//=============================================================================
// main() - Just calls execute() and displays exceptions
//=============================================================================
int main(int argc, const char** argv)
{
    try
    {
        parseCommandLine(argv);
        execute();
    }
    catch(const llnse::runtime_error& e)
    {
        cerr << e.file_ << "(" << e.line_ << "): " << e.what() << "\n";
        exit(1);        
    }
    
}
//=============================================================================


//=============================================================================
// parseCommandLine() - Parses the command-line options
//=============================================================================
void parseCommandLine(const char** argv)
{
    int i=1;
    
    // Gather all of the tokens on the command line into a vector
    while (argv[i])
    {
        string token = argv[i++];
        keyword.push_back(token);
    };

}
//=============================================================================


//=============================================================================
// showHelp() - Display help and exit
//=============================================================================
void showHelp()
{
    printf("Available commands:\n");
    printf("   iotool rtl-version\n");
    printf("   iotool rtl-date\n");
    printf("   iotool rtl-time\n");
    printf("   iotool rtl-hash\n");
    printf("   iotool rtl-type\n");
    printf("   iotool gpio-switches\n");
    printf("   iotool gpio-leds <value>\n");
    printf("   iotool px0-iodir <value>\n");
    printf("   iotool px0-pullup <value>\n");    
    printf("   iotool px0-gpio [new_value]\n");        

}
//=============================================================================


//=============================================================================
// Fetches a keyword as a uint32_t
//=============================================================================
uint32_t getu32(int index)
{
    std::string s = getKeyword(index);
    if (s.empty())
    {
        cerr << "Missing parameter\n";
        exit(1);            
    }
    return touint32(s);
}
//=============================================================================




//=============================================================================
// execute() - Carry out the command from the command line
//=============================================================================
void execute()
{
    int c=0;
    uint32_t value;
    string s;
    gpioAccess::rtl_t rtl;

    if (keyword.size() == 0)
    {
        showHelp();
        exit(0);
    }

    // Connect to the llnse server
    conn.connect(fifo_folder, 42);
   
    // The first keyword is our command
    string& cmd = keyword[c++];

    // Is the user asking for the RTL version?
    if (cmd == "rtl-version")
    {   
        rtl = fpga.getRtl();
        cout << rtl.version << "\n";
        exit(0);
    }

    // Is the user asking for the RTL version?
    if (cmd == "rtl-version")
    {   
        rtl = fpga.getRtl();
        cout << rtl.version << "\n";
        exit(0);
    }

    // Is the user asking for the RTL date?
    if (cmd == "rtl-date")
    {   
        rtl = fpga.getRtl();
        cout << rtl.date << "\n";
        exit(0);
    }

    // Is the user asking for the RTL time?
    if (cmd == "rtl-time")
    {   
        rtl = fpga.getRtl();
        cout << rtl.time << "\n";
        exit(0);
    }

    // Is the user asking for the RTL time?
    if (cmd == "rtl-time")
    {   
        rtl = fpga.getRtl();
        cout << rtl.time << "\n";
        exit(0);
    }

    // Is the user asking for the RTL type?
    if (cmd == "rtl-type")
    {   
        rtl = fpga.getRtl();
        cout << rtl.type << "\n";
        exit(0);
    }

    // Is the user asking for the RTL repo hash?
    if (cmd == "rtl-hash")
    {   
        rtl = fpga.getRtl();
        cout << rtl.hash << "\n";
        exit(0);
    }

    // Is the user asking for the state of the GPIO DIP switches?
    if (cmd == "gpio-switches" || cmd == "gpio-dips")
    {   
        value = fpga.getSwitches();
        cout << value << "\n";
        exit(0);
    }

    // Is the user setting the GPIO LEDs?
    if (cmd == "gpio-leds")
    {   
        // Fetch a numeric value
        value = getu32(c++);

        // And drive the LEDS to the specified value
        fpga.setLeds(value);
        exit(0);
    }

    // Is the user setting the direction of the pins on PX0?
    if (cmd == "px0-iodir")
    {
        // Fetch a numeric value
        value = getu32(c++);

        // Set the directions of the 16 pins on PX0
        fpga.setPx0Iodir(value);
        exit(0);
    }


    // Is the user enabling pullup resistors on PX0?
    if (cmd == "px0-pullup")
    {
        // Fetch a numeric value
        value = getu32(c++);

        // Enable/disable optional pullup resistors on PX0
        fpga.setPx0Pullup(value);
        exit(0);
    }

    // Is the user setting / getting GPIO outputs on PX0?
    if (cmd == "px0-gpio")
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



    // If we get here, we didn't recognize the command
    cerr << "syntax-error\n";
    exit(1);
}
//=============================================================================



