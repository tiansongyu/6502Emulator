/*
        olc6502 - An emulation of the 6502/2A03 processor
        "Thanks Dad for believing computers were gonna be a big deal..."
   - javidx9

        License (OLC-3)
        ~~~~~~~~~~~~~~~

        Copyright 2018-2019 OneLoneCoder.com

        Redistribution and use in source and binary forms, with or without
        modification, are permitted provided that the following conditions
        are met:

        1. Redistributions or derivations of source code must retain the above
        copyright notice, this list of conditions and the following disclaimer.

        2. Redistributions or derivative works in binary form must reproduce
        the above copyright notice. This list of conditions and the following
        disclaimer must be reproduced in the documentation and/or other
        materials provided with the distribution.

        3. Neither the name of the copyright holder nor the names of its
        contributors may be used to endorse or promote products derived
        from this software without specific prior written permission.

        THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
        "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
        LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
        A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
        HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
        SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
        LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
        DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
        THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
        (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
        OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

        Author
        ~~~~~~
        David Barr, aka javidx9, OneLoneCoder 2019
*/

#include "Nes6502.h"

#include "Bus.h"
#include "StateIO.h"

// The opcode translation table. 16x16 entries: the bottom 4 bits of the
// instruction byte choose the column, the top 4 bits choose the row.
// "???" rows are unofficial opcodes, captured by XXX (a NOP with the
// documented cycle cost).
using a = Nes6502;
const Nes6502::Instruction Nes6502::lookup[256] = {
      {"BRK", &a::BRK, AM::IMM, 7}, {"ORA", &a::ORA, AM::IZX, 6},
      {"???", &a::XXX, AM::IMP, 2}, {"???", &a::XXX, AM::IMP, 8},
      {"???", &a::NOP, AM::IMP, 3}, {"ORA", &a::ORA, AM::ZP0, 3},
      {"ASL", &a::ASL, AM::ZP0, 5}, {"???", &a::XXX, AM::IMP, 5},
      {"PHP", &a::PHP, AM::IMP, 3}, {"ORA", &a::ORA, AM::IMM, 2},
      {"ASL", &a::ASL, AM::IMP, 2}, {"???", &a::XXX, AM::IMP, 2},
      {"???", &a::NOP, AM::IMP, 4}, {"ORA", &a::ORA, AM::ABS, 4},
      {"ASL", &a::ASL, AM::ABS, 6}, {"???", &a::XXX, AM::IMP, 6},
      {"BPL", &a::BPL, AM::REL, 2}, {"ORA", &a::ORA, AM::IZY, 5},
      {"???", &a::XXX, AM::IMP, 2}, {"???", &a::XXX, AM::IMP, 8},
      {"???", &a::NOP, AM::IMP, 4}, {"ORA", &a::ORA, AM::ZPX, 4},
      {"ASL", &a::ASL, AM::ZPX, 6}, {"???", &a::XXX, AM::IMP, 6},
      {"CLC", &a::CLC, AM::IMP, 2}, {"ORA", &a::ORA, AM::ABY, 4},
      {"???", &a::NOP, AM::IMP, 2}, {"???", &a::XXX, AM::IMP, 7},
      {"???", &a::NOP, AM::IMP, 4}, {"ORA", &a::ORA, AM::ABX, 4},
      {"ASL", &a::ASL, AM::ABX, 7}, {"???", &a::XXX, AM::IMP, 7},
      {"JSR", &a::JSR, AM::ABS, 6}, {"AND", &a::AND, AM::IZX, 6},
      {"???", &a::XXX, AM::IMP, 2}, {"???", &a::XXX, AM::IMP, 8},
      {"BIT", &a::BIT, AM::ZP0, 3}, {"AND", &a::AND, AM::ZP0, 3},
      {"ROL", &a::ROL, AM::ZP0, 5}, {"???", &a::XXX, AM::IMP, 5},
      {"PLP", &a::PLP, AM::IMP, 4}, {"AND", &a::AND, AM::IMM, 2},
      {"ROL", &a::ROL, AM::IMP, 2}, {"???", &a::XXX, AM::IMP, 2},
      {"BIT", &a::BIT, AM::ABS, 4}, {"AND", &a::AND, AM::ABS, 4},
      {"ROL", &a::ROL, AM::ABS, 6}, {"???", &a::XXX, AM::IMP, 6},
      {"BMI", &a::BMI, AM::REL, 2}, {"AND", &a::AND, AM::IZY, 5},
      {"???", &a::XXX, AM::IMP, 2}, {"???", &a::XXX, AM::IMP, 8},
      {"???", &a::NOP, AM::IMP, 4}, {"AND", &a::AND, AM::ZPX, 4},
      {"ROL", &a::ROL, AM::ZPX, 6}, {"???", &a::XXX, AM::IMP, 6},
      {"SEC", &a::SEC, AM::IMP, 2}, {"AND", &a::AND, AM::ABY, 4},
      {"???", &a::NOP, AM::IMP, 2}, {"???", &a::XXX, AM::IMP, 7},
      {"???", &a::NOP, AM::IMP, 4}, {"AND", &a::AND, AM::ABX, 4},
      {"ROL", &a::ROL, AM::ABX, 7}, {"???", &a::XXX, AM::IMP, 7},
      {"RTI", &a::RTI, AM::IMP, 6}, {"EOR", &a::EOR, AM::IZX, 6},
      {"???", &a::XXX, AM::IMP, 2}, {"???", &a::XXX, AM::IMP, 8},
      {"???", &a::NOP, AM::IMP, 3}, {"EOR", &a::EOR, AM::ZP0, 3},
      {"LSR", &a::LSR, AM::ZP0, 5}, {"???", &a::XXX, AM::IMP, 5},
      {"PHA", &a::PHA, AM::IMP, 3}, {"EOR", &a::EOR, AM::IMM, 2},
      {"LSR", &a::LSR, AM::IMP, 2}, {"???", &a::XXX, AM::IMP, 2},
      {"JMP", &a::JMP, AM::ABS, 3}, {"EOR", &a::EOR, AM::ABS, 4},
      {"LSR", &a::LSR, AM::ABS, 6}, {"???", &a::XXX, AM::IMP, 6},
      {"BVC", &a::BVC, AM::REL, 2}, {"EOR", &a::EOR, AM::IZY, 5},
      {"???", &a::XXX, AM::IMP, 2}, {"???", &a::XXX, AM::IMP, 8},
      {"???", &a::NOP, AM::IMP, 4}, {"EOR", &a::EOR, AM::ZPX, 4},
      {"LSR", &a::LSR, AM::ZPX, 6}, {"???", &a::XXX, AM::IMP, 6},
      {"CLI", &a::CLI, AM::IMP, 2}, {"EOR", &a::EOR, AM::ABY, 4},
      {"???", &a::NOP, AM::IMP, 2}, {"???", &a::XXX, AM::IMP, 7},
      {"???", &a::NOP, AM::IMP, 4}, {"EOR", &a::EOR, AM::ABX, 4},
      {"LSR", &a::LSR, AM::ABX, 7}, {"???", &a::XXX, AM::IMP, 7},
      {"RTS", &a::RTS, AM::IMP, 6}, {"ADC", &a::ADC, AM::IZX, 6},
      {"???", &a::XXX, AM::IMP, 2}, {"???", &a::XXX, AM::IMP, 8},
      {"???", &a::NOP, AM::IMP, 3}, {"ADC", &a::ADC, AM::ZP0, 3},
      {"ROR", &a::ROR, AM::ZP0, 5}, {"???", &a::XXX, AM::IMP, 5},
      {"PLA", &a::PLA, AM::IMP, 4}, {"ADC", &a::ADC, AM::IMM, 2},
      {"ROR", &a::ROR, AM::IMP, 2}, {"???", &a::XXX, AM::IMP, 2},
      {"JMP", &a::JMP, AM::IND, 5}, {"ADC", &a::ADC, AM::ABS, 4},
      {"ROR", &a::ROR, AM::ABS, 6}, {"???", &a::XXX, AM::IMP, 6},
      {"BVS", &a::BVS, AM::REL, 2}, {"ADC", &a::ADC, AM::IZY, 5},
      {"???", &a::XXX, AM::IMP, 2}, {"???", &a::XXX, AM::IMP, 8},
      {"???", &a::NOP, AM::IMP, 4}, {"ADC", &a::ADC, AM::ZPX, 4},
      {"ROR", &a::ROR, AM::ZPX, 6}, {"???", &a::XXX, AM::IMP, 6},
      {"SEI", &a::SEI, AM::IMP, 2}, {"ADC", &a::ADC, AM::ABY, 4},
      {"???", &a::NOP, AM::IMP, 2}, {"???", &a::XXX, AM::IMP, 7},
      {"???", &a::NOP, AM::IMP, 4}, {"ADC", &a::ADC, AM::ABX, 4},
      {"ROR", &a::ROR, AM::ABX, 7}, {"???", &a::XXX, AM::IMP, 7},
      {"???", &a::NOP, AM::IMP, 2}, {"STA", &a::STA, AM::IZX, 6},
      {"???", &a::NOP, AM::IMP, 2}, {"???", &a::XXX, AM::IMP, 6},
      {"STY", &a::STY, AM::ZP0, 3}, {"STA", &a::STA, AM::ZP0, 3},
      {"STX", &a::STX, AM::ZP0, 3}, {"???", &a::XXX, AM::IMP, 3},
      {"DEY", &a::DEY, AM::IMP, 2}, {"???", &a::NOP, AM::IMP, 2},
      {"TXA", &a::TXA, AM::IMP, 2}, {"???", &a::XXX, AM::IMP, 2},
      {"STY", &a::STY, AM::ABS, 4}, {"STA", &a::STA, AM::ABS, 4},
      {"STX", &a::STX, AM::ABS, 4}, {"???", &a::XXX, AM::IMP, 4},
      {"BCC", &a::BCC, AM::REL, 2}, {"STA", &a::STA, AM::IZY, 6},
      {"???", &a::XXX, AM::IMP, 2}, {"???", &a::XXX, AM::IMP, 6},
      {"STY", &a::STY, AM::ZPX, 4}, {"STA", &a::STA, AM::ZPX, 4},
      {"STX", &a::STX, AM::ZPY, 4}, {"???", &a::XXX, AM::IMP, 4},
      {"TYA", &a::TYA, AM::IMP, 2}, {"STA", &a::STA, AM::ABY, 5},
      {"TXS", &a::TXS, AM::IMP, 2}, {"???", &a::XXX, AM::IMP, 5},
      {"???", &a::NOP, AM::IMP, 5}, {"STA", &a::STA, AM::ABX, 5},
      {"???", &a::XXX, AM::IMP, 5}, {"???", &a::XXX, AM::IMP, 5},
      {"LDY", &a::LDY, AM::IMM, 2}, {"LDA", &a::LDA, AM::IZX, 6},
      {"LDX", &a::LDX, AM::IMM, 2}, {"???", &a::XXX, AM::IMP, 6},
      {"LDY", &a::LDY, AM::ZP0, 3}, {"LDA", &a::LDA, AM::ZP0, 3},
      {"LDX", &a::LDX, AM::ZP0, 3}, {"???", &a::XXX, AM::IMP, 3},
      {"TAY", &a::TAY, AM::IMP, 2}, {"LDA", &a::LDA, AM::IMM, 2},
      {"TAX", &a::TAX, AM::IMP, 2}, {"???", &a::XXX, AM::IMP, 2},
      {"LDY", &a::LDY, AM::ABS, 4}, {"LDA", &a::LDA, AM::ABS, 4},
      {"LDX", &a::LDX, AM::ABS, 4}, {"???", &a::XXX, AM::IMP, 4},
      {"BCS", &a::BCS, AM::REL, 2}, {"LDA", &a::LDA, AM::IZY, 5},
      {"???", &a::XXX, AM::IMP, 2}, {"???", &a::XXX, AM::IMP, 5},
      {"LDY", &a::LDY, AM::ZPX, 4}, {"LDA", &a::LDA, AM::ZPX, 4},
      {"LDX", &a::LDX, AM::ZPY, 4}, {"???", &a::XXX, AM::IMP, 4},
      {"CLV", &a::CLV, AM::IMP, 2}, {"LDA", &a::LDA, AM::ABY, 4},
      {"TSX", &a::TSX, AM::IMP, 2}, {"???", &a::XXX, AM::IMP, 4},
      {"LDY", &a::LDY, AM::ABX, 4}, {"LDA", &a::LDA, AM::ABX, 4},
      {"LDX", &a::LDX, AM::ABY, 4}, {"???", &a::XXX, AM::IMP, 4},
      {"CPY", &a::CPY, AM::IMM, 2}, {"CMP", &a::CMP, AM::IZX, 6},
      {"???", &a::NOP, AM::IMP, 2}, {"???", &a::XXX, AM::IMP, 8},
      {"CPY", &a::CPY, AM::ZP0, 3}, {"CMP", &a::CMP, AM::ZP0, 3},
      {"DEC", &a::DEC, AM::ZP0, 5}, {"???", &a::XXX, AM::IMP, 5},
      {"INY", &a::INY, AM::IMP, 2}, {"CMP", &a::CMP, AM::IMM, 2},
      {"DEX", &a::DEX, AM::IMP, 2}, {"???", &a::XXX, AM::IMP, 2},
      {"CPY", &a::CPY, AM::ABS, 4}, {"CMP", &a::CMP, AM::ABS, 4},
      {"DEC", &a::DEC, AM::ABS, 6}, {"???", &a::XXX, AM::IMP, 6},
      {"BNE", &a::BNE, AM::REL, 2}, {"CMP", &a::CMP, AM::IZY, 5},
      {"???", &a::XXX, AM::IMP, 2}, {"???", &a::XXX, AM::IMP, 8},
      {"???", &a::NOP, AM::IMP, 4}, {"CMP", &a::CMP, AM::ZPX, 4},
      {"DEC", &a::DEC, AM::ZPX, 6}, {"???", &a::XXX, AM::IMP, 6},
      {"CLD", &a::CLD, AM::IMP, 2}, {"CMP", &a::CMP, AM::ABY, 4},
      {"NOP", &a::NOP, AM::IMP, 2}, {"???", &a::XXX, AM::IMP, 7},
      {"???", &a::NOP, AM::IMP, 4}, {"CMP", &a::CMP, AM::ABX, 4},
      {"DEC", &a::DEC, AM::ABX, 7}, {"???", &a::XXX, AM::IMP, 7},
      {"CPX", &a::CPX, AM::IMM, 2}, {"SBC", &a::SBC, AM::IZX, 6},
      {"???", &a::NOP, AM::IMP, 2}, {"???", &a::XXX, AM::IMP, 8},
      {"CPX", &a::CPX, AM::ZP0, 3}, {"SBC", &a::SBC, AM::ZP0, 3},
      {"INC", &a::INC, AM::ZP0, 5}, {"???", &a::XXX, AM::IMP, 5},
      {"INX", &a::INX, AM::IMP, 2}, {"SBC", &a::SBC, AM::IMM, 2},
      {"NOP", &a::NOP, AM::IMP, 2}, {"???", &a::SBC, AM::IMP, 2},
      {"CPX", &a::CPX, AM::ABS, 4}, {"SBC", &a::SBC, AM::ABS, 4},
      {"INC", &a::INC, AM::ABS, 6}, {"???", &a::XXX, AM::IMP, 6},
      {"BEQ", &a::BEQ, AM::REL, 2}, {"SBC", &a::SBC, AM::IZY, 5},
      {"???", &a::XXX, AM::IMP, 2}, {"???", &a::XXX, AM::IMP, 8},
      {"???", &a::NOP, AM::IMP, 4}, {"SBC", &a::SBC, AM::ZPX, 4},
      {"INC", &a::INC, AM::ZPX, 6}, {"???", &a::XXX, AM::IMP, 6},
      {"SED", &a::SED, AM::IMP, 2}, {"SBC", &a::SBC, AM::ABY, 4},
      {"NOP", &a::NOP, AM::IMP, 2}, {"???", &a::XXX, AM::IMP, 7},
      {"???", &a::NOP, AM::IMP, 4}, {"SBC", &a::SBC, AM::ABX, 4},
      {"INC", &a::INC, AM::ABX, 7}, {"???", &a::XXX, AM::IMP, 7},
  };

