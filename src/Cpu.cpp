#include "Cpu.h"

const bool DEBUG_MODE = true; 

// public
Cpu::Cpu(Bus& bus)
    :   bus{bus}, cyclesRequired{0}, currentCycle{0}, lowByte{0}, highByte{0}, readMode{true}, writeMode{!readMode},
        A{0}, X{0}, Y{0}, PC{0xFFFC}, S{0xFD}, P{4}
{
    if(DEBUG_MODE)
    {   
        if(std::filesystem::exists("../debug/cpuState.txt"))
        {
            std::filesystem::remove("../debug/cpuState.txt");
        }
    }
    createInstructionSet();
}

Cpu::Cpu(Bus& bus, string fileName)
    :   bus{bus}, cyclesRequired{0}, currentCycle{0}, lowByte{0}, highByte{0}, readMode{true}, writeMode{!readMode},
        A{0}, X{0}, Y{0}, PC{0xFFFC}, S{0xFD}, P{4}
{
    if(DEBUG_MODE)
    {   
        if(std::filesystem::exists("../debug/cpuState.txt"))
        {
            std::filesystem::remove("../debug/cpuState.txt");
        }
    }
    createInstructionSet();
    loadCartridge(fileName);
}

uint8_t Cpu::readTestMem(uint16_t address)
{
    return read(address);
}

void Cpu::writeTestMem(uint16_t address, uint8_t value)
{
    write(address, value);
}

uint16_t Cpu::getRegisterValue(string reg)
{
    if(reg.compare("A") == 0)
    {
        return A;
    }
    else if(reg.compare("X") == 0)
    {
        return X;
    }
    else if(reg.compare("Y") == 0)
    {
        return Y;
    }
    else if(reg.compare("PC") == 0)
    {
        return PC;
    }
    else if(reg.compare("S") == 0)
    {
        return S;
    }
    else if(reg.compare("P") == 0)
    {
        return P;
    }
    else
    {
        return -1;
        std::cout << "Invalid register access!\n";
    }   
}


void Cpu::setTestCpuState(string reg, uint16_t value)
{
    if(reg.compare("A") == 0)
    {
        A = value;
    }
    else if(reg.compare("X") == 0)
    {
        X = value;
    }
    else if(reg.compare("Y") == 0)
    {
        Y = value;
    }
    else if(reg.compare("PC") == 0)
    {
        PC = value;
    }
    else if(reg.compare("S") == 0)
    {
        S = value;
    }
    else if(reg.compare("P") == 0)
    {
        P = value;
    }
    else
    {
        std::cout << "Invalid state set in CPU!\n";
    }
}

void Cpu::runCycle()
{    
    currentCycle++;

    if(currentCycle == 1)
    {
        fetchOpCode();
    }
    else if(currentCycle <= cyclesRequired)
    {
        (this->*currentInstruction.cycles)();
    }
    if(currentCycle == cyclesRequired)
    {
        (this->*currentInstruction.execute)();
    }
}

void Cpu::autoRunCycles()
{
    while(currentCycle <= cyclesRequired)
    {
        runCycle();
    }
}

