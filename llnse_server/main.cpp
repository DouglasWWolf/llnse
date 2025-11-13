//=============================================================================
// llnse - Low Level NSE
//=============================================================================
#include <unistd.h>
#include <stdint.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <thread>
#include <iostream>
#include <stdexcept>
#include "nse_channel.h"
#include "global.h"
#include "logger.h"

// "parseCommandLine" will set this is the "-down" switch is used
bool bring_down = false;

// Global variables
global_t g;

void execute();

//=============================================================================
// throwRuntime() - Throws a runtime exception
//=============================================================================
void throwRuntime(const std::string& fmt, ...)
{
    char buffer[1024];
    va_list ap;
    va_start(ap, fmt);
    vsprintf(buffer, fmt.c_str(), ap);
    va_end(ap);

    throw std::runtime_error(buffer);
}
//=============================================================================


//=============================================================================
// Formats a string using printf-style formatting
//=============================================================================
std::string format(const char* fmt, ...)
{
    char buffer[1024];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buffer, sizeof buffer, fmt, ap);
    va_end(ap);
    return buffer;
}
//=============================================================================


//=============================================================================
// start_channel() - Laucher for NseChannel threads
//=============================================================================
void start_channel(uint32_t channel)
{
    CNseChannel obj;
    printf("Starting channel %i\n", channel);
    obj.start(channel);
    printf("Ending channel %i\n", channel);
}
//=============================================================================


//=============================================================================
// Parse the command line parameters.
//
// On output:  bring_down = true if we should bring down the server
//=============================================================================
void parseCommandLine(const char** argv)
{
    int i=1;

    while (argv[i])
    {
        std::string token = argv[i++];

        if (token == "-kill")
        {
            bring_down = true;
            continue;
        }

        std::cout << "Unknown command parameter: " << token << "\n";
        exit(1);
    }
}
//=============================================================================



int main(int argc, const char** argv)
{
    // Don't terminate due to broken pipes!
    signal(SIGPIPE, SIG_IGN);

    // Allow this application to directly set file permissions
    umask(0777);

    // Parse the command line
    parseCommandLine(argv);

    try
    {
        execute();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

}

//=============================================================================
// kill_server() - If another copy of this program is running, calling this
//                 function should cause it to terminate
//=============================================================================
void kill_server()
{
    char signal_fifo[256];
    int  rc, fd;
    const char* X = "X";

    // Have to be root to do this
    if (geteuid() != 0)
    {
        throwRuntime("Must be root user.  Use sudo!");
    }

    // Create the signal FIFO
    sprintf(signal_fifo, "%s/signal.fifo", g.fifo_folder.c_str());
    rc = mkfifo(signal_fifo, 0666);
    if (rc != 0 && errno != EEXIST)
    {
        throwRuntime("Failed to create %s", signal_fifo);
    }

    // Open the signal FIFO for writing
    fd = open(signal_fifo, O_WRONLY);
    if (fd < 0)
    {
        throwRuntime("Failed to open %s", signal_fifo);        
    }

    // Write an uppercase "X" to the signal pipe
    rc = write(fd, X, 1);
    close(fd);
}
//=============================================================================


//=============================================================================
// execute() - Waits for incoming connection attempts and launches threads
//=============================================================================
void execute()
{
    char signal_fifo[256];
    int rc, fd = -1;
    uint8_t channel;

    // Initialize the logger
    g.Logger.init("llnse");

    // If we're just bringing down another copy of the server, make it so
    if (bring_down)
    {
        kill_server();
        exit(0);
    }

    // Build the memory map in RAM
    CMMIO::readMemoryMap(g.memmap);

    // Map the RTL registers into userspace
        g.leds.map("/pl_rtl/axi_gpio_0/S_AXI");
    g.switches.map("/pl_rtl/axi_gpio_1/S_AXI");
       g.rtlid.map("/pl_rtl/axi_revision/S_AXI");
       g.i2c_0.map("/pl_rtl/axi_iic_0/S_AXI");

    // Tell the I2C bus what bus number it is
    g.i2c_0.init(0);

    // Initialize the MCP23017 port expander
    g.px0.init(g.i2c_0, 0x20);

    // Create the signal FIFO
    sprintf(signal_fifo, "%s/signal.fifo", g.fifo_folder.c_str());
    rc = mkfifo(signal_fifo, 0666);
    if (rc != 0 && errno != EEXIST)
    {
        throwRuntime("Failed to create %s", signal_fifo);
    }
    chmod(signal_fifo, 0666);

    // Open the signal FIFO for reading
    fd = open(signal_fifo, O_RDONLY);
    if (fd < 0)
    {
        throwRuntime("Failed to open %s", signal_fifo);        
    }

    // We're going to loop forever, waiting for clients to ask us
    // to open communication channels
    while (true)
    {
        // Open the named FIFO
        if (fd == -1)
        {
            fd = open(signal_fifo, O_RDONLY);
            if (fd < 0) throwRuntime("Failed to open %s", signal_fifo);        
        }

        // Find out which channel number to listen on
        rc = read(fd, &channel, 1);
        if (rc != 1)
        {
            close(fd);
            fd = -1;
            continue;
        }

        // A channe number of decimal 88 means "Drop dead"
        if (channel == 'X')
        {
            fprintf(stderr, "llnse_server has been killed\n");
            exit(0);
        }

        // Crank up a server thread for this channel
        std::thread th(start_channel, channel);
        th.detach();
    }

}
//=============================================================================
