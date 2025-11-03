#pragma once
#include <stdint.h>
#include <string>
#include <memory>

namespace llnse
{

    //=========================================================================
    // We have our own version of a runtime error
    //=========================================================================
    class runtime_error : public std::runtime_error
    {
    public:
        runtime_error
        (
            const std::string& file,
            const uint32_t     line,
            const std::string& what_arg
        ) :
        std::runtime_error(what_arg), file_(file), line_(line)  {};

        std::string file_;
        uint32_t line_;
    };
    //=========================================================================


    //=========================================================================
    // Public API to an llnse "Connection"
    //=========================================================================
    class Connection
    {   
    
    public:
        // Call this to connect to the llnse server
        void connect(const std::string fifodir, uint8_t channel);

    // Has to be public because other classes access this
    public:
        class Impl;
        std::unique_ptr<Impl> p_impl;

    public:
        Connection(); 
        ~Connection();

        // Objects of this class can't be copied;
        Connection(const Connection&) = delete;
        Connection& operator=(const Connection&) = delete;
    };
    //=========================================================================

}