// private
void Cpu::createInstructionSet()
{
    // ADC
    instructionSet[ADC_IMM] = Instruction{OpCode{"ADC - Immediate", ADC_IMM, 2, 2, IMMEDIATE}, &Cpu::IMM, &Cpu::ADC};

    instructionSet[ADC_ZP] = Instruction{OpCode{"ADC - Zero Page", ADC_ZP, 2, 3, ZEROPAGE}, &Cpu::ZP, &Cpu::ADC};
    instructionSet[ADC_ZPX] = Instruction{OpCode{"ADC - Zero Page, X", ADC_ZPX, 2, 4, ZEROPAGE_X}, &Cpu::ZPX, &Cpu::ADC};

    instructionSet[ADC_ABS] = Instruction{OpCode{"ADC - Absolute", ADC_ABS, 3, 4, ABSOLUTE}, &Cpu::ABS, &Cpu::ADC};
    instructionSet[ADC_ABSX] = Instruction{OpCode{"ADC - Absolute, X", ADC_ABSX, 3, 4, ABSOLUTE_X}, &Cpu::ABX, &Cpu::ADC};
    instructionSet[ADC_ABSY] = Instruction{OpCode{"ADC - Absolute, Y", ADC_ABSY, 3, 4, ABSOLUTE_Y}, &Cpu::ABY, &Cpu::ADC};

    instructionSet[ADC_IIX] = Instruction{OpCode{"ADC - Indexed Indirect, X", ADC_IIX, 2, 6, INDEXED_INDIRECT_X}, &Cpu::IIX, &Cpu::ADC};
    instructionSet[ADC_IIY] = Instruction{OpCode{"ADC - Indirect Indexed, Y", ADC_IIY, 2, 5, INDIRECT_INDEXED_Y}, &Cpu::IIY, &Cpu::ADC};
/* 
    // AND
    instructionSet[AND_IMM] = Instruction{2, 2, &Cpu::AND};
    instructionSet[AND_ZP] = Instruction{2, 3, &Cpu::AND};
    instructionSet[AND_ZPX] = Instruction{2, 4, &Cpu::AND};
    instructionSet[AND_ABS] = Instruction{3, 4, &Cpu::AND};
    instructionSet[AND_ABSX] = Instruction{3, 4, &Cpu::AND};
    instructionSet[AND_ABSY] = Instruction{3, 4, &Cpu::AND};
    instructionSet[AND_IIX] = Instruction{2, 6, &Cpu::AND};
    instructionSet[AND_IIY] = Instruction{2, 5, &Cpu::AND};

    // ASL
    instructionSet[ASL_ACC] = Instruction{1, 2, &Cpu::ASL};
    instructionSet[ASL_ZP] = Instruction{2, 5, &Cpu::ASL};
    instructionSet[ASL_ZPX] = Instruction{2, 6, &Cpu::ASL};
    instructionSet[ASL_ABS] = Instruction{3, 6, &Cpu::ASL};
    instructionSet[ASL_ABSX] = Instruction{3, 7, &Cpu::ASL}; 

    // BCC
    instructionSet[BCC_REL] = Instruction{2, 2, &Cpu::BCC};
    
    // BCS
    instructionSet[BCS_REL] = Instruction{2, 2, &Cpu::BCS};

    // BEQ
    instructionSet[BEQ_REL] = Instruction{2, 2, &Cpu::BEQ};

    // BIT
    instructionSet[BIT_ZP] = Instruction{2, 3, &Cpu::BIT};
    instructionSet[BIT_ABS] = Instruction{3, 4, &Cpu::BIT};

    // BMI
    instructionSet[BMI_REL] = Instruction{2, 2, &Cpu::BMI};

    // BNE
    instructionSet[BNE_REL] = Instruction{2, 2, &Cpu::BNE};

    // BPL
    instructionSet[BPL_REL] = Instruction{2, 2, &Cpu::BPL};

    // BRK
    instructionSet[BRK_IMPL] = Instruction{1, 7, &Cpu::BRK};
    // instructionSet[BRK_IMM] = Instruction{2, 7, &Cpu::BRK};

    // BVC
    instructionSet[BVC_REL] = Instruction{2, 2, &Cpu::BVC};

    // BVS
    instructionSet[BVS_REL] = Instruction{2, 2, &Cpu::BVS};

    // CLC
    instructionSet[CLC_IMPL] = Instruction{1, 2, &Cpu::CLC};

    // CLD
    instructionSet[CLD_IMPL] = Instruction{1, 2, &Cpu::CLD};

    // CLI
    instructionSet[CLI_IMPL] = Instruction{1, 2, &Cpu::CLI};

    // CLV
    instructionSet[CLV_IMPL] = Instruction{1, 2, &Cpu::CLV};

    // CMP
    instructionSet[CMP_IMM] = Instruction{2, 2, &Cpu::CMP};
    instructionSet[CMP_ZP] = Instruction{2, 3, &Cpu::CMP};
    instructionSet[CMP_ZPX] = Instruction{2, 4, &Cpu::CMP};
    instructionSet[CMP_ABS] = Instruction{3, 4, &Cpu::CMP};
    instructionSet[CMP_ABSX] = Instruction{3, 4, &Cpu::CMP};
    instructionSet[CMP_ABSY] = Instruction{3, 4, &Cpu::CMP};
    instructionSet[CMP_IIX] = Instruction{2, 6, &Cpu::CMP};
    instructionSet[CMP_IIY] = Instruction{2, 5, &Cpu::CMP};

    // CPX
    instructionSet[CPX_IMM] = Instruction{2, 2, &Cpu::CPX};
    instructionSet[CPX_ZP] = Instruction{2, 3, &Cpu::CPX};
    instructionSet[CPX_ABS] = Instruction{3, 4, &Cpu::CPX};

    // CPY
    instructionSet[CPY_IMM] = Instruction{2, 2, &Cpu::CPY};
    instructionSet[CPY_ZP] = Instruction{2, 3, &Cpu::CPY};
    instructionSet[CPY_ABS] = Instruction{3, 4, &Cpu::CPY};

    // DEC
    instructionSet[DEC_ZP] = Instruction{2, 5, &Cpu::DEC};
    instructionSet[DEC_ZPX] = Instruction{2, 6, &Cpu::DEC};
    instructionSet[DEC_ABS] = Instruction{3, 6, &Cpu::DEC};
    instructionSet[DEC_ABSX] = Instruction{3, 7, &Cpu::DEC};

    // DEX
    instructionSet[DEX_IMPL] = Instruction{1, 2, &Cpu::DEX};

    // DEY
    instructionSet[DEY_IMPL] = Instruction{1, 2, &Cpu::DEY};

    // EOR
    instructionSet[EOR_IMM] = Instruction{2, 2, &Cpu::EOR};
    instructionSet[EOR_ZP] = Instruction{2, 3, &Cpu::EOR};
    instructionSet[EOR_ZPX] = Instruction{2, 4, &Cpu::EOR};
    instructionSet[EOR_ABS] = Instruction{3, 4, &Cpu::EOR};
    instructionSet[EOR_ABSX] = Instruction{3, 4, &Cpu::EOR};
    instructionSet[EOR_ABSY] = Instruction{3, 4, &Cpu::EOR};
    instructionSet[EOR_IIX] = Instruction{2, 6, &Cpu::EOR};
    instructionSet[EOR_IIY] = Instruction{2, 5, &Cpu::EOR};

    // INC
    instructionSet[INC_ZP] = Instruction{2, 5, &Cpu::INC};
    instructionSet[INC_ZPX] = Instruction{2, 6, &Cpu::INC};
    instructionSet[INC_ABS] = Instruction{3, 6, &Cpu::INC};
    instructionSet[INC_ABSX] = Instruction{3, 7, &Cpu::INC};

    // INX
    instructionSet[INX_IMPL] = Instruction{1, 2, &Cpu::INX};

    // INY
    instructionSet[INY_IMPL] = Instruction{1, 2, &Cpu::INY};

    // JMP
    instructionSet[JMP_ABS] = Instruction{3, 3, &Cpu::JMP};
    instructionSet[JMP_IND] = Instruction{3, 5, &Cpu::JMP};

    // JSR
    instructionSet[JSR_ABS] = Instruction{3, 6, &Cpu::JSR};

    // LDA
    instructionSet[LDA_IMM] = Instruction{2, 2, &Cpu::LDA};
    instructionSet[LDA_ZP] = Instruction{2, 3, &Cpu::LDA};
    instructionSet[LDA_ZPX] = Instruction{2, 4, &Cpu::LDA};
    instructionSet[LDA_ABS] = Instruction{3, 4, &Cpu::LDA};
    instructionSet[LDA_ABSX] = Instruction{3, 4, &Cpu::LDA};
    instructionSet[LDA_ABSY] = Instruction{3, 4, &Cpu::LDA};
    instructionSet[LDA_IIX] = Instruction{2, 6, &Cpu::LDA};
    instructionSet[LDA_IIY] = Instruction{2, 5, &Cpu::LDA};

    // LDX
    instructionSet[LDX_IMM] = Instruction{2, 2, &Cpu::LDX};
    instructionSet[LDX_ZP] = Instruction{2, 3, &Cpu::LDX};
    instructionSet[LDX_ZPY] = Instruction{2, 4, &Cpu::LDX};
    instructionSet[LDX_ABS] = Instruction{3, 4, &Cpu::LDX};
    instructionSet[LDX_ABSY] = Instruction{3, 4, &Cpu::LDX};

    // LDY
    instructionSet[LDY_IMM] = Instruction{2, 2, &Cpu::LDY};
    instructionSet[LDY_ZP] = Instruction{2, 3, &Cpu::LDY};
    instructionSet[LDY_ZPX] = Instruction{2, 4, &Cpu::LDY};
    instructionSet[LDY_ABS] = Instruction{3, 4, &Cpu::LDY};
    instructionSet[LDY_ABSX] = Instruction{3, 4, &Cpu::LDY};

    // LSR
    instructionSet[LSR_ACC] = Instruction{1, 2, &Cpu::LSR};
    instructionSet[LSR_ZP] = Instruction{2, 5, &Cpu::LSR};
    instructionSet[LSR_ZPX] = Instruction{2, 6, &Cpu::LSR};
    instructionSet[LSR_ABS] = Instruction{3, 6, &Cpu::LSR};
    instructionSet[LSR_ABSX] = Instruction{3, 7, &Cpu::LSR};

    // NOP
    instructionSet[NOP_IMPL] = Instruction{1, 2, &Cpu::NOP};

    // ORA
    instructionSet[ORA_IMM] = Instruction{2, 2, &Cpu::ORA};
    instructionSet[ORA_ZP] = Instruction{2, 3, &Cpu::ORA};
    instructionSet[ORA_ZPX] = Instruction{2, 4, &Cpu::ORA};
    instructionSet[ORA_ABS] = Instruction{3, 4, &Cpu::ORA};
    instructionSet[ORA_ABSX] = Instruction{3, 4, &Cpu::ORA};
    instructionSet[ORA_ABSY] = Instruction{3, 4, &Cpu::ORA};
    instructionSet[ORA_IIX] = Instruction{2, 6, &Cpu::ORA};
    instructionSet[ORA_IIY] = Instruction{2, 5, &Cpu::ORA};

    // PHA
    instructionSet[PHA_IMPL] = Instruction{1, 3, &Cpu::PHA};

    // PHP
    instructionSet[PHP_IMPL] = Instruction{1, 3, &Cpu::PHP};

    // PLA
    instructionSet[PLA_IMPL] = Instruction{1, 4, &Cpu::PLA};

    // PLP
    instructionSet[PLP_IMPL] = Instruction{1, 4, &Cpu::PLP};

    // ROL
    instructionSet[ROL_ACC] = Instruction{1, 2, &Cpu::ROL};
    instructionSet[ROL_ZP] = Instruction{2, 5, &Cpu::ROL};
    instructionSet[ROL_ZPX] = Instruction{2, 6, &Cpu::ROL};
    instructionSet[ROL_ABS] = Instruction{3, 6, &Cpu::ROL};
    instructionSet[ROL_ABSX] = Instruction{3, 7, &Cpu::ROL};

    // ROR
    instructionSet[ROR_ACC] = Instruction{1, 2, &Cpu::ROR};
    instructionSet[ROR_ZP] = Instruction{2, 5, &Cpu::ROR};
    instructionSet[ROR_ZPX] = Instruction{2, 6, &Cpu::ROR};
    instructionSet[ROR_ABS] = Instruction{3, 6, &Cpu::ROR};
    instructionSet[ROR_ABSX] = Instruction{3, 7, &Cpu::ROR};

    // RTI
    instructionSet[RTI_IMPL] = Instruction{1, 6, &Cpu::RTI};

    // RTS
    instructionSet[RTS_IMPL] = Instruction{1, 6, &Cpu::RTS};

    // SBC
    instructionSet[SBC_IMM] = Instruction{2, 2, &Cpu::SBC};
    instructionSet[SBC_ZP] = Instruction{2, 3, &Cpu::SBC};
    instructionSet[SBC_ZPX] = Instruction{2, 4, &Cpu::SBC};
    instructionSet[SBC_ABS] = Instruction{3, 4, &Cpu::SBC};
    instructionSet[SBC_ABSX] = Instruction{3, 4, &Cpu::SBC};
    instructionSet[SBC_ABSY] = Instruction{3, 4, &Cpu::SBC};
    instructionSet[SBC_IIX] = Instruction{2, 6, &Cpu::SBC};
    instructionSet[SBC_IIY] = Instruction{2, 5, &Cpu::SBC};

    // SEC
    instructionSet[SEC_IMPL] = Instruction{1, 2, &Cpu::SEC};

    // SED
    instructionSet[SED_IMPL] = Instruction{1, 2, &Cpu::SED};

    // SEI
    instructionSet[SEI_IMPL] = Instruction{1, 2, &Cpu::SEI};

    // STA
    instructionSet[STA_ZP] = Instruction{2, 3, &Cpu::STA};
    instructionSet[STA_ZPX] = Instruction{2, 4, &Cpu::STA};
    instructionSet[STA_ABS] = Instruction{3, 4, &Cpu::STA};
    instructionSet[STA_ABSX] = Instruction{3, 5, &Cpu::STA};
    instructionSet[STA_ABSY] = Instruction{3, 5, &Cpu::STA};
    instructionSet[STA_IIX] = Instruction{2, 6, &Cpu::STA};
    instructionSet[STA_IIY] = Instruction{2, 6, &Cpu::STA};

    // STX
    instructionSet[STX_ZP] = Instruction{2, 3, &Cpu::STX};
    instructionSet[STX_ZPY] = Instruction{2, 4, &Cpu::STX};
    instructionSet[STX_ABS] = Instruction{3, 4, &Cpu::STX};

    // STY
    instructionSet[STY_ZP] = Instruction{2, 3, &Cpu::STY};
    instructionSet[STY_ZPX] = Instruction{2, 4, &Cpu::STY};
    instructionSet[STY_ABS] = Instruction{3, 4, &Cpu::STY};

    // TAX
    instructionSet[TAX_IMPL] = Instruction{1, 2, &Cpu::TAX};

    // TAY
    instructionSet[TAY_IMPL] = Instruction{1, 2, &Cpu::TAY};

    // TSX
    instructionSet[TSX_IMPL] = Instruction{1, 2, &Cpu::TSX};

    // TXA
    instructionSet[TXA_IMPL] = Instruction{1, 2, &Cpu::TXA};

    // TXS
    instructionSet[TXS_IMPL] = Instruction{1, 2, &Cpu::TXS};

    // TYA
    instructionSet[TYA_IMPL] = Instruction{1, 2, &Cpu::TYA}; */
}

