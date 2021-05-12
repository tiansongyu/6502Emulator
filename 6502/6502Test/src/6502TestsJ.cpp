#include <gtest/gtest.h>
#include "main_6502.h"

class M6502Test1 : public testing::Test
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
		EXPECT_EQ(cpu.C, CPUCopy.C);
		EXPECT_EQ(cpu.I, CPUCopy.I);
		EXPECT_EQ(cpu.D, CPUCopy.D);
		EXPECT_EQ(cpu.B, CPUCopy.B);
		EXPECT_EQ(cpu.V, CPUCopy.V);
	}
	void VerfiyCPUFlagALL(const m6502::CPU& cpu, const m6502::CPU& CPUCopy)
	{
		EXPECT_EQ(cpu.C, CPUCopy.C);
		EXPECT_EQ(cpu.I, CPUCopy.I);
		EXPECT_EQ(cpu.D, CPUCopy.D);
		EXPECT_EQ(cpu.B, CPUCopy.B);
		EXPECT_EQ(cpu.V, CPUCopy.V);
		EXPECT_EQ(cpu.Z, CPUCopy.Z);
		EXPECT_EQ(cpu.N, CPUCopy.N);
	}
};

TEST_F(M6502Test1, JSRANDRTSTEST)
{
	// given:
	// start - inline a little program
	cpu.PC = 0xFFF0;
	mem[0xFFF0] = m6502::CPU::INS_JSR;
	mem[0xFFF1] = 0x42;
	mem[0xFFF2] = 0x37;
	mem[0x3742] = m6502::CPU::INS_RTS;
	mem[0xFFF3] = m6502::CPU::INS_LDA_IM;
	mem[0xFFF4] = 0x23;
	// end - inline a little program
	constexpr int CYCLES = 6 + 6 + 2;

	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(cpu.A, 0x23, CYCLES, CYCLES);
}