Nes6502::Nes6502() {}

Nes6502::~Nes6502() {}

///////////////////////////////////////////////////////////////////////////////
// BUS CONNECTIVITY

// Reads an 8-bit byte from the bus, located at the specified 16-bit address
uint8_t Nes6502::read(uint16_t a) {
  // In normal operation "read only" is set to false. This may seem odd. Some
  // devices on the bus may change state when they are read from, and this
  // is intentional under normal circumstances. However the disassembler will
  // want to read the data at an address without changing the state of the
  // devices on the bus
  return bus->cpuRead(a, false);
}

// Writes a byte to the bus at the specified address
void Nes6502::write(uint16_t a, uint8_t d) { bus->cpuWrite(a, d); }

///////////////////////////////////////////////////////////////////////////////
// EXTERNAL INPUTS

// Forces the 6502 into a known state. This is hard-wired inside the CPU. The
// registers are set to 0x00, the status register is cleared except for unused
// bit which remains at 1. An absolute address is read from location 0xFFFC
// which contains a second address that the program counter is set to. This
// allows the programmer to jump to a known and programmable location in the
// memory to start executing from.
void Nes6502::reset() {
  // Get address to set program counter to
  uint16_t lo = read(0xFFFC);
  uint16_t hi = read(0xFFFD);
  pc = (hi << 8) | lo;

  // Reset internal registers
  a = 0;
  x = 0;
  y = 0;
  stkp = 0xFD;
  status = 0x00 | U;

  // Clear internal helper variables
  addr_rel = 0x0000;
  addr_abs = 0x0000;
  fetched = 0x00;

  // Reset takes time
  cycles = 8;
}