uint8_t Cpu::read(uint16_t address)
{
    return bus.cpuRead(address);
}

void Cpu::write(uint16_t address, uint8_t value)
{
    bus.cpuWrite(address, value);
}

void Cpu::fetchOpCode()
{
    uint8_t opCode = bus.cpuRead(PC++);
    currentInstruction = instructionSet[opCode];
    cyclesRequired = currentInstruction.opCode.minCycles;
}

void Cpu::fetchLowByte()
{
    lowByte = bus.cpuRead(PC++);
}

void Cpu::fetchHighByte()
{
    highByte = bus.cpuRead(PC++);
}

void Cpu::setReadMode(bool isOn)
{
    readMode = isOn;
    writeMode = !isOn;
}

void Cpu::setWriteMode(bool isOn)
{
    writeMode = isOn;
    readMode = !isOn;
}

void Cpu::loadCartridge(string fileName)
{
    std::ifstream file(fileName, std::ios::binary);
    char byte;
    int start = 0x0;
    int end = Bus::MEMORY_SIZE;
    while(file.read(&byte, 1) && start < end)
    {
        bus.cpuWrite(start, byte);
        start++;
    }
    file.close();
}

void Cpu::powerUpStateInitializer()
{
    A = 0;
    X = 0;
    Y = 0;
    PC = 0xFFFC;
    S = 0xFD; // 0x00 - 3
    setPFlag(C, 0);
    setPFlag(Z, 0);
    setPFlag(I, 1);
    setPFlag(D, 0);
    setPFlag(V, 0);
    setPFlag(N, 0);
}

