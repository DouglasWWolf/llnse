#pragma once
#include <stdint.h>

class CMMIO
{
public:

    CMMIO() {fd_ = -1; ptr = nullptr;}
    bool map(uint32_t address, uint32_t size);
    void* ptr;

protected:

    int fd_;
};