// The common interrupt sequence: the current instruction is allowed to finish
// (which I facilitate by doing the whole thing when cycles == 0), then the
// program counter and status register are pushed to the stack, interrupts are
// disabled, and execution continues at the address read from the vector. The
// service routine returns with RTI, which restores status and pc.
void Nes6502::Interrupt(uint16_t vector, uint8_t n_cycles) {
  // Push the program counter to the stack. It's 16-bits dont
  // forget so that takes two pushes
  write(0x0100 + stkp, (pc >> 8) & 0x00FF);
  stkp--;
  write(0x0100 + stkp, pc & 0x00FF);
  stkp--;

  // Then Push the status register to the stack. The pushed copy must show
  // I as it was when the interrupt hit - hardware raises I only after the
  // push, so RTI re-enables interrupts inside the service routine's caller.
  SetFlag(B, 0);
  SetFlag(U, 1);
  write(0x0100 + stkp, status);
  stkp--;
  SetFlag(I, 1);

  // Read new program counter location from fixed address
  pc = (uint16_t)read(vector) | ((uint16_t)read(vector + 1) << 8);

  // Interrupts take time
  cycles = n_cycles;
}

// Interrupt requests only happen if the "disable interrupt" flag is 0.
void Nes6502::irq() {
  if (GetFlag(I) == 0) Interrupt(0xFFFE, 7);
}