void Cpu::setPFlag(PFlags flag, bool val)
{
    P = (P & ~(1 << (7-flag))) | (val << (7-flag));
}

bool Cpu::getPFlag(PFlags flag)
{
    return (P >> (7-flag)) & 0x1; 
}

void Cpu::IMP()
{
    switch(currentCycle)
    {
        case 2:
        {
            read(PC);
        }
        break;
        
        default: std::cout << "Invalid IMP Instruction Cycle\n";
        break;
    }
}

void Cpu::IMM()
{
    switch(currentCycle)
    {  
        case 2:
        {
            fetchLowByte();
            data = lowByte;
        }
        break;

        default: std::cout << "Invalid IMM Instruction Cycle\n";
        break;
    }
}

void Cpu::ACC()
{
    switch(currentCycle)
    {
        case 2:
        {
            read(PC);
        }
        break;
        
        default: std::cout << "Invalid ACC Instruction Cycle\n";
        break;
    }
}

void Cpu::REL()
{

}

void Cpu::ZP()
{
    switch(currentCycle)
    {
        case 2:
        {
            fetchLowByte();
            address = lowByte;
        }
        case 3:
        {
            data = read(address);
        }
        break;

        default: std::cout << "Invalid ZP Instruction Cycle\n";
        break;
    }    
}

