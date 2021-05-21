#include <gtest/gtest.h>
#include "main_6502.h"

class M6502Test1Arithmetic : public testing::Test
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

TEST_F(M6502Test1Arithmetic, ADCBranchTest)
{
	// given:
	// start - inline a little program
	cpu.A = 0x32;
	cpu.Flags.C = 1;
	cpu.PC = 0xFFF0;
	mem[0xFFF0] = m6502::CPU::INS_ADC_IM;
	mem[0xFFF1] = 0x45;
	// end - inline a little program
	constexpr int CYCLES = 2;

	STCyclesConfirm(cpu.A, 0x32 + 0x1 + 0x45, CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.N, false);
	EXPECT_EQ(cpu.Flags.V, false);
	EXPECT_EQ(cpu.Flags.C, false);
}

TEST_F(M6502Test1Arithmetic, ADCBranchOverFlowAndCarryTest)
{
	// given:
	// start - inline a little program
	cpu.A = 0b10000000;
	cpu.Flags.C = 0;
	cpu.PC = 0xFFF0;
	mem[0xFFF0] = m6502::CPU::INS_ADC_IM;
	mem[0xFFF1] = 0b11111111;
	// end - inline a little program
	constexpr int CYCLES = 2;

	STCyclesConfirm(cpu.A, 0b11111111 + 0x0 + 0b10000000, CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.N, false);
	EXPECT_EQ(cpu.Flags.V, true);
	EXPECT_EQ(cpu.Flags.C, true);
}

TEST_F(M6502Test1Arithmetic, ADCBranchJustOverFlowTest)
{
	// given:
	// start - inline a little program
	cpu.A = 0b00000000;
	cpu.Flags.C = 1;
	cpu.PC = 0xFFF0;
	mem[0xFFF0] = m6502::CPU::INS_ADC_IM;
	mem[0xFFF1] = 0b01111111;
	// end - inline a little program
	constexpr int CYCLES = 2;

	STCyclesConfirm(cpu.A, 0b00000000 + 0x1 + 0b01111111, CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.N, true);
	EXPECT_EQ(cpu.Flags.V, true);
	EXPECT_EQ(cpu.Flags.C, false);
}
TEST_F(M6502Test1Arithmetic, ADCBranchJustCarryTest)
{
	// given:
	// start - inline a little program
	cpu.A = 0b10000000;
	cpu.Flags.C = 1;
	cpu.PC = 0xFFF0;
	mem[0xFFF0] = m6502::CPU::INS_ADC_IM;
	mem[0xFFF1] = 0b11111111;
	// end - inline a little program
	constexpr int CYCLES = 2;

	STCyclesConfirm(cpu.A, 0b10000000 + 0x1 + 0b11111111, CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.N, true);
	EXPECT_EQ(cpu.Flags.V, false);
	EXPECT_EQ(cpu.Flags.C, true);
}

