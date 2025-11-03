#pragma once
#include <stdint.h>
#include "mmio.h"
class CGPIO
{
public:

    void        map(uint32_t address);
    void        write(uint32_t value);
    uint32_t    read();

protected:

    CMMIO   mmio_;
  
};
