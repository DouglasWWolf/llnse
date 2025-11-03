#pragma once
#include <stdint.h>


//=============================================================================
//=============================================================================
class CNseChannel
{
public:

    // Start the thread that handles this channel
    void start(uint32_t ch_number);

protected:

    // Runs a message server on the FIFO pair
    void    fifo_server(uint32_t channel);

};
//=============================================================================