void Cpu::ZPX()
{
    switch(currentCycle)
    {
        case 2:
        {
            fetchLowByte();
            address = lowByte;
        }
        break;

        case 3: 
        {
            read(address);
            address = (lowByte + X) % 256;
        }
        break;

        case 4:
        {
            data = read(address);
        }
        break;

        default: std::cout << "Invalid ZPX Instruction Cycle\n";
        break;
    }
}

void Cpu::ZPY()
{
    switch(currentCycle)
    {
        case 2:
        {
            fetchLowByte();
            address = lowByte;
        }
        break;

        case 3: 
        {
            read(address);
            address = (lowByte + Y) % 256;
        }
        break;

        case 4:
        {
            data = read(address);
        }
        break;

        default: std::cout << "Invalid ZPY Instruction Cycle\n";
        break;
    }
}

void Cpu::ABS()
{
    switch(currentCycle)
    {   
        case 2:
        {
            fetchLowByte();
            address = PC;
        }
        break;

        case 3: 
        {
            fetchHighByte();
            address = (highByte << 8) | lowByte;
        }
        break;

        case 4:
        {
            data = read(address);
        }
        break;
    
        default: std::cout << "Invalid ABS Instruction Cycle\n";
        break;
    }
}

void Cpu::ABX()
{
    switch(currentCycle)
    {      
        case 2:
        {
            fetchLowByte();
            address = PC;
        }
        break;

        case 3:
        {
            fetchHighByte();
            if(lowByte + X > 0xFF)
            {
                cyclesRequired++;
            }
            address = (highByte << 8) | ((lowByte + X) & 0x00FF);
        }
        break;

        case 4:
        {
            data = read(address);
            if(lowByte + X > 0xFF)
            {
                address = ((highByte + 1) << 8) | ((lowByte + X) & 0x00FF);
            }
        }
        break;

        case 5:
            data = read(address);
        break;

        default: "Invalid ABX Instruction Cycle\n";
        break;
    }
}

