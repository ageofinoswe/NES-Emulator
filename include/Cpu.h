#ifndef CPU_H_
#define CPU_H_

#include <filesystem>
#include "Bus.h"

// forward declarations for structs
class Cpu;

// ENUMS
enum AddressingModes
{
    NONE,
    IMPLIED,
    IMMEDIATE,
    ACCUMULATOR,
    RELATIVE,
    ZEROPAGE,
    ZEROPAGE_X,
    ZEROPAGE_Y,
    ABSOLUTE,
    ABSOLUTE_X,
    ABSOLUTE_Y,
    INDIRECT,
    INDEXED_INDIRECT_X,
    INDIRECT_INDEXED_Y
};

enum OpCodes
{
    // ADC
    ADC_IMM = 0x69,
    ADC_ZP = 0x65,
    ADC_ZPX = 0x75,
    ADC_ABS = 0x6D,
    ADC_ABSX = 0x7D,
    ADC_ABSY = 0x79,
    ADC_IIX = 0x61,
    ADC_IIY = 0x71,
    // AND
    AND_IMM = 0x29,
    AND_ZP = 0x25,
    AND_ZPX = 0x35,
    AND_ABS = 0x2D,
    AND_ABSX = 0x3D,
    AND_ABSY = 0x39,
    AND_IIX = 0x21,
    AND_IIY = 0x31,
    // ASL
    ASL_ACC = 0x0A,
    ASL_ZP = 0x06,
    ASL_ZPX = 0x16,
    ASL_ABS = 0x0E,
    ASL_ABSX = 0x1E,
    // BCC
    BCC_REL = 0x90,
    // BCS
    BCS_REL = 0xB0,
    // BEQ
    BEQ_REL = 0xF0,
    // BIT
    BIT_ZP = 0x24,
    BIT_ABS = 0x2C,
    // BMI
    BMI_REL = 0x30,
    // BNE
    BNE_REL = 0xD0,
    // BPL
    BPL_REL = 0x10,
    // BRK
    BRK_IMPL = 0x00,
    /* BRK_IMM = 0x00, */
    // BVC
    BVC_REL = 0x50,
    // BVS
    BVS_REL = 0x70,
    // CLC
    CLC_IMPL = 0x18,
    // CLD
    CLD_IMPL = 0xD8,
    // CLI
    CLI_IMPL = 0x58,
    // CLV
    CLV_IMPL = 0xB8,
    // CMP
    CMP_IMM = 0xC9,
    CMP_ZP = 0xC5,
    CMP_ZPX = 0xD5,
    CMP_ABS = 0xCD,
    CMP_ABSX = 0xDD,
    CMP_ABSY = 0xD9,
    CMP_IIX = 0xC1,
    CMP_IIY = 0xD1,
    // CPX
    CPX_IMM = 0xE0,
    CPX_ZP = 0xE4,
    CPX_ABS = 0xEC,
    // CPY
    CPY_IMM = 0xC0,
    CPY_ZP = 0xC4,
    CPY_ABS = 0xCC,
    // DEC
    DEC_ZP = 0xC6,
    DEC_ZPX = 0xD6,
    DEC_ABS = 0xCE,
    DEC_ABSX = 0xDE,
    // DEX
    DEX_IMPL = 0xCA,
    // DEY
    DEY_IMPL = 0x88,
    // EOR
    EOR_IMM = 0x49,
    EOR_ZP = 0x45,
    EOR_ZPX = 0x55,
    EOR_ABS = 0x4D,
    EOR_ABSX = 0x5D,
    EOR_ABSY = 0x59,
    EOR_IIX = 0x41,
    EOR_IIY = 0x51,
    // INC
    INC_ZP = 0xE6,
    INC_ZPX = 0xF6,
    INC_ABS = 0xEE,
    INC_ABSX = 0xFE,
    // INX
    INX_IMPL = 0xE8,
    // INY
    INY_IMPL = 0xC8,
    // JMP
    JMP_ABS = 0x4C,
    JMP_IND = 0x6C,
    // JSR
    JSR_ABS = 0x20,
    // LDA
    LDA_IMM = 0xA9,
    LDA_ZP = 0xA5,
    LDA_ZPX = 0xB5,
    LDA_ABS = 0xAD,
    LDA_ABSX = 0xBD,
    LDA_ABSY = 0xB9,
    LDA_IIX = 0xA1,
    LDA_IIY = 0xB1,
    // LDX
    LDX_IMM = 0xA2,
    LDX_ZP = 0xA6,
    LDX_ZPY = 0xB6,
    LDX_ABS = 0xAE,
    LDX_ABSY = 0xBE,
    // LDY
    LDY_IMM = 0xA0,
    LDY_ZP = 0xA4,
    LDY_ZPX = 0xB4,
    LDY_ABS = 0xAC,
    LDY_ABSX = 0xBC,
    // LSR
    LSR_ACC = 0x4A,
    LSR_ZP = 0x46,
    LSR_ZPX = 0x56,
    LSR_ABS = 0x4E,
    LSR_ABSX = 0x5E,
    // NOP
    NOP_IMPL = 0xEA,
    // ORA
    ORA_IMM = 0x09,
    ORA_ZP = 0x05,
    ORA_ZPX = 0x15,
    ORA_ABS = 0x0D,
    ORA_ABSX = 0x1D,
    ORA_ABSY = 0x19,
    ORA_IIX = 0x01,
    ORA_IIY = 0x11,
    // PHA
    PHA_IMPL = 0x48,
    // PHP
    PHP_IMPL = 0x08,
    // PLA
    PLA_IMPL = 0x68,
    // PLP
    PLP_IMPL = 0x28,
    // ROL
    ROL_ACC = 0x2A,
    ROL_ZP = 0x26,
    ROL_ZPX = 0x36,
    ROL_ABS = 0x2E,
    ROL_ABSX = 0x3E,
    // ROR
    ROR_ACC = 0x6A,
    ROR_ZP = 0x66,
    ROR_ZPX = 0x76,
    ROR_ABS = 0x6E,
    ROR_ABSX = 0x7E,
    // RTI
    RTI_IMPL = 0x40,
    // RTS
    RTS_IMPL = 0x60,
    // SBC
    SBC_IMM = 0xE9,
    SBC_ZP = 0xE5,
    SBC_ZPX = 0xF5,
    SBC_ABS = 0xED,
    SBC_ABSX = 0xFD,
    SBC_ABSY = 0xF9,
    SBC_IIX = 0xE1,
    SBC_IIY = 0xF1,
    // SEC
    SEC_IMPL = 0x38,
    // SED
    SED_IMPL = 0xF8,
    // SEI
    SEI_IMPL = 0x78,
    // STA
    STA_ZP = 0x85,
    STA_ZPX = 0x95,
    STA_ABS = 0x8D,
    STA_ABSX = 0x9D,
    STA_ABSY = 0x99,
    STA_IIX = 0x81,
    STA_IIY = 0x91,
    // STX
    STX_ZP = 0x86,
    STX_ZPY = 0x96,
    STX_ABS = 0x8E,
    // STY
    STY_ZP = 0x84,
    STY_ZPX = 0x94,
    STY_ABS = 0x8C,
    // TAX
    TAX_IMPL = 0xAA,
    // TAY
    TAY_IMPL = 0xA8,
    // TSX
    TSX_IMPL = 0xBA,
    // TXA
    TXA_IMPL = 0x8A,
    // TXS
    TXS_IMPL = 0x9A,
    // TYA
    TYA_IMPL = 0x98,
};