// A Non-Maskable Interrupt cannot be ignored. It behaves in exactly the
// same way as a regular IRQ, but reads the new program counter address
// form location 0xFFFA.
void Nes6502::nmi() { Interrupt(0xFFFA, 8); }

// Perform one clock cycles worth of emulation
void Nes6502::clock() {
  // Each instruction requires a variable number of clock cycles to execute.
  // In my emulation, I only care about the final result and so I perform
  // the entire computation in one hit. In hardware, each clock cycle would
  // perform "microcode" style transformations of the CPUs state.
  //
  // To remain compliant with connected devices, it's important that the
  // emulation also takes "time" in order to execute instructions, so I
  // implement that delay by simply counting down the cycles required by
  // the instruction. When it reaches 0, the instruction is complete, and
  // the next one is ready to be executed.
  if (cycles == 0) {
    // Read next instruction byte. This 8-bit value is used to index
    // the translation table to get the relevant information about
    // how to implement the instruction
    opcode = read(pc);

    // Always set the unused status flag bit to 1
    SetFlag(U, true);

    // Increment program counter, we read the opcode byte
    pc++;

    // Get Starting number of cycles
    cycles = lookup[opcode].cycles;

    // Perform fetch of intermmediate data using the
    // required addressing mode
    uint8_t additional_cycle1 = DoAddressing(lookup[opcode].mode);

    // Perform operation
    uint8_t additional_cycle2 = (this->*lookup[opcode].operate)();

    // The addressmode and opcode may have altered the number
    // of cycles this instruction requires before its completed
    cycles += (additional_cycle1 & additional_cycle2);

    // Always set the unused status flag bit to 1
    SetFlag(U, true);
  }

  // Decrement the number of cycles remaining for this instruction
  cycles--;
}

///////////////////////////////////////////////////////////////////////////////
// FLAG FUNCTIONS

// Returns the value of a specific bit of the status register
uint8_t Nes6502::GetFlag(FLAGS6502 f) { return ((status & f) > 0) ? 1 : 0; }

// Sets or clears a specific bit of the status register
void Nes6502::SetFlag(FLAGS6502 f, bool v) {
  if (v)
    status |= f;
  else
    status &= ~f;
}

///////////////////////////////////////////////////////////////////////////////
// ADDRESSING MODES

// The 6502 can address between 0x0000 - 0xFFFF. The high byte is often referred
// to as the "page", and the low byte is the offset into that page. This implies
// there are 256 pages, each containing 256 bytes.
//
// Several addressing modes have the potential to require an additional clock
// cycle if they cross a page boundary. This is combined with several
// instructions that enable this additional clock cycle. So each addressing
// function returns a flag saying it has potential, as does each instruction. If
// both instruction and address function return 1, then an additional clock
// cycle is required.

uint8_t Nes6502::DoAddressing(AM mode) {
  switch (mode) {
    case AM::IMP: return IMP();
    case AM::IMM: return IMM();
    case AM::ZP0: return ZP0();
    case AM::ZPX: return ZPX();
    case AM::ZPY: return ZPY();
    case AM::REL: return REL();
    case AM::ABS: return ABS();
    case AM::ABX: return ABX();
    case AM::ABY: return ABY();
    case AM::IND: return IND();
    case AM::IZX: return IZX();
    case AM::IZY: return IZY();
  }
  return 0;
}

// Address Mode: Implied
// There is no additional data required for this instruction. The instruction
// does something very simple like like sets a status bit. However, we will
// target the accumulator, for instructions like PHA
uint8_t Nes6502::IMP() {
  fetched = a;
  return 0;
}

// Address Mode: Immediate
// The instruction expects the next byte to be used as a value, so we'll prep
// the read address to point to the next byte
uint8_t Nes6502::IMM() {
  addr_abs = pc++;
  return 0;
}

// Address Mode: Zero Page
// To save program bytes, zero page addressing allows you to absolutely address
// a location in first 0xFF bytes of address range. Clearly this only requires
// one byte instead of the usual two.
uint8_t Nes6502::ZP0() {
  addr_abs = read(pc);
  pc++;
  addr_abs &= 0x00FF;
  return 0;
}

// Address Mode: Zero Page with X Offset
// Fundamentally the same as Zero Page addressing, but the contents of the X
// Register is added to the supplied single byte address. This is useful for
// iterating through ranges within the first page.
uint8_t Nes6502::ZPX() {
  addr_abs = (read(pc) + x);
  pc++;
  addr_abs &= 0x00FF;
  return 0;
}

// Address Mode: Zero Page with Y Offset
// Same as above but uses Y Register for offset
uint8_t Nes6502::ZPY() {
  addr_abs = (read(pc) + y);
  pc++;
  addr_abs &= 0x00FF;
  return 0;
}

// Address Mode: Relative
// This address mode is exclusive to branch instructions. The address
// must reside within -128 to +127 of the branch instruction, i.e.
// you cant directly branch to any address in the addressable range.
uint8_t Nes6502::REL() {
  addr_rel = read(pc);
  pc++;
  if (addr_rel & 0x80) addr_rel |= 0xFF00;
  return 0;
}

// Address Mode: Absolute
// A full 16-bit address is loaded and used
uint8_t Nes6502::ABS() {
  uint16_t lo = read(pc);
  pc++;
  uint16_t hi = read(pc);
  pc++;

  addr_abs = (hi << 8) | lo;

  return 0;
}

// Address Mode: Absolute with X Offset
// Fundamentally the same as absolute addressing, but the contents of the X
// Register is added to the supplied two byte address. If the resulting address
// changes the page, an additional clock cycle is required
uint8_t Nes6502::ABX() {
  uint16_t lo = read(pc);
  pc++;
  uint16_t hi = read(pc);
  pc++;

  addr_abs = (hi << 8) | lo;
  addr_abs += x;

  return ((addr_abs & 0xFF00) != (hi << 8)) ? 1 : 0;
}

