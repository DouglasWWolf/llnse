#pragma once
#include <mutex>
#include "llnseApi.h"
#include "llnse_messages.h"

namespace llnse {

//=============================================================================
// Implementation of the "Connection" class
//=============================================================================
class Connection::Impl
{
public:

    // Call this to connect to the server
    void    connect(std::string fifodir, uint8_t channel);    

    // Call this to perform a remote procedure call on the server
    void    rpc(llnse::base_msg_t& req, llnse::base_msg_t& rsp);

protected:        

    // File descriptor - Client-Out-Server-In
    int     cosi_fd_;
    
    // File descriptor - Client-In-Server-Out
    int     ciso_fd_;

    // We synchronize remote procedure calls via this mutex
    std::mutex mutex_;
};
//=============================================================================


} /* end of namespace */

