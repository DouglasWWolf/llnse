#include <unistd.h>
#include <stdio.h>
#include "logger.h"

log::CLogger logger;

int main(int argc, const char** argv)
{
    logger.init("demo");

    logger.log(log::FATAL, __FILE__, __LINE__, "Six times seven is %i", 42);

    exit(0);
}



