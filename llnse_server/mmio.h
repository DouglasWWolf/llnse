#pragma once
#include <stdint.h>
#include <map>
#include <string>

class CMMIO
{
public:

    CMMIO()     {fd_ = -1; iobase_ = nullptr;}
    static void readMemoryMap(const std::string& filename);
    void        map(uint32_t address,        uint32_t size = 0x1000);
    void        map(const std::string& name, uint32_t size = 0x1000);

protected:
    static std::map<std::string, uint32_t> memmap_;
    int fd_;
    void*   iobase_;
};