// Address Mode: Absolute with Y Offset
// Fundamentally the same as absolute addressing, but the contents of the Y
// Register is added to the supplied two byte address. If the resulting address
// changes the page, an additional clock cycle is required
uint8_t Nes6502::ABY() {
  uint16_t lo = read(pc);
  pc++;
  uint16_t hi = read(pc);
  pc++;

  addr_abs = (hi << 8) | lo;
  addr_abs += y;

  return ((addr_abs & 0xFF00) != (hi << 8)) ? 1 : 0;
}

// Note: The next 3 address modes use indirection (aka Pointers!)

// Address Mode: Indirect
// The supplied 16-bit address is read to get the actual 16-bit address. This is
// instruction is unusual in that it has a bug in the hardware! To emulate its
// function accurately, we also need to emulate this bug. If the low byte of the
// supplied address is 0xFF, then to read the high byte of the actual address
// we need to cross a page boundary. This doesnt actually work on the chip as
// designed, instead it wraps back around in the same page, yielding an
// invalid actual address
uint8_t Nes6502::IND() {
  uint16_t ptr_lo = read(pc);
  pc++;
  uint16_t ptr_hi = read(pc);
  pc++;

  uint16_t ptr = (ptr_hi << 8) | ptr_lo;

  if (ptr_lo == 0x00FF) {  // Simulate page boundary hardware bug
    addr_abs = (read(ptr & 0xFF00) << 8) | read(ptr + 0);
  } else {  // Behave normally
    addr_abs = (read(ptr + 1) << 8) | read(ptr + 0);
  }

  return 0;
}

// Address Mode: Indirect X
// The supplied 8-bit address is offset by X Register to index
// a location in page 0x00. The actual 16-bit address is read
// from this location
uint8_t Nes6502::IZX() {
  uint16_t t = read(pc);
  pc++;

  uint16_t lo = read((uint16_t)(t + (uint16_t)x) & 0x00FF);
  uint16_t hi = read((uint16_t)(t + (uint16_t)x + 1) & 0x00FF);

  addr_abs = (hi << 8) | lo;

  return 0;
}

// Address Mode: Indirect Y
// The supplied 8-bit address indexes a location in page 0x00. From
// here the actual 16-bit address is read, and the contents of
// Y Register is added to it to offset it. If the offset causes a
// change in page then an additional clock cycle is required.
uint8_t Nes6502::IZY() {
  uint16_t t = read(pc);
  pc++;

  uint16_t lo = read(t & 0x00FF);
  uint16_t hi = read((t + 1) & 0x00FF);

  addr_abs = (hi << 8) | lo;
  addr_abs += y;

  return ((addr_abs & 0xFF00) != (hi << 8)) ? 1 : 0;
}

// This function sources the data used by the instruction into
// a convenient numeric variable. Some instructions dont have to
// fetch data as the source is implied by the instruction. For example
// "INX" increments the X register. There is no additional data
// required. For all other addressing modes, the data resides at
// the location held within addr_abs, so it is read from there.
// Immediate adress mode exploits this slightly, as that has
// set addr_abs = pc + 1, so it fetches the data from the
// next byte.
uint8_t Nes6502::fetch() {
  if (lookup[opcode].mode != AM::IMP) fetched = read(addr_abs);
  return fetched;
}

///////////////////////////////////////////////////////////////////////////////
// SHARED INSTRUCTION MECHANICS

// All branch instructions are the same circuit: if the condition holds,
// jump to pc + relative offset, paying one extra cycle for the taken
// branch and another for crossing a page boundary.
uint8_t Nes6502::Branch(bool take) {
  if (take) {
    cycles++;
    addr_abs = pc + addr_rel;

    if ((addr_abs & 0xFF00) != (pc & 0xFF00)) cycles++;

    pc = addr_abs;
  }
  return 0;
}

// Shift and rotate instructions write their result to the accumulator in
// implied (accumulator) mode, and back to memory otherwise.
void Nes6502::StoreALU(uint8_t v) {
  if (lookup[opcode].mode == AM::IMP)
    a = v;
  else
    write(addr_abs, v);
}

///////////////////////////////////////////////////////////////////////////////
// INSTRUCTION IMPLEMENTATIONS

// Instruction: Add with Carry In
// Function:    A = A + M + C
// Flags Out:   C, V, N, Z
//
// The addition is performed in the 16-bit domain so the carry appears in
// bit 8. The signed overflow flag is V = ~(A^M) & (A^R) on the sign bits:
// adding two numbers of the same sign overflowed exactly when the result's
// sign differs from theirs.
uint8_t Nes6502::ADC() {
  fetch();

  uint16_t t = (uint16_t)a + (uint16_t)fetched + (uint16_t)GetFlag(C);
  SetFlag(C, t > 255);
  SetFlag(Z, (t & 0x00FF) == 0);
  SetFlag(V,
          (~((uint16_t)a ^ (uint16_t)fetched) & ((uint16_t)a ^ t)) & 0x0080);
  SetFlag(N, t & 0x80);
  a = t & 0x00FF;

  // This instruction has the potential to require an additional clock cycle
  return 1;
}

// Instruction: Subtraction with Borrow In
// Function:    A = A - M - (1 - C)
// Flags Out:   C, V, N, Z
//
// A - M - (1-C) = A + ~M + C in two's complement, so inverting the operand
// turns this into exactly the addition above.
uint8_t Nes6502::SBC() {
  fetch();

  uint16_t value = ((uint16_t)fetched) ^ 0x00FF;

  uint16_t t = (uint16_t)a + value + (uint16_t)GetFlag(C);
  SetFlag(C, t & 0xFF00);
  SetFlag(Z, ((t & 0x00FF) == 0));
  SetFlag(V, (t ^ (uint16_t)a) & (t ^ value) & 0x0080);
  SetFlag(N, t & 0x0080);
  a = t & 0x00FF;
  return 1;
}

// Instruction: Bitwise Logic AND
// Function:    A = A & M
// Flags Out:   N, Z
uint8_t Nes6502::AND() {
  fetch();
  a = a & fetched;
  SetFlag(Z, a == 0x00);
  SetFlag(N, a & 0x80);
  return 1;
}

