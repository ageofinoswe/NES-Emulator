#include "Cpu.h"

// public
Cpu::Cpu()
    : Cpu("bytes.nes")
{
}
Cpu::Cpu(string fileName)
    : instructionSet{}, A{0}, X{0}, Y{0}, PC{0xFFFC}, S{0xFD}, P{0}, C{0}, Z{0}, I{1}, D{0}, B{0}, noCpuEffect{1}, V{0}, N{0}
{
    createInstructionSet();
    loadCartridge(fileName);
    memory.memoryMapDump();
}

// private
void Cpu::createInstructionSet()
{
    // ADC
    instructionSet[0x69] = Instruction{Immediate, None, 2, 2, &Cpu::ADC};
    instructionSet[0x65] = Instruction{ZeroPage, None, 2, 3, &Cpu::ADC};
    instructionSet[0x75] = Instruction{ZeroPage, XIndexed, 2, 4, &Cpu::ADC};
    instructionSet[0x6D] = Instruction{Absolute, None, 3, 4, &Cpu::ADC};
    instructionSet[0x7D] = Instruction{Absolute, XIndexed, 3, 4, &Cpu::ADC};
    instructionSet[0x79] = Instruction{Absolute, YIndexed, 3, 4, &Cpu::ADC};
    instructionSet[0x61] = Instruction{IndexedIndirect, XIndexed, 2, 6, &Cpu::ADC};
    instructionSet[0x71] = Instruction{IndexedIndirect, YIndexed, 2, 5, &Cpu::ADC};

    // AND
    instructionSet[0x29] = Instruction{Immediate, None, 2, 2, &Cpu::AND};
    instructionSet[0x25] = Instruction{ZeroPage, None, 2, 3, &Cpu::AND};
    instructionSet[0x35] = Instruction{ZeroPage, XIndexed, 2, 4, &Cpu::AND};
    instructionSet[0x2D] = Instruction{Absolute, None, 3, 4, &Cpu::AND};
    instructionSet[0x3D] = Instruction{Absolute, XIndexed, 3, 4, &Cpu::AND};
    instructionSet[0x39] = Instruction{Absolute, YIndexed, 3, 4, &Cpu::AND};
    instructionSet[0x21] = Instruction{IndexedIndirect, XIndexed, 2, 6, &Cpu::AND};
    instructionSet[0x31] = Instruction{IndexedIndirect, YIndexed, 2, 5, &Cpu::AND};

    // ASL
    instructionSet[0x0A] = Instruction{Accumulator, None, 1, 2, &Cpu::ASL};
    instructionSet[0x06] = Instruction{ZeroPage, None, 2, 5, &Cpu::ASL};
    instructionSet[0x16] = Instruction{ZeroPage, XIndexed, 2, 6, &Cpu::ASL};
    instructionSet[0x0E] = Instruction{Absolute, None, 3, 6, &Cpu::ASL};
    instructionSet[0x1E] = Instruction{Absolute, XIndexed, 3, 7, &Cpu::ASL}; 

    // BCC
    instructionSet[0x90] = Instruction{Relative, None, 2, 2, &Cpu::BCC};
    
    // BCS
    instructionSet[0xB0] = Instruction{Relative, None, 2, 2, &Cpu::BCS};

    // BEQ
    instructionSet[0xF0] = Instruction{Relative, None, 2, 2, &Cpu::BEQ};

    // BIT
    instructionSet[0x24] = Instruction{ZeroPage, None, 2, 3, &Cpu::BIT};
    instructionSet[0x2C] = Instruction{Absolute, None, 3, 4, &Cpu::BIT};

    // BMI
    instructionSet[0x30] = Instruction{Relative, None, 2, 2, &Cpu::BMI};

    // BNE
    instructionSet[0xD0] = Instruction{Relative, None, 2, 2, &Cpu::BNE};

    // BPL
    instructionSet[0x10] = Instruction{Relative, None, 2, 2, &Cpu::BPL};

    // BRK
    instructionSet[0x00] = Instruction{Implied, None, 1, 7, &Cpu::BRK};
    /* instructionSet[0x00] = Instruction{Immediate, None, 2, 7, &Cpu::BRK}; */

    // BVC
    instructionSet[0x50] = Instruction{Relative, None, 2, 2, &Cpu::BVC};

    // BVS
    instructionSet[0x70] = Instruction{Relative, None, 2, 2, &Cpu::BVS};

    // CLC
    instructionSet[0x18] = Instruction{Implied, None, 1, 2, &Cpu::CLC};

    // CLD
    instructionSet[0xD8] = Instruction{Implied, None, 1, 2, &Cpu::CLD};

    // CLI
    instructionSet[0x58] = Instruction{Implied, None, 1, 2, &Cpu::CLI};

    // CLV
    instructionSet[0xB8] = Instruction{Implied, None, 1, 2, &Cpu::CLV};

    // CMP
    instructionSet[0xC9] = Instruction{Immediate, None, 2, 2, &Cpu::CMP};
    instructionSet[0xC5] = Instruction{ZeroPage, None, 2, 3, &Cpu::CMP};
    instructionSet[0xD5] = Instruction{ZeroPage, XIndexed, 2, 4, &Cpu::CMP};
    instructionSet[0xCD] = Instruction{Absolute, None, 3, 4, &Cpu::CMP};
    instructionSet[0xDD] = Instruction{Absolute, XIndexed, 3, 4, &Cpu::CMP};
    instructionSet[0xD9] = Instruction{Absolute, YIndexed, 3, 4, &Cpu::CMP};
    instructionSet[0xC1] = Instruction{IndexedIndirect, XIndexed, 2, 6, &Cpu::CMP};
    instructionSet[0xD1] = Instruction{IndexedIndirect, YIndexed, 2, 5, &Cpu::CMP};

    // CPX
    instructionSet[0xE0] = Instruction{Immediate, None, 2, 2, &Cpu::CPX};
    instructionSet[0xE4] = Instruction{ZeroPage, None, 2, 3, &Cpu::CPX};
    instructionSet[0xEC] = Instruction{Absolute, None, 3, 4, &Cpu::CPX};

    // CPY
    instructionSet[0xC0] = Instruction{Immediate, None, 2, 2, &Cpu::CPY};
    instructionSet[0xC4] = Instruction{ZeroPage, None, 2, 3, &Cpu::CPY};
    instructionSet[0xCC] = Instruction{Absolute, None, 3, 4, &Cpu::CPY};

    // DEC
    instructionSet[0xC6] = Instruction{ZeroPage, None, 2, 5, &Cpu::DEC};
    instructionSet[0xD6] = Instruction{ZeroPage, XIndexed, 2, 6, &Cpu::DEC};
    instructionSet[0xCE] = Instruction{Absolute, None, 3, 6, &Cpu::DEC};
    instructionSet[0xDE] = Instruction{Absolute, XIndexed, 3, 7, &Cpu::DEC};

    // DEX
    instructionSet[0xCA] = Instruction{Implied, None, 1, 2, &Cpu::DEX};

    // DEY
    instructionSet[0x88] = Instruction{Implied, None, 1, 2, &Cpu::DEY};

    // EOR
    instructionSet[0x49] = Instruction{Immediate, None, 2, 2, &Cpu::EOR};
    instructionSet[0x45] = Instruction{ZeroPage, None, 2, 3, &Cpu::EOR};
    instructionSet[0x55] = Instruction{ZeroPage, XIndexed, 2, 4, &Cpu::EOR};
    instructionSet[0x4D] = Instruction{Absolute, None, 3, 4, &Cpu::EOR};
    instructionSet[0x5D] = Instruction{Absolute, XIndexed, 3, 4, &Cpu::EOR};
    instructionSet[0x59] = Instruction{Absolute, YIndexed, 3, 4, &Cpu::EOR};
    instructionSet[0x41] = Instruction{IndexedIndirect, XIndexed, 2, 6, &Cpu::EOR};
    instructionSet[0x51] = Instruction{IndexedIndirect, YIndexed, 2, 5, &Cpu::EOR};

    // INC
    instructionSet[0xE6] = Instruction{ZeroPage, None, 2, 5, &Cpu::INC};
    instructionSet[0xF6] = Instruction{ZeroPage, XIndexed, 2, 6, &Cpu::INC};
    instructionSet[0xEE] = Instruction{Absolute, None, 3, 6, &Cpu::INC};
    instructionSet[0xFE] = Instruction{Absolute, XIndexed, 3, 7, &Cpu::INC};

    // INX
    instructionSet[0xE8] = Instruction{Implied, None, 1, 2, &Cpu::INX};

    // INY
    instructionSet[0xC8] = Instruction{Implied, None, 1, 2, &Cpu::INY};

    // JMP
    instructionSet[0x4C] = Instruction{Absolute, None, 3, 3, &Cpu::JMP};
    instructionSet[0x6C] = Instruction{Indirect, None, 3, 5, &Cpu::JMP};

    // JSR
    instructionSet[0x20] = Instruction{Absolute, None, 3, 6, &Cpu::JSR};

    // LDA
    instructionSet[0xA9] = Instruction{Immediate, None, 2, 2, &Cpu::LDA};
    instructionSet[0xA5] = Instruction{ZeroPage, None, 2, 3, &Cpu::LDA};
    instructionSet[0xB5] = Instruction{ZeroPage, XIndexed, 2, 4, &Cpu::LDA};
    instructionSet[0xAD] = Instruction{Absolute, None, 3, 4, &Cpu::LDA};
    instructionSet[0xBD] = Instruction{Absolute, XIndexed, 3, 4, &Cpu::LDA};
    instructionSet[0xB9] = Instruction{Absolute, YIndexed, 3, 4, &Cpu::LDA};
    instructionSet[0xA1] = Instruction{IndexedIndirect, XIndexed, 2, 6, &Cpu::LDA};
    instructionSet[0xB1] = Instruction{IndexedIndirect, YIndexed, 2, 5, &Cpu::LDA};

    // LDX
    instructionSet[0xA2] = Instruction{Immediate, None, 2, 2, &Cpu::LDX};
    instructionSet[0xA6] = Instruction{ZeroPage, None, 2, 3, &Cpu::LDX};
    instructionSet[0xB6] = Instruction{ZeroPage, YIndexed, 2, 4, &Cpu::LDX};
    instructionSet[0xAE] = Instruction{Absolute, None, 3, 4, &Cpu::LDX};
    instructionSet[0xBE] = Instruction{Absolute, YIndexed, 3, 4, &Cpu::LDX};

    // LDY
    instructionSet[0xA0] = Instruction{Immediate, None, 2, 2, &Cpu::LDY};
    instructionSet[0xA4] = Instruction{ZeroPage, None, 2, 3, &Cpu::LDY};
    instructionSet[0xB4] = Instruction{ZeroPage, XIndexed, 2, 4, &Cpu::LDY};
    instructionSet[0xAC] = Instruction{Absolute, None, 3, 4, &Cpu::LDY};
    instructionSet[0xBC] = Instruction{Absolute, XIndexed, 3, 4, &Cpu::LDY};

    // LSR
    instructionSet[0x4A] = Instruction{Accumulator, None, 1, 2, &Cpu::LSR};
    instructionSet[0x46] = Instruction{ZeroPage, None, 2, 5, &Cpu::LSR};
    instructionSet[0x56] = Instruction{ZeroPage, XIndexed, 2, 6, &Cpu::LSR};
    instructionSet[0x4E] = Instruction{Absolute, None, 3, 6, &Cpu::LSR};
    instructionSet[0x5E] = Instruction{Absolute, XIndexed, 3, 7, &Cpu::LSR};

    // NOP
    instructionSet[0xEA] = Instruction{Implied, None, 1, 2, &Cpu::NOP};

    // ORA
    instructionSet[0x09] = Instruction{Immediate, None, 2, 2, &Cpu::ORA};
    instructionSet[0x05] = Instruction{ZeroPage, None, 2, 3, &Cpu::ORA};
    instructionSet[0x15] = Instruction{ZeroPage, XIndexed, 2, 4, &Cpu::ORA};
    instructionSet[0x0D] = Instruction{Absolute, None, 3, 4, &Cpu::ORA};
    instructionSet[0x1D] = Instruction{Absolute, XIndexed, 3, 4, &Cpu::ORA};
    instructionSet[0x19] = Instruction{Absolute, YIndexed, 3, 4, &Cpu::ORA};
    instructionSet[0x01] = Instruction{IndexedIndirect, XIndexed, 2, 6, &Cpu::ORA};
    instructionSet[0x11] = Instruction{IndexedIndirect, YIndexed, 2, 5, &Cpu::ORA};

    // PHA
    instructionSet[0x48] = Instruction{Implied, None, 1, 3, &Cpu::PHA};

    // PHP
    instructionSet[0x08] = Instruction{Implied, None, 1, 3, &Cpu::PHP};

    // PLA
    instructionSet[0x68] = Instruction{Implied, None, 1, 4, &Cpu::PLA};

    // PLP
    instructionSet[0x28] = Instruction{Implied, None, 1, 4, &Cpu::PLP};

    // ROL
    instructionSet[0x2A] = Instruction{Accumulator, None, 1, 2, &Cpu::ROL};
    instructionSet[0x26] = Instruction{ZeroPage, None, 2, 5, &Cpu::ROL};
    instructionSet[0x36] = Instruction{ZeroPage, XIndexed, 2, 6, &Cpu::ROL};
    instructionSet[0x2E] = Instruction{Absolute, None, 3, 6, &Cpu::ROL};
    instructionSet[0x3E] = Instruction{Absolute, XIndexed, 3, 7, &Cpu::ROL};

    // ROR
    instructionSet[0x6A] = Instruction{Accumulator, None, 1, 2, &Cpu::ROR};
    instructionSet[0x66] = Instruction{ZeroPage, None, 2, 5, &Cpu::ROR};
    instructionSet[0x76] = Instruction{ZeroPage, XIndexed, 2, 6, &Cpu::ROR};
    instructionSet[0x6E] = Instruction{Absolute, None, 3, 6, &Cpu::ROR};
    instructionSet[0x7E] = Instruction{Absolute, XIndexed, 3, 7, &Cpu::ROR};

    // RTI
    instructionSet[0x40] = Instruction{Implied, None, 1, 6, &Cpu::RTI};

    // RTS
    instructionSet[0x60] = Instruction{Implied, None, 1, 6, &Cpu::RTS};

    // SBC
    instructionSet[0xE9] = Instruction{Immediate, None, 2, 2, &Cpu::SBC};
    instructionSet[0xE5] = Instruction{ZeroPage, None, 2, 3, &Cpu::SBC};
    instructionSet[0xF5] = Instruction{ZeroPage, XIndexed, 2, 4, &Cpu::SBC};
    instructionSet[0xED] = Instruction{Absolute, None, 3, 4, &Cpu::SBC};
    instructionSet[0xFD] = Instruction{Absolute, XIndexed, 3, 4, &Cpu::SBC};
    instructionSet[0xF9] = Instruction{Absolute, YIndexed, 3, 4, &Cpu::SBC};
    instructionSet[0xE1] = Instruction{IndexedIndirect, XIndexed, 2, 6, &Cpu::SBC};
    instructionSet[0xF1] = Instruction{IndexedIndirect, YIndexed, 2, 5, &Cpu::SBC};

    // SEC
    instructionSet[0x38] = Instruction{Implied, None, 1, 2, &Cpu::SEC};

    // SED
    instructionSet[0xF8] = Instruction{Implied, None, 1, 2, &Cpu::SED};

    // SEI
    instructionSet[0x78] = Instruction{Implied, None, 1, 2, &Cpu::SEI};

    // STA
    instructionSet[0x85] = Instruction{ZeroPage, None, 2, 3, &Cpu::STA};
    instructionSet[0x95] = Instruction{ZeroPage, XIndexed, 2, 4, &Cpu::STA};
    instructionSet[0x8D] = Instruction{Absolute, None, 3, 4, &Cpu::STA};
    instructionSet[0x9D] = Instruction{Absolute, XIndexed, 3, 5, &Cpu::STA};
    instructionSet[0x99] = Instruction{Absolute, YIndexed, 3, 5, &Cpu::STA};
    instructionSet[0x81] = Instruction{IndexedIndirect, XIndexed, 2, 6, &Cpu::STA};
    instructionSet[0x91] = Instruction{IndexedIndirect, XIndexed, 2, 6, &Cpu::STA};

    // STX
    instructionSet[0x86] = Instruction{ZeroPage, None, 2, 3, &Cpu::STX};
    instructionSet[0x96] = Instruction{ZeroPage, YIndexed, 2, 4, &Cpu::STX};
    instructionSet[0x8E] = Instruction{Absolute, None, 3, 4, &Cpu::STX};

    // STY
    instructionSet[0x84] = Instruction{ZeroPage, None, 2, 3, &Cpu::STY};
    instructionSet[0x94] = Instruction{ZeroPage, XIndexed, 2, 4, &Cpu::STY};
    instructionSet[0x8C] = Instruction{Absolute, None, 3, 4, &Cpu::STY};

    // TAX
    instructionSet[0xAA] = Instruction{Implied, None, 1, 2, &Cpu::TAX};

    // TAY
    instructionSet[0xA8] = Instruction{Implied, None, 1, 2, &Cpu::TAY};

    // TSX
    instructionSet[0xBA] = Instruction{Implied, None, 1, 2, &Cpu::TSX};

    // TXA
    instructionSet[0x8A] = Instruction{Implied, None, 1, 2, &Cpu::TXA};

    // TXS
    instructionSet[0x9A] = Instruction{Implied, None, 1, 2, &Cpu::TXS};

    // TYA
    instructionSet[0x98] = Instruction{Implied, None, 1, 2, &Cpu::TYA};
}