TEST_F(M6502Test1Arithmetic, ADCBranchZeroPageTest)
{
	// given:
	// start - inline a little program
	cpu.A = 0b10000000;
	cpu.Flags.C = 1;
	cpu.PC = 0xFFF0;
	mem[0xFFF0] = m6502::CPU::INS_ADC_ZP;
	mem[0xFFF1] = 0x02;
	mem[0x0002] = 0b11111111;
	// end - inline a little program
	constexpr int CYCLES = 3;

	STCyclesConfirm(cpu.A, 0b10000000 + 0x1 + 0b11111111, CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.N, true);
	EXPECT_EQ(cpu.Flags.V, false);
	EXPECT_EQ(cpu.Flags.C, true);
}
//TODO
TEST_F(M6502Test1Arithmetic, ADCBranchZeroPageXTest)
{
	// given:
	// start - inline a little program
	cpu.X = 0x3;
	cpu.A = 0b10000000;
	cpu.Flags.C = 1;
	cpu.PC = 0xFFF0;
	mem[0xFFF0] = m6502::CPU::INS_ADC_ZPX;
	mem[0xFFF1] = 0x02;
	mem[0x0005] = 0b11111111;
	// end - inline a little program
	constexpr int CYCLES = 4;

	STCyclesConfirm(cpu.A, 0b10000000 + 0x1 + 0b11111111, CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.N, true);
	EXPECT_EQ(cpu.Flags.V, false);
	EXPECT_EQ(cpu.Flags.C, true);
}
//TODO
TEST_F(M6502Test1Arithmetic, ADCBranchAbsoluteTest)
{
	// given:
	// start - inline a little program
	cpu.A = 0b10000000;
	cpu.Flags.C = 1;
	cpu.PC = 0xFFF0;
	mem[0xFFF0] = m6502::CPU::INS_ADC_ABS;
	mem[0xFFF1] = 0x02;
	mem[0xFFF2] = 0x03;
	mem[0x0302] = 0b11111111;
	// end - inline a little program
	constexpr int CYCLES = 4;

	STCyclesConfirm(cpu.A, 0b10000000 + 0x1 + 0b11111111, CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.N, true);
	EXPECT_EQ(cpu.Flags.V, false);
	EXPECT_EQ(cpu.Flags.C, true);
}
//TODO
TEST_F(M6502Test1Arithmetic, ADCBranchAbsoluteXTest)
{
	// given:
	// start - inline a little program
	cpu.X = 0x3;
	cpu.A = 0b10000000;
	cpu.Flags.C = 1;
	cpu.PC = 0xFFF0;
	mem[0xFFF0] = m6502::CPU::INS_ADC_ABS_X;
	mem[0xFFF1] = 0x02;
	mem[0xFFF2] = 0x03;
	mem[0x0305] = 0b11111111;
	// end - inline a little program
	constexpr int CYCLES = 4;

	STCyclesConfirm(cpu.A, 0b10000000 + 0x1 + 0b11111111, CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.N, true);
	EXPECT_EQ(cpu.Flags.V, false);
	EXPECT_EQ(cpu.Flags.C, true);
}
//TODO
TEST_F(M6502Test1Arithmetic, ADCBranchAbsoluteYTest)
{
	// given:
	// start - inline a little program
	cpu.Y = 0x3;
	cpu.A = 0b10000000;
	cpu.Flags.C = 1;
	cpu.PC = 0xFFF0;
	mem[0xFFF0] = m6502::CPU::INS_ADC_ABS_Y;
	mem[0xFFF1] = 0x02;
	mem[0xFFF2] = 0x03;
	mem[0x0305] = 0b11111111;
	// end - inline a little program
	constexpr int CYCLES = 4;

	STCyclesConfirm(cpu.A, 0b10000000 + 0x1 + 0b11111111, CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.N, true);
	EXPECT_EQ(cpu.Flags.V, false);
	EXPECT_EQ(cpu.Flags.C, true);
}
//TODO
TEST_F(M6502Test1Arithmetic, ADCBranchIndirectXTest)
{
	// given:
	// start - inline a little program
	cpu.X = 0x3;
	cpu.A = 0b10000000;
	cpu.Flags.C = 1;
	cpu.PC = 0xFFF0;
	mem[0xFFF0] = m6502::CPU::INS_ADC_IND_X;
	mem[0xFFF1] = 0x02;
	mem[0x0005] = 0x03;
	mem[0x0006] = 0x04;
	mem[0x0403] = 0b11111111;
	// end - inline a little program
	constexpr int CYCLES = 6;

	STCyclesConfirm(cpu.A, 0b10000000 + 0x1 + 0b11111111, CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.N, true);
	EXPECT_EQ(cpu.Flags.V, false);
	EXPECT_EQ(cpu.Flags.C, true);
}
//TODO
TEST_F(M6502Test1Arithmetic, ADCBranchIndirectYTest)
{
	// given:
	// start - inline a little program
	cpu.Y = 0x3;
	cpu.A = 0b10000000;
	cpu.Flags.C = 1;
	cpu.PC = 0xFFF0;
	mem[0xFFF0] = m6502::CPU::INS_ADC_IND_Y;
	mem[0xFFF1] = 0x02;
	mem[0x0002] = 0x03;
	mem[0x0003] = 0x04;
	mem[0x0406] = 0b11111111;
	// end - inline a little program
	constexpr int CYCLES = 5;

	STCyclesConfirm(cpu.A, 0b10000000 + 0x1 + 0b11111111, CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.N, true);
	EXPECT_EQ(cpu.Flags.V, false);
	EXPECT_EQ(cpu.Flags.C, true);
}
//TODO