// Instruction: Arithmetic Shift Left
// Function:    A = C <- (A << 1) <- 0
// Flags Out:   N, Z, C
uint8_t Nes6502::ASL() {
  fetch();
  uint16_t t = (uint16_t)fetched << 1;
  SetFlag(C, (t & 0xFF00) > 0);
  SetFlag(Z, (t & 0x00FF) == 0x00);
  SetFlag(N, t & 0x80);
  StoreALU(t & 0x00FF);
  return 0;
}

// Instruction: Branch if Carry Clear
uint8_t Nes6502::BCC() { return Branch(GetFlag(C) == 0); }

// Instruction: Branch if Carry Set
uint8_t Nes6502::BCS() { return Branch(GetFlag(C) == 1); }

// Instruction: Branch if Equal
uint8_t Nes6502::BEQ() { return Branch(GetFlag(Z) == 1); }

// Instruction: Branch if Not Equal
uint8_t Nes6502::BNE() { return Branch(GetFlag(Z) == 0); }

// Instruction: Branch if Negative
uint8_t Nes6502::BMI() { return Branch(GetFlag(N) == 1); }

// Instruction: Branch if Positive
uint8_t Nes6502::BPL() { return Branch(GetFlag(N) == 0); }

// Instruction: Branch if Overflow Clear
uint8_t Nes6502::BVC() { return Branch(GetFlag(V) == 0); }

// Instruction: Branch if Overflow Set
uint8_t Nes6502::BVS() { return Branch(GetFlag(V) == 1); }

// Instruction: Test Bits in Memory with Accumulator
// Function:    Z <- (A & M) == 0,  N <- M7,  V <- M6
uint8_t Nes6502::BIT() {
  fetch();
  uint16_t t = a & fetched;
  SetFlag(Z, (t & 0x00FF) == 0x00);
  SetFlag(N, fetched & (1 << 7));
  SetFlag(V, fetched & (1 << 6));
  return 0;
}

// Instruction: Break
// Function:    Program Sourced Interrupt
uint8_t Nes6502::BRK() {
  // The IMM addressing mode already advanced pc past the padding byte, so
  // pc is the correct hardware return address (BRK opcode + 2). The pushed
  // status shows B=1 and the pre-interrupt I; I is raised after the push.
  write(0x0100 + stkp, (pc >> 8) & 0x00FF);
  stkp--;
  write(0x0100 + stkp, pc & 0x00FF);
  stkp--;

  SetFlag(B, 1);
  write(0x0100 + stkp, status);
  stkp--;
  SetFlag(B, 0);
  SetFlag(I, 1);

  pc = (uint16_t)read(0xFFFE) | ((uint16_t)read(0xFFFF) << 8);
  return 0;
}

// Instruction: Clear Carry Flag
uint8_t Nes6502::CLC() {
  SetFlag(C, false);
  return 0;
}

// Instruction: Clear Decimal Flag
uint8_t Nes6502::CLD() {
  SetFlag(D, false);
  return 0;
}

// Instruction: Enable Interrupts / Clear Interrupt Disable Flag
uint8_t Nes6502::CLI() {
  SetFlag(I, false);
  return 0;
}

// Instruction: Clear Overflow Flag
uint8_t Nes6502::CLV() {
  SetFlag(V, false);
  return 0;
}

// Instruction: Compare Accumulator
// Function:    C <- A >= M      Z <- (A - M) == 0
// Flags Out:   N, C, Z
uint8_t Nes6502::CMP() {
  fetch();
  uint16_t t = (uint16_t)a - (uint16_t)fetched;
  SetFlag(C, a >= fetched);
  SetFlag(Z, (t & 0x00FF) == 0x0000);
  SetFlag(N, t & 0x0080);
  return 1;
}

// Instruction: Compare X Register
// Function:    C <- X >= M      Z <- (X - M) == 0
// Flags Out:   N, C, Z
uint8_t Nes6502::CPX() {
  fetch();
  uint16_t t = (uint16_t)x - (uint16_t)fetched;
  SetFlag(C, x >= fetched);
  SetFlag(Z, (t & 0x00FF) == 0x0000);
  SetFlag(N, t & 0x0080);
  return 0;
}

// Instruction: Compare Y Register
// Function:    C <- Y >= M      Z <- (Y - M) == 0
// Flags Out:   N, C, Z
uint8_t Nes6502::CPY() {
  fetch();
  uint16_t t = (uint16_t)y - (uint16_t)fetched;
  SetFlag(C, y >= fetched);
  SetFlag(Z, (t & 0x00FF) == 0x0000);
  SetFlag(N, t & 0x0080);
  return 0;
}

// Instruction: Decrement Value at Memory Location
// Function:    M = M - 1
// Flags Out:   N, Z
uint8_t Nes6502::DEC() {
  fetch();
  uint16_t t = fetched - 1;
  write(addr_abs, t & 0x00FF);
  SetFlag(Z, (t & 0x00FF) == 0x0000);
  SetFlag(N, t & 0x0080);
  return 0;
}

// Instruction: Decrement X Register
// Function:    X = X - 1
// Flags Out:   N, Z
uint8_t Nes6502::DEX() {
  x--;
  SetFlag(Z, x == 0x00);
  SetFlag(N, x & 0x80);
  return 0;
}

// Instruction: Decrement Y Register
// Function:    Y = Y - 1
// Flags Out:   N, Z
uint8_t Nes6502::DEY() {
  y--;
  SetFlag(Z, y == 0x00);
  SetFlag(N, y & 0x80);
  return 0;
}

// Instruction: Bitwise Logic XOR
// Function:    A = A xor M
// Flags Out:   N, Z
uint8_t Nes6502::EOR() {
  fetch();
  a = a ^ fetched;
  SetFlag(Z, a == 0x00);
  SetFlag(N, a & 0x80);
  return 1;
}

// Instruction: Increment Value at Memory Location
// Function:    M = M + 1
// Flags Out:   N, Z
uint8_t Nes6502::INC() {
  fetch();
  uint16_t t = fetched + 1;
  write(addr_abs, t & 0x00FF);
  SetFlag(Z, (t & 0x00FF) == 0x0000);
  SetFlag(N, t & 0x0080);
  return 0;
}

// Instruction: Increment X Register
// Function:    X = X + 1
// Flags Out:   N, Z
uint8_t Nes6502::INX() {
  x++;
  SetFlag(Z, x == 0x00);
  SetFlag(N, x & 0x80);
  return 0;
}