void Cpu::powerUpStateInitializer()
{
    A = 0;
    X = 0;
    Y = 0;
    PC = 0xFFFC;
    S = 0xFD; // 0x00 - 3
    P = 0;
    C = 0;
    Z = 0;
    I = 1;
    D = 0;
    B = 0;
    noCpuEffect = 1;
    V = 0;
    N = 0;
}

void Cpu::loadCartridge(string fileName)
{
    std::ifstream file(fileName, std::ios::binary);
    char byte;
    int start = 0x0;
    int end = CpuMemory::MEMORY_SIZE;
    while(file.read(&byte, 1) && start < end)
    {
        memory.write(start, byte);
        start++;
    }
    file.close();
}

void Cpu::setOperand(AddressingModes mode, AddressingModeModifiers modifier, uint16_t address)
{
    switch (mode)
    {
        case Accumulator:
            switch (modifier)
            {
                case None:
                break;

                case XIndexed:
                case YIndexed:
                default: std::cout << "Invalid Accumulator Addressing Mode!" << std::endl;
                break;
            }
        break;

        case Immediate:
            switch (modifier)
            {
                case None:
                break;

                case XIndexed:
                case YIndexed:
                default: std::cout << "Invalid Immediate Addressing Mode!" << std::endl;
                break;
            }
        break;

        case ZeroPage:
            switch (modifier)
            {
                case None:
                {
                    operand = memory.read(address);
                }
                break;

                case XIndexed:
                {
                    operand = memory.read((address + X) % 256);
                }
                break;

                case YIndexed:
                {
                    operand = memory.read((address + Y) % 256);
                }
                break;

                default: std::cout << "Invalid Zero Page Addressing Mode!" << std::endl;
                break;
            }
        break;

        case Absolute:
            switch (modifier)
            {
                case None:
                {
                    operand = memory.read(address);
                }
                break;

                case XIndexed:
                {
                    operand = memory.read(address + X); 
                }
                break;

                case YIndexed:
                {
                    operand = memory.read(address + Y);
                }
                break;

                default: std::cout << "Invalid Absolute Addressing Mode!" << std::endl;
                break;
            }
        break;

        case Relative:
            switch (modifier)
            {
                case None:
                {
                    operand = memory.read(PC + address);
                }
                break;

                case XIndexed:
                case YIndexed:
                default: std::cout << "Invalid Relative Addressing Mode!" << std::endl;
                break;
            }
        break;

        case Indirect:
            switch (modifier)
            {
                case None:
                {
                    operand = memory.read(memory.read(address));
                }
                break;

                case XIndexed:
                case YIndexed:
                default: std::cout << "Invalid Indirect Addressing Mode!" << std::endl;
                break;
            }
        break;

        case IndexedIndirect:
            switch (modifier)
            {
                case None: std::cout << "Invalid Indexed/Indirect Addressing Mode!" << std::endl;
                break;

                case XIndexed:
                {
                    const uint16_t k1 = (address + X) % 256;
                    const uint16_t k2 = (address + X + 1) % 256;
                    operand = memory.read(memory.read(k1) + (memory.read(k2) << 8));
                }
                break;

                case YIndexed:
                {
                    const uint16_t k1 = (address + 1) % 256;
                    operand = memory.read(memory.read(address) + (memory.read(k1) << 8) + Y);
                }
                break;

                default: std::cout << "Invalid Indexed/Indirect Addressing Mode!" << std::endl;
                break;
            }
        break;

        case Implied:
            switch (modifier)
            {
                case None:
                break;

                case XIndexed:
                case YIndexed:
                default: std::cout << "Invalid Implied Addressing Mode!" << std::endl;   
                break;
            }
        break;

        default: std::cout << "Invalid Addressing Mode!" << std::endl;
        break;
    }
}

