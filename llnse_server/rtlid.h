#pragma once
#include <stdint.h>
#include "mmio.h"

class CRtlId : public CMMIO
{
public:
    
    // Default constructor
    CRtlId() : CMMIO() {};

    // Map the module to an AXI address
    void  map(const std::string& name, uint32_t size = 0x1000);

    // Fetch the RTL version as a string
    void  getVersion(char* buffer);

    // Fetch the build date as a string
    void  getDate(char* buffer);

    // Fetch the buid time as a string
    void  getTime(char* buffer);

    // Fetchy the git-hash as a string
    void  getHash(char* buffer);

    // Fetch the pair of integers that defines what RTL this is
    uint32_t getRtlType();
    uint32_t getRtlSubtype();
};
