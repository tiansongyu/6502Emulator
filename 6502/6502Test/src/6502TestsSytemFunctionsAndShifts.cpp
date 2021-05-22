#include <gtest/gtest.h>
#include "main_6502.h"

class M6502Test1SystemAndShifts : public testing::Test
{
public:

	m6502::Mem mem;
	m6502::CPU cpu;
	virtual void SetUp()
	{
		cpu.Reset(mem);
	}

	virtual void TearDown()
	{
	}
	void CyclesConfirm(uint8_t* _register, const uint8_t& DataA, const int32_t Cycles, const int32_t RequestCycles)
	{
		int32_t CyclesUsed = cpu.Execute(Cycles, mem);
		EXPECT_EQ(*_register, DataA);
		EXPECT_EQ(CyclesUsed, RequestCycles);
	}
	void STCyclesConfirm(const uint8_t& _register, const uint8_t& DataA, const int32_t Cycles, const int32_t RequestCycles)
	{
		int32_t CyclesUsed = cpu.Execute(Cycles, mem);
		EXPECT_EQ(_register, DataA);
		EXPECT_EQ(CyclesUsed, RequestCycles);
	}
	void VerfiyCPUFlagCIDBV(const m6502::CPU& cpu, const m6502::CPU& CPUCopy)
	{
		EXPECT_EQ(cpu.Flags.C, CPUCopy.Flags.C);
		EXPECT_EQ(cpu.Flags.I, CPUCopy.Flags.I);
		EXPECT_EQ(cpu.Flags.D, CPUCopy.Flags.D);
		EXPECT_EQ(cpu.Flags.B, CPUCopy.Flags.B);
		EXPECT_EQ(cpu.Flags.V, CPUCopy.Flags.V);
	}
	void VerfiyCPUFlagALL(const m6502::CPU& cpu, const m6502::CPU& CPUCopy)
	{
		EXPECT_EQ(cpu.Flags.C, CPUCopy.Flags.C);
		EXPECT_EQ(cpu.Flags.I, CPUCopy.Flags.I);
		EXPECT_EQ(cpu.Flags.D, CPUCopy.Flags.D);
		EXPECT_EQ(cpu.Flags.B, CPUCopy.Flags.B);
		EXPECT_EQ(cpu.Flags.V, CPUCopy.Flags.V);
		EXPECT_EQ(cpu.Flags.Z, CPUCopy.Flags.Z);
		EXPECT_EQ(cpu.Flags.N, CPUCopy.Flags.N);
	}
};

TEST_F(M6502Test1SystemAndShifts, ASLIMM)
{
	cpu.A = 0b01000001;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_ASL_ACC;
	// end - inline a little program
	constexpr int CYCLES = 2;

	STCyclesConfirm(0b10000010, cpu.A, CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.C, false);
	EXPECT_EQ(cpu.Flags.N, true);
}
TEST_F(M6502Test1SystemAndShifts, ASLZP)
{
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_ASL_ZP;
	mem[0xFFFD] = 0x11;
	mem[0x0011] = 0b01000001;
	// end - inline a little program
	constexpr int CYCLES = 5;

	STCyclesConfirm(0b10000010, mem[0x0011], CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.C, false);
	EXPECT_EQ(cpu.Flags.N, true);
}

TEST_F(M6502Test1SystemAndShifts, ASLZPX)
{
	cpu.X = 0x3;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_ASL_ZPX;
	mem[0xFFFD] = 0x11;
	mem[0x0014] = 0b01000001;
	// end - inline a little program
	constexpr int CYCLES = 6;

	STCyclesConfirm(0b10000010, mem[0x0011], CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.C, false);
	EXPECT_EQ(cpu.Flags.N, true);
}
TEST_F(M6502Test1SystemAndShifts, ASLABS)
{
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_ASL_ABS;
	mem[0xFFFD] = 0x11;
	mem[0xFFFE] = 0x12;
	mem[0x1211] = 0b01000001;
	// end - inline a little program
	constexpr int CYCLES = 6;

	STCyclesConfirm(0b10000010, mem[0x1211], CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.C, false);
	EXPECT_EQ(cpu.Flags.N, true);
}

