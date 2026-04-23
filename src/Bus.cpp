#include "Bus.h"

// namespaces
namespace
{
    void writeHeaderToFile(std::ofstream& file, string header, string range)
    {
        file << "************************************************\n";
        file << header << std::endl;
        file << range << std::endl;
        file << "************************************************\n";
        file << format("{:<20}{}\n", "Address", "Memory");
        file << format("{:<20}{}\n", "-------", "-------");
    }
    
    void writeMemoryToFile(std::ofstream& file, const Bus& bus, int start, int end)
    {
        for(int i = start ; i <= end ; i++)
        {
            file << format("{:<20}", format("({:05d}) {:#06x}", i, i))
                 << std::hex
                 << static_cast<int>(bus.cpuRead(i))
                 << "\n"
                 << std::dec;
        }
    }
}

// public
Bus::Bus()
    : Bus(true)
{
}

Bus::Bus(bool memoryMapOn)
    : memory{}
{
    this->memoryMapOn = memoryMapOn;
}

uint8_t Bus::cpuRead(uint16_t address, BusActivity* activity = nullptr) const
{
    if(validateAddressRange(address))
    {   
        if(activity)
        {
            activity->address = address;
            activity->data = memory[address];
            activity->action = "Read";
        }
        if(memoryMapOn)
        {
            // 2 KB internal ram & mirrors
            if(address < 0x2000)
            {
                return memory[address & 0x07FF];
            }
            // NES PPU registers & mirrors
            else if(address < 0x4000)
            {
                return memory[address & 0x0007];
            }
            // NES APU & I/O Registers, cartridge ram, cartridge rom and mapper registers
            else
            {
                return memory[address];
            }
        }
        else
        {
            return memory[address];
        }
    }
    else
    {
        std::cout << format("Error reading memory at address: {}\n", address);
        return -1;
    }
}

void Bus::cpuWrite(uint16_t address, uint8_t value, BusActivity* activity = nullptr)
{
    if(validateAddressRange(address))
    {
        if(activity)
        {
            activity->address = address;
            activity->data = value;
            activity->action = "Write";
        }
        if(memoryMapOn)
        {
            // 2 KB internal ram & mirrors
            if(address < 0x2000)
            {
                memory[address & 0x07FF] = value;
            }
            // NES PPU registers & mirrors
            else if(address < 0x4000)
            {
                memory[address & 0x0007] = value;
            }
            // NES APU & I/O Registers, cartridge ram, cartridge rom and mapper registers
            else
            {
                memory[address] = value;
            }
        }
        else
        {
            memory[address] = value;
        }
    }
    else
    {
        std::cout << format("Error writing to memory: {} @ address: {}\n", value, address);
        memory[address] = -1;
    }
}

void Bus::memoryMapDump(string path = "../debug/memoryMap.txt") const
{
    std::ofstream file(path);
    
    // 0x0000 - 0x07FF --> 2 KB internal ram
    writeHeaderToFile(file, "2KB INTERNAL RAM", "0x0000 - 0x07FF");
    writeMemoryToFile(file, *this, 0x0000, 0x07FF);
    
    // 0x0800 - 0x0FFF --> mirror of 0x0000-0x07FF
    writeHeaderToFile(file, "2KB INTERNAL RAM (MIRROR 1)", "0x0800 - 0x0FFF");
    writeMemoryToFile(file, *this, 0x0800, 0x0FFF);
    
    // 0x1000 - 0x17FF --> mirror of 0x0000-0x07FF
    writeHeaderToFile(file, "2KB INTERNAL RAM (MIRROR 2)", "0x1000 - 0x17FF");
    writeMemoryToFile(file, *this, 0x1000, 0x17FF);
    
    // 0x1800 - 0x1FFF --> mirror of 0x0000-0x07FF
    writeHeaderToFile(file, "2KB INTERNAL RAM (MIRROR 3)", "0x1800 - 0x1FFF");
    writeMemoryToFile(file, *this, 0x1800, 0x1FFF);
    
    // 0x2000 - 0x2007 --> NES PPU registers
    writeHeaderToFile(file, "PPU REGISTERS", "0x2000 - 0x2007");
    writeMemoryToFile(file, *this, 0x2000, 0x2007);
    
    // 0x2008 - 0x3FFF --> mirrors of 0x2000-0x2007 (repeated every 8 bytes)
    writeHeaderToFile(file, "MIRRORS OF PPU REGISTERS (REPEATED EVERY 8 BYTES)", "0x2008 - 0x3FFF");
    writeMemoryToFile(file, *this, 0x2008, 0x3FFF);
    
    // 0x4000 - 0x4017 --> NES APU and I/O registers
    writeHeaderToFile(file, "APU & I/O REGISTERS", "0x4000 - 0x4017");
    writeMemoryToFile(file, *this, 0x4000, 0x4017);
    
    // 0x4018 - 0x401F --> APU and I/O functionality that is normally disabled
    writeHeaderToFile(file, "APU & I/O FUNCTIONALITY (DISABLED)", "0x4018 - 0x401F");
    writeMemoryToFile(file, *this, 0x4018, 0x401F);
    
    // 0x4020 - 0xFFFF --> unmapped. avaliable for cartridge use
    // 0x4020 - 0x5FFF
    writeHeaderToFile(file, "UNMAPPED (AVAILABLE FOR CARTRIDGE USE)", "0x4020 - 0xFFFF");
    writeMemoryToFile(file, *this, 0x4020, 0x5FFF);
    
    // 0x6000 - 0x7FFF --> usually cartridge ram, when present
    writeHeaderToFile(file, "CARTRIDGE RAM", "0x6000 - 0x7FFF");
    writeMemoryToFile(file, *this, 0x6000, 0x7FFF);
    
    // 0x8000 - 0xFFFF --> usually cartridge rom and mapper registers
    writeHeaderToFile(file, "CARTRIDGE ROM & MAPPER REGISTERS", "0x8000 - 0xFFFF");
    writeMemoryToFile(file, *this, 0x8000, 0xFFFF);
}

// private
bool Bus::validateAddressRange(uint16_t address) const
{
    return (address >= 0x0000 && address < MEMORY_SIZE);
}