///////////////////////////
TEST_F(M6502Test1Arithmetic, SBCBranchTest)
{
	// given:
	// start - inline a little program
	cpu.A = 0x32;
	cpu.Flags.C = 1;
	cpu.PC = 0xFFF0;
	mem[0xFFF0] = m6502::CPU::INS_SBC_IM;
	mem[0xFFF1] = 0x11;
	// end - inline a little program
	constexpr int CYCLES = 2;

	STCyclesConfirm(cpu.A, 0x32 - 0x1 - 0x11, CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.N, false);
	EXPECT_EQ(cpu.Flags.V, false);
	EXPECT_EQ(cpu.Flags.C, false);
}

TEST_F(M6502Test1Arithmetic, SBCBranchOverFlowAndCarryTest)
{
	// given:
	// start - inline a little program
	cpu.A = 0b11111111;
	cpu.Flags.C = 0;
	cpu.PC = 0xFFF0;
	mem[0xFFF0] = m6502::CPU::INS_SBC_IM;
	mem[0xFFF1] = 0b10000000;
	// end - inline a little program
	constexpr int CYCLES = 2;

	STCyclesConfirm(cpu.A, 0b11111111 - 0x0 - 0b10000000, CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.N, false);
	EXPECT_EQ(cpu.Flags.V, true);
	EXPECT_EQ(cpu.Flags.C, true);
}

TEST_F(M6502Test1Arithmetic, SBCBranchJustOverFlowTest)
{
	// given:
	// start - inline a little program
	cpu.A = 0b00000000;
	cpu.Flags.C = 1;
	cpu.PC = 0xFFF0;
	mem[0xFFF0] = m6502::CPU::INS_SBC_IM;
	mem[0xFFF1] = 0b01111111;
	// end - inline a little program
	constexpr int CYCLES = 2;

	STCyclesConfirm(cpu.A, 0b00000000 - 0x1 - 0b01111111, CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.N, true);
	EXPECT_EQ(cpu.Flags.V, true);
	EXPECT_EQ(cpu.Flags.C, false);
}
TEST_F(M6502Test1Arithmetic, SBCBranchJustCarryTest)
{
	// given:
	// start - inline a little program
	cpu.A = 0b10000000;
	cpu.Flags.C = 1;
	cpu.PC = 0xFFF0;
	mem[0xFFF0] = m6502::CPU::INS_SBC_IM;
	mem[0xFFF1] = 0b11111111;
	// end - inline a little program
	constexpr int CYCLES = 2;

	STCyclesConfirm(cpu.A, 0b10000000 - 0x1 - 0b11111111, CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.N, true);
	EXPECT_EQ(cpu.Flags.V, false);
	EXPECT_EQ(cpu.Flags.C, true);
}