void Cpu::ABY()
{
    switch(currentCycle)
    {      
        case 2:
        {
            fetchLowByte();
            address = PC;
        }
        break;

        case 3:
        {
            fetchHighByte();
            if(lowByte + Y > 0xFF)
            {
                cyclesRequired++;
            }
            address = (highByte << 8) | ((lowByte + Y) & 0x00FF);
        }
        break;

        case 4:
        {
            data = read(address);
            if(lowByte + Y > 0xFF)
            {
                address = ((highByte + 1) << 8) | ((lowByte + Y) & 0x00FF);
            }
        }
        break;

        case 5:
            data = read(address);
        break;

        default: "Invalid ABY Instruction Cycle\n";
        break;
    }
}

void Cpu::IND()
{
    switch(currentCycle)
    {
        case 2:
        {
            fetchLowByte();
            address = PC;
        }
        break;

        case 3:
        {
            fetchHighByte();
            address = (highByte << 8) | lowByte;
        }
        break;

        case 4:
        {
            data = read(address);
            highByte = (address & 0xFF00) & 0xFF;
            lowByte = (address & 0xFF) + 1;
            address = (highByte << 8) | lowByte;
        }
        break;
    
        case 5:
        {
            address = (read(address) << 8) | data;
        }
    }
}

void Cpu::IIX()
{
    switch(currentCycle)
    {    
        case 2:
        {
            fetchLowByte();
            address = lowByte;
        }
        break;

        case 3:
        {
            read(address);
            address = (address + X) % 256;
        }
        break;

        case 4:
        {
            lowByte = read(address);
            address = (address + 1) & 0x00FF;
        }
        break;

        case 5:
            highByte = read(address);
            address = (highByte << 8) | lowByte;
        break;

        case 6:
            data = read(address);
        break;

        default: "Invalid IIX Instruction Cycle\n";
        break;
    }
}