// Instruction: Increment Y Register
// Function:    Y = Y + 1
// Flags Out:   N, Z
uint8_t Nes6502::INY() {
  y++;
  SetFlag(Z, y == 0x00);
  SetFlag(N, y & 0x80);
  return 0;
}

// Instruction: Jump To Location
// Function:    pc = address
uint8_t Nes6502::JMP() {
  pc = addr_abs;
  return 0;
}

// Instruction: Jump To Sub-Routine
// Function:    Push current pc to stack, pc = address
uint8_t Nes6502::JSR() {
  pc--;

  write(0x0100 + stkp, (pc >> 8) & 0x00FF);
  stkp--;
  write(0x0100 + stkp, pc & 0x00FF);
  stkp--;

  pc = addr_abs;
  return 0;
}

// Instruction: Load The Accumulator
// Function:    A = M
// Flags Out:   N, Z
uint8_t Nes6502::LDA() {
  fetch();
  a = fetched;
  SetFlag(Z, a == 0x00);
  SetFlag(N, a & 0x80);
  return 1;
}

// Instruction: Load The X Register
// Function:    X = M
// Flags Out:   N, Z
uint8_t Nes6502::LDX() {
  fetch();
  x = fetched;
  SetFlag(Z, x == 0x00);
  SetFlag(N, x & 0x80);
  return 1;
}

// Instruction: Load The Y Register
// Function:    Y = M
// Flags Out:   N, Z
uint8_t Nes6502::LDY() {
  fetch();
  y = fetched;
  SetFlag(Z, y == 0x00);
  SetFlag(N, y & 0x80);
  return 1;
}

// Instruction: Logical Shift Right
// Function:    A = 0 -> (A >> 1) -> C
// Flags Out:   N, Z, C
uint8_t Nes6502::LSR() {
  fetch();
  SetFlag(C, fetched & 0x0001);
  uint16_t t = fetched >> 1;
  SetFlag(Z, (t & 0x00FF) == 0x0000);
  SetFlag(N, t & 0x0080);
  StoreALU(t & 0x00FF);
  return 0;
}

uint8_t Nes6502::NOP() {
  // Not all NOPs are equal: some unofficial ones can take an extra cycle
  switch (opcode) {
    case 0x1C:
    case 0x3C:
    case 0x5C:
    case 0x7C:
    case 0xDC:
    case 0xFC:
      return 1;
  }
  return 0;
}

// Instruction: Bitwise Logic OR
// Function:    A = A | M
// Flags Out:   N, Z
uint8_t Nes6502::ORA() {
  fetch();
  a = a | fetched;
  SetFlag(Z, a == 0x00);
  SetFlag(N, a & 0x80);
  return 1;
}

// Instruction: Push Accumulator to Stack
// Function:    A -> stack
uint8_t Nes6502::PHA() {
  write(0x0100 + stkp, a);
  stkp--;
  return 0;
}

// Instruction: Push Status Register to Stack
// Function:    status -> stack
// Note:        Break flag is set to 1 before push
uint8_t Nes6502::PHP() {
  write(0x0100 + stkp, status | B | U);
  SetFlag(B, 0);
  SetFlag(U, 0);
  stkp--;
  return 0;
}

// Instruction: Pop Accumulator off Stack
// Function:    A <- stack
// Flags Out:   N, Z
uint8_t Nes6502::PLA() {
  stkp++;
  a = read(0x0100 + stkp);
  SetFlag(Z, a == 0x00);
  SetFlag(N, a & 0x80);
  return 0;
}

// Instruction: Pop Status Register off Stack
// Function:    Status <- stack
uint8_t Nes6502::PLP() {
  stkp++;
  status = read(0x0100 + stkp);
  SetFlag(U, 1);
  return 0;
}

// Instruction: Rotate Left
// Function:    A = C <- (A << 1) <- C
// Flags Out:   N, Z, C
uint8_t Nes6502::ROL() {
  fetch();
  uint16_t t = (uint16_t)(fetched << 1) | GetFlag(C);
  SetFlag(C, t & 0xFF00);
  SetFlag(Z, (t & 0x00FF) == 0x0000);
  SetFlag(N, t & 0x0080);
  StoreALU(t & 0x00FF);
  return 0;
}

// Instruction: Rotate Right
// Function:    A = C -> (A >> 1) -> C
// Flags Out:   N, Z, C
uint8_t Nes6502::ROR() {
  fetch();
  uint16_t t = (uint16_t)(GetFlag(C) << 7) | (fetched >> 1);
  SetFlag(C, fetched & 0x01);
  SetFlag(Z, (t & 0x00FF) == 0x00);
  SetFlag(N, t & 0x0080);
  StoreALU(t & 0x00FF);
  return 0;
}

// Instruction: Return from Interrupt
// Function:    Status <- stack, pc <- stack
uint8_t Nes6502::RTI() {
  stkp++;
  status = read(0x0100 + stkp);
  status &= ~B;
  status &= ~U;

  stkp++;
  pc = (uint16_t)read(0x0100 + stkp);
  stkp++;
  pc |= (uint16_t)read(0x0100 + stkp) << 8;
  return 0;
}

// Instruction: Return from Sub-Routine
// Function:    pc <- stack, pc + 1
uint8_t Nes6502::RTS() {
  stkp++;
  pc = (uint16_t)read(0x0100 + stkp);
  stkp++;
  pc |= (uint16_t)read(0x0100 + stkp) << 8;

  pc++;
  return 0;
}

// Instruction: Set Carry Flag
uint8_t Nes6502::SEC() {
  SetFlag(C, true);
  return 0;
}

// Instruction: Set Decimal Flag
uint8_t Nes6502::SED() {
  SetFlag(D, true);
  return 0;
}

// Instruction: Set Interrupt Flag / Disable Interrupts
uint8_t Nes6502::SEI() {
  SetFlag(I, true);
  return 0;
}

// Instruction: Store Accumulator at Address
// Function:    M = A
uint8_t Nes6502::STA() {
  write(addr_abs, a);
  return 0;
}

// Instruction: Store X Register at Address
// Function:    M = X
uint8_t Nes6502::STX() {
  write(addr_abs, x);
  return 0;
}

// Instruction: Store Y Register at Address
// Function:    M = Y
uint8_t Nes6502::STY() {
  write(addr_abs, y);
  return 0;
}

