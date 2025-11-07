#pragma once
#include <stdint.h>
#include "mmio.h"
class CGPIO : public CMMIO
{
public:
    
    // Default constructor
    CGPIO() : CMMIO() {};

public:

    void        write(uint32_t value);
    uint32_t    read();

};
