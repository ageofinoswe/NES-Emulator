#ifndef SHAREDTYPES_H_
#define SHAREDTYPES_H_

#include <cstdint>
#include <string>

using std::string;

struct BusActivity
{
    uint16_t address;
    uint8_t data;
    string action;
};

#endif