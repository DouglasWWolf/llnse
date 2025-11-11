#include <unistd.h>
#include <stdio.h>
#include "logger.h"

log::CLogger logger;

int main(int argc, const char** argv)
{
    logger.init("demo");


    for (int i=0; i<100; ++i)
    {
        logger.log(log::DEBUG, __FILE__, __LINE__, "Log Message #%i", i);        
    }

    logger.log(log::FATAL, __FILE__, __LINE__, "Six times seven is %i", 42);

    exit(0);
}



