#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "mmio.h"

//=============================================================================
// map() - Maps a block of address space into users-space
//=============================================================================
bool CMMIO::map(uint32_t address, uint32_t size)
{
    const char* filename = "/dev/mem";

    // Memory protection flags for mmap()
    const int protection = PROT_READ | PROT_WRITE;

    // If we're in simulation mode, just allocated the RAM
    #ifndef __aarch64__
        if (ptr) free(ptr);
        ptr = malloc(size);
        return true;
    #endif

    // If we already have /dev/mem open, close it!
    if (fd_ != -1)
    {
        close(fd_);
        fd_ = -1;
        ptr = nullptr;
    }

    // Open the /dev/mem device
    fd_ = ::open(filename, O_RDWR| O_SYNC);

    // If that failed, we're done here
    if (fd_ < 0) return false;

    // Map the memory
    ptr = ::mmap(0, size, protection, MAP_SHARED, fd_, address);

    // If a mapping error occurs, we're done
    if (ptr == MAP_FAILED)
    {
        ptr = nullptr;
        close(fd_);
        fd_ = -1;
        return false;
    }

    // Tell the caller that the memory is mapped
    return true;
}
//=============================================================================