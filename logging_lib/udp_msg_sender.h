#pragma once
#include <stdint.h>
#include <string>
#include <netdb.h>


namespace UdpSocket {

// This is the equivalent of a 'struct addrinfo', but with no pointers
struct addrinfo_t
{
    addrinfo_t& operator=(addrinfo& rhs) {from_ai(rhs); return *this;}
    operator sockaddr*() const {return (sockaddr*)&addr;}
    void      from_ai(addrinfo& rhs);
    sockaddr_storage addr;
    socklen_t        addrlen;
    int              family;
    int              socktype;
    int              protocol;
};

class Sender
{
public:

    // Default constructor
    Sender() {sd_=-1; port_=0;}
    
    // Create or recreate a UDP sender socket
    void    create(std::string ip, uint32_t port);
    
    // Call this to send a message
    void    send(const char* msg, uint32_t length);

protected:

    uint32_t    port_;
    std::string ip_;
    addrinfo_t  remote_;
    uint32_t    sd_;
};

} // End of namespace