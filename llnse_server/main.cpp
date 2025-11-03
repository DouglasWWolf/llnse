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


// Global variables
global_t g;

void execute();

//=============================================================================
// throwRuntime() - Throws a runtime exception
//=============================================================================
void throwRuntime(const char* fmt, ...)
{
    char buffer[1024];
    va_list ap;
    va_start(ap, fmt);
    vsprintf(buffer, fmt, ap);
    va_end(ap);

    throw std::runtime_error(buffer);
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


int main(int argc, const char** argv)
{
    // Don't terminate due to broken pipes!
    signal(SIGPIPE, SIG_IGN);

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
// execute() - Waits for incoming connection attempts and launches threads
//=============================================================================
void execute()
{
    char signal_fifo[256];
    int rc, fd = -1;
    uint8_t channel;

    // Map the RTL registers into userspace
        g.leds.map(0xA4001000);
    g.switches.map(0xA4002000);

    // Create the signal FIFO
    sprintf(signal_fifo, "%s/signal.fifo", g.fifo_folder.c_str());
    rc = mkfifo(signal_fifo, 0666);
    if (rc != 0 && errno != EEXIST)
    {
        throwRuntime("Failed to create %s", signal_fifo);
    }

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

        // Crank up a server thread for this channel
        std::thread th(start_channel, channel);
        th.detach();
    }

}
//=============================================================================