// Instruction: Transfer Accumulator to X Register
// Flags Out:   N, Z
uint8_t Nes6502::TAX() {
  x = a;
  SetFlag(Z, x == 0x00);
  SetFlag(N, x & 0x80);
  return 0;
}

// Instruction: Transfer Accumulator to Y Register
// Flags Out:   N, Z
uint8_t Nes6502::TAY() {
  y = a;
  SetFlag(Z, y == 0x00);
  SetFlag(N, y & 0x80);
  return 0;
}

// Instruction: Transfer Stack Pointer to X Register
// Flags Out:   N, Z
uint8_t Nes6502::TSX() {
  x = stkp;
  SetFlag(Z, x == 0x00);
  SetFlag(N, x & 0x80);
  return 0;
}

// Instruction: Transfer X Register to Accumulator
// Flags Out:   N, Z
uint8_t Nes6502::TXA() {
  a = x;
  SetFlag(Z, a == 0x00);
  SetFlag(N, a & 0x80);
  return 0;
}

// Instruction: Transfer X Register to Stack Pointer
uint8_t Nes6502::TXS() {
  stkp = x;
  return 0;
}

// Instruction: Transfer Y Register to Accumulator
// Flags Out:   N, Z
uint8_t Nes6502::TYA() {
  a = y;
  SetFlag(Z, a == 0x00);
  SetFlag(N, a & 0x80);
  return 0;
}

// This function captures illegal opcodes
uint8_t Nes6502::XXX() { return 0; }

///////////////////////////////////////////////////////////////////////////////
// HELPER FUNCTIONS

bool Nes6502::complete() { return cycles == 0; }

void Nes6502::SaveState(std::ostream &os) {
  VisitState([&os](auto &v) { PutPod(os, v); });
}

void Nes6502::LoadState(std::istream &is) {
  VisitState([&is](auto &v) { GetPod(is, v); });
}

// This is the disassembly function. Its workings are not required for
// emulation. It is merely a convenience function to turn the binary instruction
// code into human readable form. Its included as part of the emulator because
// it can take advantage of many of the CPUs internal operations to do this.
std::map<uint16_t, std::string> Nes6502::disassemble(uint16_t nStart,
                                                     uint16_t nStop) {
  uint32_t addr = nStart;
  uint8_t value = 0x00, lo = 0x00, hi = 0x00;
  std::map<uint16_t, std::string> mapLines;
  uint16_t line_addr = 0;

  // A convenient utility to convert variables into
  // hex strings because "modern C++"'s method with
  // streams is atrocious
  auto hex = [](uint32_t n, uint8_t d) {
    std::string s(d, '0');
    for (int i = d - 1; i >= 0; i--, n >>= 4)
      s[i] = "0123456789ABCDEF"[n & 0xF];
    return s;
  };

  // Starting at the specified address we read an instruction
  // byte, which in turn yields information from the lookup table
  // as to how many additional bytes we need to read and what the
  // addressing mode is. I need this info to assemble human readable
  // syntax, which is different depending upon the addressing mode

  // As the instruction is decoded, a std::string is assembled
  // with the readable output
  while (addr <= (uint32_t)nStop) {
    line_addr = addr;

    // Prefix line with instruction address
    std::string sInst = "$" + hex(addr, 4) + ": ";

    // Read instruction, and get its readable name
    uint8_t opcode = bus->cpuRead(addr, true);
    addr++;
    sInst += lookup[opcode].name;
    sInst += " ";

    // Get oprands from desired locations, and form the
    // instruction based upon its addressing mode. These
    // routines mimmick the actual fetch routine of the
    // 6502 in order to get accurate data as part of the
    // instruction
    switch (lookup[opcode].mode) {
      case AM::IMP:
        sInst += " {IMP}";
        break;
      case AM::IMM:
        value = bus->cpuRead(addr, true);
        addr++;
        sInst += "#$" + hex(value, 2) + " {IMM}";
        break;
      case AM::ZP0:
        lo = bus->cpuRead(addr, true);
        addr++;
        sInst += "$" + hex(lo, 2) + " {ZP0}";
        break;
      case AM::ZPX:
        lo = bus->cpuRead(addr, true);
        addr++;
        sInst += "$" + hex(lo, 2) + ", X {ZPX}";
        break;
      case AM::ZPY:
        lo = bus->cpuRead(addr, true);
        addr++;
        sInst += "$" + hex(lo, 2) + ", Y {ZPY}";
        break;
      case AM::IZX:
        lo = bus->cpuRead(addr, true);
        addr++;
        sInst += "($" + hex(lo, 2) + ", X) {IZX}";
        break;
      case AM::IZY:
        lo = bus->cpuRead(addr, true);
        addr++;
        sInst += "($" + hex(lo, 2) + "), Y {IZY}";
        break;
      case AM::ABS:
        lo = bus->cpuRead(addr, true);
        addr++;
        hi = bus->cpuRead(addr, true);
        addr++;
        sInst += "$" + hex((uint16_t)(hi << 8) | lo, 4) + " {ABS}";
        break;
      case AM::ABX:
        lo = bus->cpuRead(addr, true);
        addr++;
        hi = bus->cpuRead(addr, true);
        addr++;
        sInst += "$" + hex((uint16_t)(hi << 8) | lo, 4) + ", X {ABX}";
        break;
      case AM::ABY:
        lo = bus->cpuRead(addr, true);
        addr++;
        hi = bus->cpuRead(addr, true);
        addr++;
        sInst += "$" + hex((uint16_t)(hi << 8) | lo, 4) + ", Y {ABY}";
        break;
      case AM::IND:
        lo = bus->cpuRead(addr, true);
        addr++;
        hi = bus->cpuRead(addr, true);
        addr++;
        sInst += "($" + hex((uint16_t)(hi << 8) | lo, 4) + ") {IND}";
        break;
      case AM::REL:
        value = bus->cpuRead(addr, true);
        addr++;
        sInst += "$" + hex(value, 2) + " [$" + hex(addr + (int8_t)value, 4) +
                 "] {REL}";
        break;
    }

    // Add the formed string to a std::map, using the instruction's
    // address as the key. This makes it convenient to look for later
    // as the instructions are variable in length, so a straight up
    // incremental index is not sufficient.
    mapLines[line_addr] = sInst;
  }

  return mapLines;
}

// End of File - Jx9
