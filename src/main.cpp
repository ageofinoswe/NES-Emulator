#include "Cpu.h"

int main()
{
    // instantiate a cpu and bus without memory mapping
    Bus bus(false);
    Cpu cpu(bus);

    // run SST (instruction, opCode)
    cpu.runTest("ADC", "6d");
    cpu.runTest("ADC", "7d");
    cpu.runTest("ADC", "61");
    cpu.runTest("ADC", "65");
    cpu.runTest("ADC", "69");
    cpu.runTest("ADC", "71");
    cpu.runTest("ADC", "75");
    cpu.runTest("ADC", "79");

    cpu.runTest("AND", "2d");
    cpu.runTest("AND", "3d");
    cpu.runTest("AND", "21");
    cpu.runTest("AND", "25");
    cpu.runTest("AND", "29");
    cpu.runTest("AND", "31");
    cpu.runTest("AND", "35");
    cpu.runTest("AND", "39");

    return 0;
}