void Cpu::IIY()
{
    switch(currentCycle)
    {      
        case 2:
        {
            fetchLowByte();
            address = lowByte;
        }
        break;

        case 3:
        {
            lowByte = read(address);
            address = (address + 1) % 256;
        }
        break;

        case 4:
        {
            highByte = read(address);
            if(lowByte + Y > 0xFF)
            {
                cyclesRequired++;
            }
            address = (highByte << 8) | ((lowByte + Y) & 0xFF);
        }
        break;

        case 5:
            data = read(address);
            if(lowByte + Y > 0xFF)
            {
                address = ((highByte + 1) << 8) | ((lowByte + Y) & 0xFF);
            }
        break;

        case 6:
            data = read(address);
        break;
        
        default: "Invalid IIY Instruction Cycle\n";
        break;
    }
}

void Cpu::ADC()
{
    int result = A + data + getPFlag(C);
    setPFlag(C, result > 0xFF);
    setPFlag(Z, (result & 0x00FF) == 0);
    setPFlag(V, ((result ^ A) & (result ^ data) & 0x80));
    setPFlag(N, ((result >> 7) & 0x1));
    A = result & 0xFF;
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
    setPFlag(C, 0);
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

void Cpu::printState()
{
    std::ofstream file("../debug/cpuState.txt", std::ios::app);

    file << "***************************************************************************\n";
    file << "*OpCode / Addressing Mode: " << ((currentInstruction.opCode.name == nullptr) ? "OpCode not Found" : currentInstruction.opCode.name)
         << std::hex << " (0x" << static_cast<int>(currentInstruction.opCode.opCodeValue) << ")" << std::dec << "\n";
    file << "*Number of Bytes: " << static_cast<int>(currentInstruction.opCode.numBytes) << "\n";
    file << "*Cycles Elapsed: " << static_cast<int>(currentCycle) << "\n";
    file << "*Cycles Required: " << static_cast<int>(cyclesRequired) << "\n";
    file << "***************************************************************************\n";
    file << "----------------\n";
    file << "|STATE\n";
    file << "----------------\n";
    file << std::dec;
    file << "|Address: " << static_cast<int>(address) << "\n";
    file << "|Data: " << static_cast<int>(data) << "\n";
    file << "|Low Byte: " << static_cast<int>(lowByte) << "\n";
    file << "|High Byte: " << static_cast<int>(highByte) << "\n";
    file << "|A: " << static_cast<int>(A) << "\n";
    file << "|X: " << static_cast<int>(X) << "\n";
    file << "|Y: " << static_cast<int>(Y) << "\n";
    file << "|PC: " << static_cast<int>(PC) << "\n";
    file << "|S: " << static_cast<int>(S) << "\n";
    file << "|P: " << static_cast<int>(P) << "\n";
    file << "|P(N): " << static_cast<int>(getPFlag(N)) << "\n";
    file << "|P(V): " << static_cast<int>(getPFlag(V)) << "\n";
    file << "|P(1): " << static_cast<int>(getPFlag(TRUE)) << "\n";
    file << "|P(B): " << static_cast<int>(getPFlag(B)) << "\n";
    file << "|P(D): " << static_cast<int>(getPFlag(D)) << "\n";
    file << "|P(I): " << static_cast<int>(getPFlag(I)) << "\n";
    file << "|P(Z): " << static_cast<int>(getPFlag(Z)) << "\n";
    file << "|P(C): " << static_cast<int>(getPFlag(C)) << "\n\n";
    file << std::dec;

    file.close();
}            