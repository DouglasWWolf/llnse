//=============================================================================
// This manages a UDP socket for sending messages to a particular IP address
// and port number
//=============================================================================
#include <unistd.h>
#include <cstdarg>
#include <cstring>
#include <string>
#include <iostream>
#include <mutex>
#include "udp_msg_sender.h"

using namespace UdpSocket;


//=============================================================================
// from_ai() - Fill in a addrinfo_t structure from a 'struct addrinfo'
//=============================================================================
void addrinfo_t::from_ai(addrinfo& ai)
{
    // Save the IP address, port, family, etc
    addr = *(sockaddr_storage*)ai.ai_addr;

    // Save the length of m_addr
    addrlen = ai.ai_addrlen;

    // Save the address family (AF_INET or AF_INET6)
    family = ai.ai_family;

    // Save the socket type (SOCK_DGRAM or SOCK_STREAM)
    socktype = ai.ai_socktype;

    // Save the protocol
    protocol = ai.ai_protocol;
}
//=============================================================================



//=============================================================================
// get_server_addrinfo() - Returns connection information for a remote server
//=============================================================================
static bool get_server_addrinfo(int type, std::string server, int port,
                                int family, addrinfo_t* p_result)
{
    char ascii_port[20];
    struct addrinfo hints, *p_res;

    // If we fail, our entire return structure will be zero
    memset(p_result, 0, sizeof(addrinfo));

    // Get an ASCII version of the port number
    sprintf(ascii_port, "%i", port);

    // We're going to build an IPv4/IPv6 TCP socket
    memset(&hints, 0, sizeof hints);
    hints.ai_family   = family;
    hints.ai_socktype = type;
   
    // Get information about this server.  If we can't, it doesn't exist
    int rc = getaddrinfo(server.c_str(), ascii_port, &hints, &p_res);
    if (rc != 0) return false;
   
    // If we didn't get a result from getaddrinfo, something's wrong
    if (p_res == NULL) return false;

    // Save a copy of the results
    *p_result = *p_res;

    // Free the memory that was allocated by getaddrinfo
    freeaddrinfo(p_res);

    // Tell the caller that we have information about the server
    return true;
}
//=============================================================================




//=============================================================================
// throwRuntime() - Throws a runtime exception
//=============================================================================
static void throwRuntime(const char* fmt, ...)
{
    char buffer[1024];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buffer, sizeof buffer, fmt, ap);
    va_end(ap);

    throw std::runtime_error(buffer);
}
//=============================================================================



//=============================================================================
// create() - Creates a UDP socket that points to a particular target IP 
//            address and port number
//=============================================================================
void Sender::create(std::string ip, uint32_t port)
{
    // If the sender already points at this IP/port, do nothing
    if (ip == ip_ && port == port_) return;

    // If the socket needs to be closed, close it
    if (sd_ >= 0)
    {
        close(sd_);
        sd_ = -1;
    }


    // Create the addrinfo structure for sending UDP messages
    if (!get_server_addrinfo(SOCK_DGRAM, ip.c_str(), port,  AF_INET, &remote_))
    {
        throwRuntime("UDPSender::create - Failed to create addrinfo structure");
    }

    // Create the UDP sender socket
    sd_ = socket(remote_.family, remote_.socktype, remote_.protocol);

    // If that failed, tell the caller
    if (sd_ < 0)
    {
        throwRuntime("UDPSender::create - Failed to create UDP socket");
    }

    // And keep track of what IP address and port we're pointed at
    ip_   = ip;
    port_ = port;
}
//=============================================================================


//=============================================================================
// Sends a UDP message
//
// This is thread-safe
//=============================================================================
void Sender::send(const char* msg, uint32_t length)
{
    static std::mutex mtx;
    
    // If we don't have an open UDP socket, do nothing
    if (sd_ == -1) return;

    mtx.lock();
    ::sendto(sd_, msg, length, 0, remote_, remote_.addrlen);
    mtx.unlock();
}
//=============================================================================