void Cpu::cycle()
{
    uint8_t opCode = fetch();
    const Instruction& instruction = decode(opCode);
    execute(instruction);
}

uint8_t Cpu::fetch()
{
    return memory.read(PC++);
}

const Instruction& Cpu::decode(uint8_t opCode)
{
    const Instruction& instruction = instructionSet[opCode];
    uint16_t address = 0;
    uint8_t numBytes = instruction.numBytes;
    for(int bytes = 1 ; bytes < numBytes ; bytes++)
    {
        address |= memory.read(PC++) << (8 * (bytes - 1));
    }
    setOperand(instruction.mode, instruction.modifier, address);
    return instruction;
}

void Cpu::execute(const Instruction& instruction)
{
    (this->*(instruction.instruction))();
}


void Cpu::ADC()
{
    CLC();
    int temp = A + operand + C;
    C = (temp > 0xFF);
    Z = (temp == 0);
    V = (temp ^ A) & (temp ^ operand) & 0x80;
    N = (temp >> 7) & 0x1;
    A = (temp & 0xFF);
}
void Cpu::AND()
{

}
void Cpu::ASL()
{

}
void Cpu::BCC()
{

}
void Cpu::BCS()
{

}
void Cpu::BEQ()
{

}
void Cpu::BIT()
{

}
void Cpu::BMI()
{

}
void Cpu::BNE()
{

}
void Cpu::BPL()
{

}
void Cpu::BRK()
{

}
void Cpu::BVC()
{

}
void Cpu::BVS()
{

}
void Cpu::CLC()
{
    C = 0;
}
void Cpu::CLD()
{

}
void Cpu::CLI()
{

}
void Cpu::CLV()
{

}
void Cpu::CMP()
{

}
void Cpu::CPX()
{

}
void Cpu::CPY()
{

}
void Cpu::DEC()
{

}
void Cpu::DEX()
{

}
void Cpu::DEY()
{

}
void Cpu::EOR()
{

}
void Cpu::INC()
{

}
void Cpu::INX()
{

}
void Cpu::INY()
{

}
void Cpu::JMP()
{

}
void Cpu::JSR()
{

}
void Cpu::LDA()
{

}
void Cpu::LDX()
{

}
void Cpu::LDY()
{

}
void Cpu::LSR()
{

}
void Cpu::NOP()
{

}
void Cpu::ORA()
{

}
void Cpu::PHA()
{

}
void Cpu::PHP()
{

}
void Cpu::PLA()
{

}
void Cpu::PLP()
{

}
void Cpu::ROL()
{

}
void Cpu::ROR()
{

}
void Cpu::RTI()
{

}
void Cpu::RTS()
{

}
void Cpu::SBC()
{

}
void Cpu::SEC()
{

}
void Cpu::SED()
{

}
void Cpu::SEI()
{

}
void Cpu::STA()
{

}
void Cpu::STX()
{

}
void Cpu::STY()
{

}
void Cpu::TAX()
{

}
void Cpu::TAY()
{

}
void Cpu::TSX()
{

}
void Cpu::TXA()
{

}
void Cpu::TXS()
{

}
void Cpu::TYA()
{

}