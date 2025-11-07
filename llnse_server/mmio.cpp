#include <unistd.h>
#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "mmio.h"


void throwRuntime(const char* fmt, ...);

std::map<std::string, uint32_t> CMMIO::memmap_;

//=============================================================================
// asciiAddr() - Converts a 32-bit address into a displayable hex ASCII string
//=============================================================================
std::string asciiAddr(uint32_t addr)
{
    char buffer[64];
    sprintf(buffer, "0x%04X_%04X", (addr >> 16) & 0xFFFF, addr & 0xFFFF);
    return buffer;
}
//=============================================================================


//=============================================================================
// map() - Maps a block of address space into users-space
//=============================================================================
void CMMIO::map(const std::string name, uint32_t size)
{
     auto it = memmap_.find(name);

     // If we can't find the name, this is fatal
     if (it == memmap_.end())
     {
        throwRuntime("Unknown module name %s", name.c_str());
     }

     // Fetch the AXI address of this module
     uint32_t address = it->second;
    
     // Tell the world what we're doing.
     std::cout << "Mapping " << name << " to " << asciiAddr(address) << "\n";

     // Map the named RTL module to its address
     map(address, size);
}
//=============================================================================



//=============================================================================
// map() - Maps a block of address space into users-space
//=============================================================================
void CMMIO::map(uint32_t address, uint32_t size)
{
    const char* filename = "/dev/mem";

    // Memory protection flags for mmap()
    const int protection = PROT_READ | PROT_WRITE;

    // If we're in simulation mode, just allocated the RAM
    #ifndef __aarch64__
        if (iobase_) free(iobase_);
        iobase_ = malloc(size);
        return;
    #endif

    // If we already have /dev/mem open, close it!
    if (fd_ != -1)
    {
        close(fd_);
        fd_ = -1;
        iobase_ = nullptr;
    }

    // Open the /dev/mem device
    fd_ = ::open(filename, O_RDWR| O_SYNC);

    // If that failed, we're done here
    if (fd_ < 0) throwRuntime
    (
        "Failed to map MMIO 0x%04X_%04X",
        address >> 16,
        address & 0xFFFF
    );        


    // Map the memory
    iobase_ = ::mmap(0, size, protection, MAP_SHARED, fd_, address);

    // If a mapping error occurs, we're done
    if (iobase_ == MAP_FAILED)
    {
        iobase_ = nullptr;
        close(fd_);
        fd_ = -1;
        throwRuntime
        (
            "Failed to map MMIO 0x%04X_%04X",
            address >> 16,
            address & 0xFFFF
        );        
    }
}
//=============================================================================



//=============================================================================
// readMemoryMap() - A static function to read in the map that translates
//                   a module name into an AXI address
//=============================================================================
void CMMIO::readMemoryMap(const std::string& filename)
{
    char line[256];
    char token[256], *out;

    // Open the input file and complain if we can't
    FILE* ifile = fopen(filename.c_str(), "r");
    if (ifile == nullptr)
    {
        throwRuntime("File not found: %s", filename.c_str());;
    }

    // Clear any existing memory map
    memmap_.clear();

    // Loop through each line of the file
    while (fgets(line, sizeof(line), ifile))    
    {
        // Fetch the address
        const uint32_t address = strtoul(line, nullptr, 0);

        // Search the line for a space
        const char* p = strchr(line, ' ');
        if (p == nullptr) continue;

        // Skip over whitespace
        while (*p == ' ' || *p == '\t') ++p;

        // Now grab the token
        out = token;
        while (*p)
        {
            if (*p == ' ' || *p == '\n') break;
            *out++ = *p++;
        }

        // Nul-terminate the token
        *out = 0;

        // Map this module name to this address
        memmap_[token] = address;
    }

    // Close the input file
    fclose(ifile);

}
//=============================================================================
