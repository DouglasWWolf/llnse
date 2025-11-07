#include "rtlid.h"

const uint32_t REG_MAJOR       = 0;
const uint32_t REG_MINOR       = 1;
const uint32_t REG_BUILD       = 2;
const uint32_t REG_RCAND       = 3;
const uint32_t REG_DATE        = 4;
const uint32_t REG_RTL_TYPE    = 5;
const uint32_t REG_RTL_SUBTYPE = 6;
const uint32_t REG_TIME        = 7;
const uint32_t REG_HASH_0      = 16;  // There are five of these consecutively
const uint32_t REG_HASH_1      = 17; 
const uint32_t REG_HASH_2      = 18; 
const uint32_t REG_HASH_3      = 19; 
const uint32_t REG_HASH_4      = 20; 


#define reg ((uint32_t*)iobase_)

//=============================================================================
// This is just the underlying call to the base-class's "map" function, with
// the addition that if we're running on desktop Linux, we fill in the relevant
// registers with reasonable example values.   We do this so we can develop
// and debug via normal Linux desktop.
//=============================================================================
void CRtlId::map(const std::string& name, uint32_t size)
{
    // Call the base-class function
    CMMIO::map(name, size);

    // Simulate some values for when we're debugging on a desktop PC
    #ifndef __aarch64__
        reg[REG_MAJOR   ] = 1;
        reg[REG_MINOR   ] = 2;
        reg[REG_BUILD   ] = 3;
        reg[REG_RTL_TYPE] = 11125;
        reg[REG_DATE    ] = (11 << 24) | (4 << 16) | 2025;
        reg[REG_TIME    ] = (15 << 16) | (40 << 8) | 11;
        reg[REG_HASH_4  ] = 0x01020304;
        reg[REG_HASH_3  ] = 0x05060708;
        reg[REG_HASH_2  ] = 0x090A0B0C;
        reg[REG_HASH_1  ] = 0x0D0E0F10;
        reg[REG_HASH_0  ] = 0x11121314;
    #endif
}
//=============================================================================


//=============================================================================
// This fetches the RTL version as a MAJOR.MINOR.BUILD string
//=============================================================================
void CRtlId::getVersion(char* buffer)
{
    sprintf(buffer, "%i.%i.%i", reg[REG_MAJOR], reg[REG_MINOR], reg[REG_BUILD]);
}
//=============================================================================


//=============================================================================
// This fetches the RTL build date as a MM/DD/YYYY string
//=============================================================================
void CRtlId::getDate(char* buffer)
{
    auto date = reg[REG_DATE];
    int month = (date >> 24) & 0xFF;
    int day   = (date >> 16) & 0xFF;
    int year  = (date      ) & 0xFFFF;
    sprintf(buffer, "%02i/%02i/%04i", month, day, year);
}
//=============================================================================


//=============================================================================
// This fetches the RTL build time as a HH:MM:SS string
//=============================================================================
void CRtlId::getTime(char* buffer)
{
    auto time = reg[REG_TIME];
    int hour = (time >> 16) & 0xFF;
    int min  = (time >>  8) & 0xFF;
    int sec  = (time      ) & 0xFF;
    sprintf(buffer, "%02i:%02i:%02i", hour, min, sec);
}
//=============================================================================


//=============================================================================
// These fetch the pair of integers that define what kind of RTL we are
//=============================================================================
uint32_t CRtlId::getRtlType()    {return reg[REG_RTL_TYPE   ];}
uint32_t CRtlId::getRtlSubtype() {return reg[REG_RTL_SUBTYPE];}
//=============================================================================


//=============================================================================
// This fetches the git hash of the repo the RTL was built from
//=============================================================================
void CRtlId::getHash(char* buffer)
{
    uint32_t* word = &reg[REG_HASH_0];

    for (int i=0; i<5; ++i)
    {
        sprintf(buffer, "%08x", word[i]);
        buffer += 8;
    }
}
//=============================================================================
