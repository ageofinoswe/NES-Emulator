#include "Cpu.h"

// public
Cpu::Cpu()
    : Cpu("bytes.nes")
{
}
Cpu::Cpu(string fileName)
    : instructionSet{}, A{0}, X{0}, Y{0}, PC{0xFFFC}, S{0xFD}, P{0b00000000}
{
    createInstructionSet();
    loadCartridge(fileName);
    memory.memoryMapDump();
}


uint8_t Cpu::fetch()
{
    return memory.read(PC++);
}

void Cpu::runCycle()
{
    // continue instruction
    if(cyclesRemaining > 0)
    {   
        // perform micro-operation (may decrement cyclesRemaining)
        (this->*(currentInstruction.cycles[instructionStep++]))();
        
        if(cyclesRemaining <= 0)
        {
            // all micro-operations have been performed, execute the instruction in the same cycle
            (this->*(currentInstruction.execute))();
        }
    }
    // get next instruction
    else 
    {
        uint8_t opCode = fetch();
        currentInstruction = instructionSet[opCode];
        instructionStep = 0;
        cyclesRemaining = currentInstruction.opCode.minCycles - 1; // 1 cycle consumed by fetch operation
    }
}

void Cpu::fetchLowByte()
{
    if(currentInstruction.opCode.mode == IMMEDIATE)
    {
        data = fetch();
    }
    else
    {
        address = fetch();
    }
    cyclesRemaining--;
    instructionStep++;
}
void Cpu::fetchHighByte()
{
    // increase cycles remaining if there is 8-bit overflow on the low byte, otherwise skip dummy read
    if(currentInstruction.opCode.mode == ABSOLUTE_X)
    {
        if((address + X) > 0xFF)
        {
            cyclesRemaining++;
            address = (address + X) & 0xFF;
        } 
        else
        {
            instructionStep++;
            address = (address + X) & 0xFF;
        }
    }
    else if(currentInstruction.opCode.minCycles == ABSOLUTE_Y)
    {
        if((address + Y) > 0xFF)
        {
            cyclesRemaining++;
            address = (address + Y) & 0xFF;
        } 
        else
        {
            instructionStep++;
            address = (address + Y) & 0xFF;
        }
    }

    address |= fetch() << 8;
    cyclesRemaining--;
    instructionStep++;
}


void Cpu::dummyRead()
{   // address correction
    if(currentInstruction.opCode.mode == ABSOLUTE_X || currentInstruction.opCode.mode == ABSOLUTE_Y)
    {
        data = memory.read(address); // dummy read
        uint8_t highByte = (address & 0xFF00) + 1; // overflow correction
        uint8_t lowByte = (address & 0x00FF);
        address = highByte | lowByte;
    }
    cyclesRemaining--;
    instructionStep++;
}

void Cpu::setPFlag(PFlags flag, bool val)
{
    P = P & (val << (7-flag));
}






// private
void Cpu::createInstructionSet()
{
    // ADC
    instructionSet[ADC_IMM] = Instruction{OpCode{2, 2, IMMEDIATE}, {&Cpu::fetchLowByte}, &Cpu::ADC};

    instructionSet[ADC_ZP] = Instruction{OpCode{2, 3, ZEROPAGE}, {&Cpu::fetchLowByte, &Cpu::read}, &Cpu::ADC};
    instructionSet[ADC_ZPX] = Instruction{OpCode{2, 4, ZEROPAGE_X}, {&Cpu::fetchLowByte, &Cpu::ZPX, &Cpu::read}, &Cpu::ADC};

    instructionSet[ADC_ABS] = Instruction{OpCode{3, 4, ABSOLUTE}, {&Cpu::fetchLowByte, &Cpu::fetchHighByte, &Cpu::read}, &Cpu::ADC};
    instructionSet[ADC_ABSX] = Instruction{OpCode{3, 4, ABSOLUTE_X}, {&Cpu::fetchLowByte, &Cpu::fetchHighByte, &Cpu::dummyRead, &Cpu::read}, &Cpu::ADC};
    instructionSet[ADC_ABSY] = Instruction{OpCode{3, 4, ABSOLUTE_Y}, {&Cpu::fetchLowByte, &Cpu::fetchHighByte, &Cpu::dummyRead, &Cpu::read}, &Cpu::ADC};

    instructionSet[ADC_IIX] = Instruction{OpCode{2, 6, INDEXED_INDIRECT_X}, {&Cpu::fetchLowByte}, &Cpu::ADC};
    instructionSet[ADC_IIY] = Instruction{OpCode{2, 5, INDIRECT_INDEXED_Y}, {}, &Cpu::ADC};

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
    /* instructionSet[BRK_IMM] = Instruction{2, 7, &Cpu::BRK}; */

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
    instructionSet[TYA_IMPL] = Instruction{1, 2, &Cpu::TYA};
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


void Cpu::read()
{
    cyclesRemaining--;
    data = memory.read(address);
}



void Cpu::IMPL(int& cycles)
{
}
void Cpu::IMM(int& cycles)
{
    data = lowByte;
}
void Cpu::ZP(int& cycles)
{
    address = lowByte;
}
void Cpu::ZPX()
{
    cyclesRemaining--;
    address = (address + X) % 256;
}
void Cpu::ZPY(int& cycles)
{
    address = (lowByte + Y) % 256;
}
void Cpu::ABS(int& cycles)
{
}
void Cpu::ABSX(int& cycles)
{
}
void Cpu::ABSY(int& cycles)
{
    address = (highByte << 8 | lowByte) + Y;
}
void Cpu::REL(int& cycles)
{
    address = PC + static_cast<short>(lowByte);
}

void Cpu::IND(int& cycles) // double check this one
{
    address = highByte << 8 | lowByte;
}

void Cpu::IIX(int& cycles)
{
    uint16_t address = highByte << 8 | lowByte;
    uint16_t k1 = (address + X) % 256;
    uint16_t k2 = (address + X + 1) % 256;
    address = memory.read(memory.read(k1) + (memory.read(k2) << 8));
}

void Cpu::IIY(int& cycles)
{
    uint16_t address = highByte << 8 | lowByte;
    uint16_t k1 = (address + 1) % 256;
    address = memory.read(memory.read(address) + (memory.read(k1) << 8) + Y);
}




bool Cpu::getPFlag(PFlags flag)
{
    return (P >> (7-flag)) & 0x1; 
}
void Cpu::ADC()
{
    CLC();
    int result = A + data + getPFlag(C);
    setPFlag(C, result > 0xFF);
    setPFlag(Z, result == 0);
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