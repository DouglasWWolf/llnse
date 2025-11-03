#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <iostream>
#include "global.h"
#include "nse_channel.h"
#include "llnse_messages.h"

extern global_t g;
using namespace llnse;
void throwRuntime(const char* fmt, ...);

#define MAKE_STRUCTS(x) const x##_req_t& req = *(x##_req_t*)msg_in; \
                        x##_rsp_t& rsp = *(x##_rsp_t*)msg_out;\
                        rsp.msglen  = sizeof(rsp); \
                        rsp.msgtype = req.msgtype



void generate_fault(char* msg_out, uint32_t error)
{
    fault_rsp_t& rsp = *(fault_rsp_t*)msg_out;
    rsp.msglen = sizeof(rsp);
    rsp.msgtype = MSG_FAULT;
    rsp.error = error;
}

void handle_ping(const char* msg_in, char* msg_out)
{
    MAKE_STRUCTS(ping);
    rsp.value = req.value;
}


void handle_fault(const char* msg_in, char* msg_out)
{
    MAKE_STRUCTS(fault);
    rsp.error = req.error;
}

void handle_set_leds(const char* msg_in, char* msg_out)
{
    MAKE_STRUCTS(set_leds);
    g.leds.write(req.value);   
}

void handle_get_switches(const char* msg_in, char* msg_out)
{
    MAKE_STRUCTS(get_switches);
    rsp.value = g.switches.read();
}



//=============================================================================
// start() -  launches a server that waits for messages to arrive on COSI
//            and responds to them on CISO
//=============================================================================
void CNseChannel::start(uint32_t ch)
{
    // Start a message server on the FIFO pair
    try
    {
        fifo_server(ch);
    }
    catch(const std::runtime_error& e)
    {
        std::cerr << e.what() << '\n';
    }
   
}
//=============================================================================



//=============================================================================
// fifo_server() : Opens the two FIFOS, listens for messages on COSI, handles
//                 them, and sends responses on CISO
//=============================================================================
void CNseChannel::fifo_server(uint32_t channel)
{
    char cosi_fn[256], ciso_fn[256];
    char msg_in[llnse::MAX_MSG_LENGTH], msg_out[llnse::MAX_MSG_LENGTH];
    int rc;

    // Map a "base_msg_t" onto the request and response messages
    base_msg_t& req = *(base_msg_t*)msg_in;
    base_msg_t& rsp = *(base_msg_t*)msg_out;

    // This is the "magic number" we'll send to the client immediately
    const uint32_t msg_version = MESSAGE_VERSION;

    // Get a pointer to the folder where our named FIFOs live
    const char* folder = g.fifo_folder.c_str();

    // Build the COSI (Client-Out-Slave-In) FIFO filename
    sprintf(cosi_fn, "%s/ch%i.cosi.fifo", folder, channel);

    // Build the CISO (Client-In-Slave-Out) FIFO filename
    sprintf(ciso_fn, "%s/ch%i.ciso.fifo", folder, channel);

    // Open the input FIFO
    int in_fd = open(cosi_fn, O_RDONLY);
    if (in_fd < 0)
    {
        throwRuntime("Can't open %s!\n", cosi_fn);
    }    

    // Open the output FIFO
    int out_fd = open(ciso_fn, O_WRONLY);
    if (out_fd < 0)
    {
        throwRuntime("Can't open %s!\n", ciso_fn);
    }    

    // Tell the client what message version we're using
    rc = write(out_fd, &msg_version, 4);

    while (true)
    {
        
        // Fetch the message length
        rc = read(in_fd, msg_in, 2);
        if (rc != 2)
        {
            close(in_fd);
            close(out_fd); 
            return;
        }

        // On an invalid message length, we just give up
        if (req.msglen > sizeof(msg_in))
        {
            printf("CH%i: Closing due to invalid length", channel);
            close(in_fd);
            close(out_fd);
            return;            
        }

        // Read in the rest of the message
        rc = read(in_fd, msg_in+2, req.msglen - 2);

        switch(req.msgtype)
        {
            case MSG_FAULT:
                handle_fault(msg_in, msg_out);
                break;

            case MSG_PING:
                handle_ping(msg_in, msg_out);
                break;            

            case MSG_SET_LEDS:
                handle_set_leds(msg_in, msg_out);
                break;

            case MSG_GET_SWITCHES:
                handle_get_switches(msg_in, msg_out);
                break;

            default:
                generate_fault(msg_out, 1);
                break;
        }

        // Send the response message
        rc = write(out_fd, msg_out, rsp.msglen);
    }
}
//=============================================================================