TEST_F(M6502Test1Arithmetic, SBCBranchZeroPageTest)
{
	// given:
	// start - inline a little program
	cpu.A = 0b10000000;
	cpu.Flags.C = 1;
	cpu.PC = 0xFFF0;
	mem[0xFFF0] = m6502::CPU::INS_SBC_ZP;
	mem[0xFFF1] = 0x02;
	mem[0x0002] = 0b11111111;
	// end - inline a little program
	constexpr int CYCLES = 3;

	STCyclesConfirm(cpu.A, 0b10000000 - 0x1 - 0b11111111, CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.N, true);
	EXPECT_EQ(cpu.Flags.V, false);
	EXPECT_EQ(cpu.Flags.C, true);
}
//TODO
TEST_F(M6502Test1Arithmetic, SBCBranchZeroPageXTest)
{
	// given:
	// start - inline a little program
	cpu.X = 0x3;
	cpu.A = 0b10000000;
	cpu.Flags.C = 1;
	cpu.PC = 0xFFF0;
	mem[0xFFF0] = m6502::CPU::INS_SBC_ZPX;
	mem[0xFFF1] = 0x02;
	mem[0x0005] = 0b11111111;
	// end - inline a little program
	constexpr int CYCLES = 4;

	STCyclesConfirm(cpu.A, 0b10000000 - 0x1 - 0b11111111, CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.N, true);
	EXPECT_EQ(cpu.Flags.V, false);
	EXPECT_EQ(cpu.Flags.C, true);
}
//TODO
TEST_F(M6502Test1Arithmetic, SBCBranchAbsoluteTest)
{
	// given:
	// start - inline a little program
	cpu.A = 0b10000000;
	cpu.Flags.C = 1;
	cpu.PC = 0xFFF0;
	mem[0xFFF0] = m6502::CPU::INS_SBC_ABS;
	mem[0xFFF1] = 0x02;
	mem[0xFFF2] = 0x03;
	mem[0x0302] = 0b11111111;
	// end - inline a little program
	constexpr int CYCLES = 4;

	STCyclesConfirm(cpu.A, 0b10000000 - 0x1 - 0b11111111, CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.N, true);
	EXPECT_EQ(cpu.Flags.V, false);
	EXPECT_EQ(cpu.Flags.C, true);
}
//TODO
TEST_F(M6502Test1Arithmetic, SBCBranchAbsoluteXTest)
{
	// given:
	// start - inline a little program
	cpu.X = 0x3;
	cpu.A = 0b10000000;
	cpu.Flags.C = 1;
	cpu.PC = 0xFFF0;
	mem[0xFFF0] = m6502::CPU::INS_SBC_ABS_X;
	mem[0xFFF1] = 0x02;
	mem[0xFFF2] = 0x03;
	mem[0x0305] = 0b11111111;
	// end - inline a little program
	constexpr int CYCLES = 4;

	STCyclesConfirm(cpu.A, 0b10000000 - 0x1 - 0b11111111, CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.N, true);
	EXPECT_EQ(cpu.Flags.V, false);
	EXPECT_EQ(cpu.Flags.C, true);
}
//TODO
TEST_F(M6502Test1Arithmetic, SBCBranchAbsoluteYTest)
{
	// given:
	// start - inline a little program
	cpu.Y = 0x3;
	cpu.A = 0b10000000;
	cpu.Flags.C = 1;
	cpu.PC = 0xFFF0;
	mem[0xFFF0] = m6502::CPU::INS_SBC_ABS_Y;
	mem[0xFFF1] = 0x02;
	mem[0xFFF2] = 0x03;
	mem[0x0305] = 0b11111111;
	// end - inline a little program
	constexpr int CYCLES = 4;

	STCyclesConfirm(cpu.A, 0b10000000 - 0x1 - 0b11111111, CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.N, true);
	EXPECT_EQ(cpu.Flags.V, false);
	EXPECT_EQ(cpu.Flags.C, true);
}
//TODO
TEST_F(M6502Test1Arithmetic, SBCBranchIndirectXTest)
{
	// given:
	// start - inline a little program
	cpu.X = 0x3;
	cpu.A = 0b10000000;
	cpu.Flags.C = 1;
	cpu.PC = 0xFFF0;
	mem[0xFFF0] = m6502::CPU::INS_SBC_IND_X;
	mem[0xFFF1] = 0x02;
	mem[0x0005] = 0x03;
	mem[0x0006] = 0x04;
	mem[0x0403] = 0b11111111;
	// end - inline a little program
	constexpr int CYCLES = 6;

	STCyclesConfirm(cpu.A, 0b10000000 - 0x1 - 0b11111111, CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.N, true);
	EXPECT_EQ(cpu.Flags.V, false);
	EXPECT_EQ(cpu.Flags.C, true);
}
//TODO
TEST_F(M6502Test1Arithmetic, SBCBranchIndirectYTest)
{
	// given:
	// start - inline a little program
	cpu.Y = 0x3;
	cpu.A = 0b10000000;
	cpu.Flags.C = 1;
	cpu.PC = 0xFFF0;
	mem[0xFFF0] = m6502::CPU::INS_SBC_IND_Y;
	mem[0xFFF1] = 0x02;
	mem[0x0002] = 0x03;
	mem[0x0003] = 0x04;
	mem[0x0406] = 0b11111111;
	// end - inline a little program
	constexpr int CYCLES = 5;

	STCyclesConfirm(cpu.A, 0b10000000 - 0x1 - 0b11111111, CYCLES, CYCLES);
	EXPECT_EQ(cpu.Flags.Z, false);
	EXPECT_EQ(cpu.Flags.N, true);
	EXPECT_EQ(cpu.Flags.V, false);
	EXPECT_EQ(cpu.Flags.C, true);
}
//TODO