enum PFlags // status register, P
{
    N,      // (N) negative flag
    V,      // (V) overflow flag
    TRUE,   // (1) no CPU effect, always 1
    B,      // (B) break flag
    D,      // (D) decimal flag (disabled for NES)
    I,      // (I) interrupt disable
    Z,      // (Z) zero flag
    C,      // (C) carry flag
};

// STRUCTS
struct OpCode
{
    const char* name = nullptr;
    uint8_t opCodeValue = 0;
    uint8_t numBytes = 0;
    uint8_t minCycles = 0;
    AddressingModes mode = NONE;
};

struct Instruction
{
    OpCode opCode;
    void (Cpu::*cycles)() = nullptr;
    void (Cpu::*execute)() = nullptr;
};

// CLASS
class Cpu
{
    public:
        // constructors
        Cpu(Bus& bus);
        Cpu(Bus& bus, string fileName);

        // address/data bus
        Bus& bus;

        // set cpu state
        void setTestCpuState(string reg, uint16_t value);
        uint8_t readTestMem(uint16_t address);
        void writeTestMem(uint16_t address, uint8_t value);
        uint16_t getRegisterValue(string reg);

        // runs 1 cycle
        void runCycle();

        // runs all cycles at once
        void autoRunCycles();

        // write cpu state to file
        void printState();

    private:
        // instructions and corresponding properties
        Instruction instructionSet[256];
        Instruction currentInstruction;
        uint8_t cyclesRequired;
        uint8_t currentCycle;
        void createInstructionSet();

        //  
        uint8_t lowByte;
        uint8_t highByte;
        uint16_t address;
        uint8_t data;
        bool readMode;
        bool writeMode;
        uint8_t read(uint16_t address);
        void write(uint16_t address, uint8_t value);
        void fetchOpCode();
        void fetchLowByte();
        void fetchHighByte();
        void setReadMode(bool isOn);
        void setWriteMode(bool isOn);
        
        // loads a cartridge into memory
        void loadCartridge(string fileName);
        
        // initializer for power up state - sets all registers and flags
        void powerUpStateInitializer();
        
        // registers
        uint8_t A;      // accumulator
        uint8_t X;      // X index
        uint8_t Y;      // Y index
        uint16_t PC;    // program counter
        uint8_t S;      // stack pointer
        uint8_t P;      // status register (NV1B DIZC)
        void setPFlag(PFlags flag, bool val);
        bool getPFlag(PFlags flag);

        // addressing modes
        void IMP(); //
        void IMM(); //
        void ACC(); //
        void REL();
        void ZP();  //
        void ZPX(); //
        void ZPY(); //
        void ABS(); //
        void ABX(); //
        void ABY(); //
        void IND(); //
        void IIX(); //
        void IIY(); //

        // instructions, 56 total
        void ADC();
        void AND();
        void ASL();
        void BCC();
        void BCS();
        void BEQ();
        void BIT();
        void BMI();
        void BNE();
        void BPL();
        void BRK();
        void BVC();
        void BVS();
        void CLC();
        void CLD();
        void CLI();
        void CLV();
        void CMP();
        void CPX();
        void CPY();
        void DEC();
        void DEX();
        void DEY();
        void EOR();
        void INC();
        void INX();
        void INY();
        void JMP();
        void JSR();
        void LDA();
        void LDX();
        void LDY();
        void LSR();
        void NOP();
        void ORA();
        void PHA();
        void PHP();
        void PLA();
        void PLP();
        void ROL();
        void ROR();
        void RTI();
        void RTS();
        void SBC();
        void SEC();
        void SED();
        void SEI();
        void STA();
        void STX();
        void STY();
        void TAX();
        void TAY();
        void TSX();
        void TXA();
        void TXS();
        void TYA();
};

#endif