#include <fstream>
#include <cstdint>
#include <format>
#include <iostream>

using std::string;

    // MEMORY MAP
    // ---------------------------------------------------------------------------
    // 0x0000 - 0x07FF --> 2 KB internal ram
    // 0x0800 - 0x0FFF --> mirror of 0x0000-0x07FF
    // 0x1000 - 0x17FF --> mirror of 0x0000-0x07FF
    // 0x1800 - 0x1FFF --> mirror of 0x0000-0x07FF
    // 0x2000 - 0x2007 --> NES PPU registers
    // 0x2008 - 0x3FFF --> mirrors of 0x2000-0x2007 (repeated every 8 bytes)
    // 0x4000 - 0x4017 --> NES APU and I/O registers
    // 0x4018 - 0x401F --> APU and I/O functionality that is normally disabled
    // 0x4020 - 0xFFFF --> unmapped. avaliable for cartridge use
    // |.....0x6000 - 0x7FFF --> usually cartridge ram, when present
    // |.....0x8000 - 0xFFFF --> usually cartridge rom and mapper registers

class CpuMemory
{
    public:
        // constructors
        CpuMemory();
        // size of the memory (0xFFFF + 0x1)
        static constexpr int MEMORY_SIZE = 0x10000;
        // reads data from a 16-bit address
        uint8_t read(uint16_t address) const;
        // writes data to a 16-bit address
        void write(uint16_t address, uint8_t value);
        // dumps the contents of the memory map
        void memoryMapDump() const;
    private:
        // memory allocation stored in an array
        uint8_t memory[MEMORY_SIZE];
        // determines if an address is valid within memory address space
        bool validateAddressRange(uint16_t address) const;
};