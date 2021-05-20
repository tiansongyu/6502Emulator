#include <gtest/gtest.h>
#include "main_6502.h"

class M6502Test1Branch : public testing::Test
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
	void CyclesConfirm(m6502::Byte* _register, const m6502::Byte& DataA, const m6502::s32 Cycles, const m6502::s32 RequestCycles)
	{
		m6502::s32 CyclesUsed = cpu.Execute(Cycles, mem);
		EXPECT_EQ(*_register, DataA);
		EXPECT_EQ(CyclesUsed, RequestCycles);
	}
	void STCyclesConfirm(const m6502::Byte& _register, const m6502::Byte& DataA, const m6502::s32 Cycles, const m6502::s32 RequestCycles)
	{
		m6502::s32 CyclesUsed = cpu.Execute(Cycles, mem);
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

TEST_F(M6502Test1Branch, BCCBranchTest)
{
	// given:
	// start - inline a little program
	cpu.X = 0x4;
	cpu.ps = ~CarryFlag;

	cpu.PC = 0xFFF0;
	mem[0xFFF0] = m6502::CPU::INS_BCC_REL;
	mem[0xFFF1] = 0x3;
	mem[0xFFF5] = m6502::CPU::INS_INX_IM;
	// end - inline a little program
	constexpr int CYCLES = 3 + 2;

	STCyclesConfirm(cpu.X, 0x5, CYCLES, CYCLES);
	EXPECT_EQ(cpu.PC, 0xFFF6);
}

TEST_F(M6502Test1Branch, BCCBranchTestCrossPage)
{
	// given:
	// start - inline a little program
	cpu.X = 0x4;
	cpu.ps = ~CarryFlag;

	cpu.PC = 0xF100;
	mem[0xF100] = m6502::CPU::INS_BCC_REL;
	mem[0xF101] = 0xff;
	mem[0xF201] = m6502::CPU::INS_INX_IM;
	// end - inline a little program
	constexpr int CYCLES = 4 + 2;

	STCyclesConfirm(cpu.X, 0x5, CYCLES, CYCLES);
	EXPECT_EQ(cpu.PC, 0xF202);
}


TEST_F(M6502Test1Branch, BCSBranchTest)
{
	// given:
	// start - inline a little program
	cpu.X = 0x4;
	cpu.ps = CarryFlag;

	cpu.PC = 0xFFF0;
	mem[0xFFF0] = m6502::CPU::INS_BCS_REL;
	mem[0xFFF1] = 0x3;
	mem[0xFFF5] = m6502::CPU::INS_INX_IM;
	// end - inline a little program
	constexpr int CYCLES = 3 + 2;

	STCyclesConfirm(cpu.X, 0x5, CYCLES, CYCLES);
	EXPECT_EQ(cpu.PC, 0xFFF6);
}

TEST_F(M6502Test1Branch, BCSBranchTestCrossPage)
{
	// given:
	// start - inline a little program
	cpu.X = 0x4;
	cpu.ps = CarryFlag;

	cpu.PC = 0xF100;
	mem[0xF100] = m6502::CPU::INS_BCS_REL;
	mem[0xF101] = 0xff;
	mem[0xF201] = m6502::CPU::INS_INX_IM;
	// end - inline a little program
	constexpr int CYCLES = 4 + 2;

	STCyclesConfirm(cpu.X, 0x5, CYCLES, CYCLES);
	EXPECT_EQ(cpu.PC, 0xF202);
}

