#pragma once
#include <string>
#include <iostream>
#include "gpioAccessApi.h"
using std::string;
using std::cout, std::cerr;

string   getKeyword(int index);
uint32_t getu32(int index);
uint32_t touint32(const string& s);

extern gpioAccess::CGpioAccess fpga;