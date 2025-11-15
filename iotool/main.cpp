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

// The handlers for these commands exist in other source files
void cmd_rtl  (int c);
void cmd_px0  (int c);
void cmd_gpio (int c);
void cmd_ps200(int c);

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
// toint32() - Converts a string to a 32-bit integer
//=============================================================================
int32_t toint32(const string& s)
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
    return strtol(buffer, nullptr, 0);
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
    printf("   iotool rtl version\n");
    printf("   iotool rtl date\n");
    printf("   iotool rtl time\n");
    printf("   iotool rtl hash\n");
    printf("   iotool rtl type\n");
    printf("\n");
    printf("   iotool gpio switches\n");
    printf("   iotool gpio leds <value>\n");
    printf("\n");
    printf("   iotool px0 iodir <value>\n");
    printf("   iotool px0 pullup <value>\n");    
    printf("   iotool px0 gpio [new_value]\n");   
    printf("   iotool px0 emulate on <input_bits>\n");     
    printf("   iotool px0 emulate off\n");         
    printf("\n");
    printf("   iotool ps200\n");   
    printf("   iotool ps200 emulate on <signed-int>\n");     
    printf("   iotool ps200 emulate off\n");         

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
// Fetches a keyword as a int32_t
//=============================================================================
int32_t get32(int index)
{
    std::string s = getKeyword(index);
    if (s.empty())
    {
        cerr << "Missing parameter\n";
        exit(1);            
    }
    return toint32(s);
}
//=============================================================================




//=============================================================================
// execute() - Carry out the command from the command line
//=============================================================================
void execute()
{
    int      c=0;
    string   s;

    if (keyword.size() == 0)
    {
        showHelp();
        exit(0);
    }

    // Connect to the llnse server
    conn.connect(fifo_folder, 42);
   
    // The first keyword is our command
    string& cmd = keyword[c++];

    // Execute one of the command groups
    if (cmd == "rtl"  ) cmd_rtl  (1);
    if (cmd == "px0"  ) cmd_px0  (1);
    if (cmd == "gpio" ) cmd_gpio (1);
    if (cmd == "ps200") cmd_ps200(1);
    
    // If we get here, we didn't recognize the command
    cerr << "syntax-error\n";
    exit(1);
}
//=============================================================================