TEST_F(M6502Test1SystemAndShifts, ASLABSX)
{
	cpu.X = 0x5;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_ASL_ABSX;
	mem[0xFFFD] = 0x11;
	mem[0xFFFE] = 0x12;
	mem[0x1216] = 0b01000001;
	// end - inline a little program
	constexpr int CYCLES = 7;

	STCyclesConfirm(0b10000010, mem[0x1216], CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.C, false);
	EXPECT_EQ(cpu.Flags.N, true);
}
TEST_F(M6502Test1SystemAndShifts, LSRIMM)
{
	cpu.A = 0b01000001;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_LSR_ACC;
	// end - inline a little program
	constexpr int CYCLES = 2;

	STCyclesConfirm(0b00100000, cpu.A, CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.C, true);
	EXPECT_EQ(cpu.Flags.N, false);
}
TEST_F(M6502Test1SystemAndShifts, LSRZP)
{
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_LSR_ZP;
	mem[0xFFFD] = 0x11;
	mem[0x0011] = 0b01000001;
	// end - inline a little program
	constexpr int CYCLES = 5;

	STCyclesConfirm(0b00100000, mem[0x0011], CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.C, true);
	EXPECT_EQ(cpu.Flags.N, false);
}

TEST_F(M6502Test1SystemAndShifts, LSRZPX)
{
	cpu.X = 0x3;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_LSR_ZPX;
	mem[0xFFFD] = 0x11;
	mem[0x0014] = 0b01000001;
	// end - inline a little program
	constexpr int CYCLES = 6;

	STCyclesConfirm(0b00100000, mem[0x0011], CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.C, true);
	EXPECT_EQ(cpu.Flags.N, false);
}
TEST_F(M6502Test1SystemAndShifts, LSRABS)
{
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_LSR_ABS;
	mem[0xFFFD] = 0x11;
	mem[0xFFFE] = 0x12;
	mem[0x1211] = 0b01000001;
	// end - inline a little program
	constexpr int CYCLES = 6;

	STCyclesConfirm(0b00100000, mem[0x1211], CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.C, true);
	EXPECT_EQ(cpu.Flags.N, false);
}

TEST_F(M6502Test1SystemAndShifts, LSRABSX)
{
	cpu.X = 0x5;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_LSR_ABSX;
	mem[0xFFFD] = 0x11;
	mem[0xFFFE] = 0x12;
	mem[0x1216] = 0b01000001;
	// end - inline a little program
	constexpr int CYCLES = 7;

	STCyclesConfirm(0b00100000, mem[0x1216], CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.C, true);
	EXPECT_EQ(cpu.Flags.N, false);
}
TEST_F(M6502Test1SystemAndShifts, RORIMM)
{
	cpu.Flags.C = true;
	cpu.A = 0b01000001;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_ROR_ACC;
	// end - inline a little program
	constexpr int CYCLES = 2;

	STCyclesConfirm(0b10100000, cpu.A, CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.C, true);
	EXPECT_EQ(cpu.Flags.N, true);
}
TEST_F(M6502Test1SystemAndShifts, RORZP)
{
	cpu.Flags.C = true;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_ROR_ZP;
	mem[0xFFFD] = 0x11;
	mem[0x0011] = 0b01000001;
	// end - inline a little program
	constexpr int CYCLES = 5;

	STCyclesConfirm(0b10100000, mem[0x0011], CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.C, true);
	EXPECT_EQ(cpu.Flags.N, true);
}

TEST_F(M6502Test1SystemAndShifts, RORZPX)
{
	cpu.Flags.C = true;
	cpu.X = 0x3;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_ROR_ZPX;
	mem[0xFFFD] = 0x11;
	mem[0x0014] = 0b01000001;
	// end - inline a little program
	constexpr int CYCLES = 6;

	STCyclesConfirm(0b10100000, mem[0x0011], CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.C, true);
	EXPECT_EQ(cpu.Flags.N, true);
}
TEST_F(M6502Test1SystemAndShifts, INS_ROR_ABS)
{
	cpu.Flags.C = true;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_ROR_ABS;
	mem[0xFFFD] = 0x11;
	mem[0xFFFE] = 0x12;
	mem[0x1211] = 0b01000001;
	// end - inline a little program
	constexpr int CYCLES = 6;

	STCyclesConfirm(0b10100000, mem[0x1211], CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.C, true);
	EXPECT_EQ(cpu.Flags.N, true);
}

