#include "CpuMemory.h"

// forward declarations for structs
class Cpu;

// enums for addressing modes
enum AddressingModes
{
    Accumulator,
    Immediate,
    ZeroPage,
    Absolute,
    Relative,
    Indirect,
    IndexedIndirect,
    Implied
};

enum AddressingModeModifiers
{
    None,
    XIndexed,
    YIndexed
};

// struct for instruction opcodes and their properties
struct Instruction
{
    AddressingModes mode;
    AddressingModeModifiers modifier;
    uint8_t numBytes;
    uint8_t minCycles;
    void (Cpu::*instruction)();
};

class Cpu
{
    public:
        // constructors
        Cpu();
        Cpu(string fileName);

    private:
        // cpu memory
        CpuMemory memory;
        
        // instruction set that contains all opcodes for all instructions
        Instruction instructionSet[256];

        // operand fetched from memory using adressing mode
        uint16_t operand;
  
        // initializes all instructions and their opcode properties
        void createInstructionSet();

        // initializer for power up state - sets all registers and flags
        void powerUpStateInitializer();
        
        // loads a cartridge into memory
        void loadCartridge(string fileName);
        
        // calculates effective address based on address mode
        void setOperand(AddressingModes mode, AddressingModeModifiers modifier, uint16_t address);
        
        // fetch, decode, execute cycle
        void cycle();
        uint8_t fetch();
        const Instruction& decode(uint8_t opcode);
        void execute(const Instruction& instruction);
        
        // registers, A, X, Y, PC, S, P(NV1B DIZC)
        // (A) accumulator
        uint8_t A;
        // (X, Y) indexes
        uint8_t X;
        uint8_t Y;
        // (PC) program counter
        uint16_t PC;
        // (S) stack pointer
        uint8_t S;
        // (P) status register = NV1B DIZC
        uint8_t P;
        // (N) negative flag
        bool N;
        // (V) overflow flag
        bool V;
        // (1) no CPU effect, always 1
        bool noCpuEffect;
        // (B) break flag, no CPU effect, transient interrupt signal
        // 0 if pushed by interrups [NMI, IRQ]
        // 1 if pushed by instructions [BRK, PHP]
        bool B;
        // (D) decimal flag (disabled for NES)
        bool D;
        // (I) interrupt disable
        bool I;
        // (Z) zero flag
        bool Z;
        // (C) carry flag
        bool C;

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