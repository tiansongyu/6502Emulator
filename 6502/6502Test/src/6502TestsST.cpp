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

TEST_F(M6502Test1, STAZeroPageCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.A = 0x89;
	mem[0xFFFC] = m6502::CPU::INS_STA_ZP;
	mem[0xFFFD] = 0x03; // 0x03
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(mem[0x03], cpu.A, 3, 3);
	VerfiyCPUFlagALL(cpu, CPUCopy);
}
TEST_F(M6502Test1, STAZeroPageXCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.A = 0x89;
	cpu.X = 0x04;
	mem[0xFFFC] = m6502::CPU::INS_STA_ZPX;
	mem[0xFFFD] = 0x03; // 0x03
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(mem[0x07], cpu.A, 4, 4);
	VerfiyCPUFlagALL(cpu, CPUCopy);
}

TEST_F(M6502Test1, STAABSCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.A = 0x89;
	mem[0xFFFC] = m6502::CPU::INS_STA_ABS;
	mem[0xFFFD] = 0x03; // 0x03
	mem[0xFFFE] = 0x04;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(mem[0x0403], cpu.A, 4, 4);
	VerfiyCPUFlagALL(cpu, CPUCopy);
}
TEST_F(M6502Test1, STAABSXCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.X = 0x05;
	cpu.A = 0x89;
	mem[0xFFFC] = m6502::CPU::INS_STA_ABS_X;
	mem[0xFFFD] = 0x03; // 0x03
	mem[0xFFFE] = 0x04;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(mem[0x0408], cpu.A, 5, 5);
	VerfiyCPUFlagALL(cpu, CPUCopy);
}
TEST_F(M6502Test1, STAABSYCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.Y = 0x05;
	cpu.A = 0x89;
	mem[0xFFFC] = m6502::CPU::INS_STA_ABS_Y;
	mem[0xFFFD] = 0x03; // 0x03
	mem[0xFFFE] = 0x04;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(mem[0x0408], cpu.A, 5, 5);
	VerfiyCPUFlagALL(cpu, CPUCopy);
}
TEST_F(M6502Test1, STAINDXCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.X = 0x05;
	cpu.A = 0x89;
	mem[0xFFFC] = m6502::CPU::INS_STA_IND_X;
	mem[0xFFFD] = 0x03; // 0x03
	mem[0x0008] = 0x56;
	mem[0x0009] = 0x67;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(mem[0x6756], cpu.A, 6, 6);
	VerfiyCPUFlagALL(cpu, CPUCopy);
}

TEST_F(M6502Test1, STAINDYCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.Y = 0x05;
	cpu.A = 0x89;
	mem[0xFFFC] = m6502::CPU::INS_STA_IND_Y;
	mem[0xFFFD] = 0x03; // 0x03
	mem[0x0003] = 0x56;
	mem[0x0004] = 0x67;

	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(mem[0x675B], cpu.A, 6, 6);
	VerfiyCPUFlagALL(cpu, CPUCopy);
}

TEST_F(M6502Test1, STXZeroPageCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.X = 0x04;
	mem[0xFFFC] = m6502::CPU::INS_STX_ZP;
	mem[0xFFFD] = 0x03; // 0x03
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(mem[0x03], cpu.X, 3, 3);
	VerfiyCPUFlagALL(cpu, CPUCopy);
}
TEST_F(M6502Test1, STXZeroPageYCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.X = 0x89;
	cpu.Y = 0x04;
	mem[0xFFFC] = m6502::CPU::INS_STX_ZPY;
	mem[0xFFFD] = 0x03; // 0x03
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(mem[0x07], cpu.X, 4, 4);
	VerfiyCPUFlagALL(cpu, CPUCopy);
}

TEST_F(M6502Test1, STXABSCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.X = 0x89;
	mem[0xFFFC] = m6502::CPU::INS_STX_ABS;
	mem[0xFFFD] = 0x03; // 0x03
	mem[0xFFFE] = 0x04;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(mem[0x0403], cpu.X, 4, 4);
	VerfiyCPUFlagALL(cpu, CPUCopy);
}

TEST_F(M6502Test1, STYZeroPageCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.Y = 0x04;
	mem[0xFFFC] = m6502::CPU::INS_STY_ZP;
	mem[0xFFFD] = 0x03; // 0x03
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(mem[0x03], cpu.Y, 3, 3);
	VerfiyCPUFlagALL(cpu, CPUCopy);
}
TEST_F(M6502Test1, STYZeroPageXCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.Y = 0x89;
	cpu.X = 0x04;
	mem[0xFFFC] = m6502::CPU::INS_STY_ZPX;
	mem[0xFFFD] = 0x03; // 0x03
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(mem[0x07], cpu.Y, 4, 4);
	VerfiyCPUFlagALL(cpu, CPUCopy);
}

TEST_F(M6502Test1, STYABSCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.Y = 0x89;
	mem[0xFFFC] = m6502::CPU::INS_STY_ABS;
	mem[0xFFFD] = 0x03; // 0x03
	mem[0xFFFE] = 0x04;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(mem[0x0403], cpu.Y, 4, 4);
	VerfiyCPUFlagALL(cpu, CPUCopy);
}