TEST_F(M6502Test1SystemAndShifts, INS_ROR_ABSX)
{
	cpu.Flags.C = true;
	cpu.X = 0x5;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_ROR_ABSX;
	mem[0xFFFD] = 0x11;
	mem[0xFFFE] = 0x12;
	mem[0x1216] = 0b01000001;
	// end - inline a little program
	constexpr int CYCLES = 7;

	STCyclesConfirm(0b10100000, mem[0x1216], CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.C, true);
	EXPECT_EQ(cpu.Flags.N, true);
}

TEST_F(M6502Test1SystemAndShifts, ROLIMM)
{
	cpu.Flags.C = true;
	cpu.A = 0b01000001;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_ROL_ACC;
	// end - inline a little program
	constexpr int CYCLES = 2;

	STCyclesConfirm(0b10000011, cpu.A, CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.C, false);
	EXPECT_EQ(cpu.Flags.N, true);
}
TEST_F(M6502Test1SystemAndShifts, ROLZP)
{
	cpu.Flags.C = true;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_ROL_ZP;
	mem[0xFFFD] = 0x11;
	mem[0x0011] = 0b01000001;
	// end - inline a little program
	constexpr int CYCLES = 5;

	STCyclesConfirm(0b10000011, mem[0x0011], CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.C, false);
	EXPECT_EQ(cpu.Flags.N, true);
}

TEST_F(M6502Test1SystemAndShifts, ROLZPX)
{
	cpu.Flags.C = true;
	cpu.X = 0x3;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_ROL_ZPX;
	mem[0xFFFD] = 0x11;
	mem[0x0014] = 0b01000001;
	// end - inline a little program
	constexpr int CYCLES = 6;

	STCyclesConfirm(0b10000011, mem[0x0011], CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.C, false);
	EXPECT_EQ(cpu.Flags.N, true);
}
TEST_F(M6502Test1SystemAndShifts, INS_ROL_ABS)
{
	cpu.Flags.C = true;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_ROL_ABS;
	mem[0xFFFD] = 0x11;
	mem[0xFFFE] = 0x12;
	mem[0x1211] = 0b01000001;
	// end - inline a little program
	constexpr int CYCLES = 6;

	STCyclesConfirm(0b10000011, mem[0x1211], CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.C, false);
	EXPECT_EQ(cpu.Flags.N, true);
}

TEST_F(M6502Test1SystemAndShifts, INS_ROL_ABSX)
{
	cpu.Flags.C = true;
	cpu.X = 0x5;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_ROL_ABSX;
	mem[0xFFFD] = 0x11;
	mem[0xFFFE] = 0x12;
	mem[0x1216] = 0b01000001;
	// end - inline a little program
	constexpr int CYCLES = 7;

	STCyclesConfirm(0b10000011, mem[0x1216], CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.C, false);
	EXPECT_EQ(cpu.Flags.N, true);
}
TEST_F(M6502Test1SystemAndShifts, INS_Break)
{
	cpu.Flags.C = true;
	cpu.PC = 0xFF00;
	uint8_t SPCOPY = cpu.SP;
	// start - inline a little program
	mem[0xFF00] = m6502::CPU::INS_BRK_IM;
	mem[0xFFFE] = 0x11;
	mem[0xFFFF] = 0x12;
	// end - inline a little program
	constexpr int CYCLES = 7;

	STCyclesConfirm(1, 1, CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.B, true);
	EXPECT_EQ(cpu.PC, 0x1211);
	EXPECT_EQ(SPCOPY, cpu.SP + 3);
}
TEST_F(M6502Test1SystemAndShifts, INS_BreakAndRTI)
{
	cpu.Flags.C = true;
	cpu.PC = 0xFF00;
	cpu.X = 0x3;
	uint8_t SPCOPY = cpu.SP;
	uint8_t psCOPY = cpu.ps;
	// start - inline a little program
	mem[0xFF00] = m6502::CPU::INS_BRK_IM;
	mem[0xFFFE] = 0x11;
	mem[0xFFFF] = 0x12;
	mem[0x1211] = m6502::CPU::INS_INX_IM;
	mem[0x1212] = m6502::CPU::INS_RTI_IM;
	// end - inline a little program
	constexpr int CYCLES = 7 + 6 + 2;

	STCyclesConfirm(1, 1, CYCLES, CYCLES);
	EXPECT_EQ(psCOPY, cpu.ps);
	EXPECT_EQ(SPCOPY, cpu.SP);
	EXPECT_EQ(cpu.PC, 0xFF02);
	EXPECT_EQ(cpu.X, 4);
}

