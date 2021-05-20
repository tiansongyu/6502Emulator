#include <gtest/gtest.h>
#include "main_6502.h"

class M6502Test1J : public testing::Test
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

TEST_F(M6502Test1J, JSRANDRTSTEST)
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

	uint8_t SPCopy = cpu.SP;
	STCyclesConfirm(cpu.A, 0x23, CYCLES, CYCLES);
	EXPECT_EQ(SPCopy, cpu.SP);
}
TEST_F(M6502Test1J, JMPANDRTSTEST)
{
	// given:
	cpu.A = 0x69;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_JMP;
	mem[0xFFFD] = 0x42;
	mem[0xFFFE] = 0X53;
	mem[0x5342] = m6502::CPU::INS_LDA_IM;
	mem[0x5343] = 0x67;
	// end - inline a little program
	constexpr int CYCLES = 3 + 2;

	uint8_t SPCopy = cpu.SP;
	STCyclesConfirm(cpu.A, 0x67, CYCLES, CYCLES);
	EXPECT_EQ(SPCopy, cpu.SP);
}
TEST_F(M6502Test1J, TSXTEST)
{
	// given:
	cpu.X = 0x69;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_TSX;
	
	// end - inline a little program
	constexpr int CYCLES = 2;
	STCyclesConfirm(cpu.X, cpu.SP, CYCLES, CYCLES);
}
TEST_F(M6502Test1J, TXSTEST)
{
	// given:
	cpu.X = 0x69;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_TXS;

	// end - inline a little program
	constexpr int CYCLES = 2;

	STCyclesConfirm(cpu.X, cpu.SP, CYCLES, CYCLES);
}
TEST_F(M6502Test1J, PHATEST)
{
	// given:
	cpu.A = 0x69;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_PHA;
	// end - inline a little program
	constexpr int CYCLES = 3;

	uint8_t SPCopy = cpu.SP;
	STCyclesConfirm(0x69, mem[0x100+ SPCopy], CYCLES, CYCLES);
	EXPECT_EQ(SPCopy-1, cpu.SP);

}
TEST_F(M6502Test1J, PHPANDstatusflagsTEST)
{
	// given:
	cpu.ps = 0b00001111;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_PHP;
	// end - inline a little program
	constexpr int CYCLES = 3;

	uint8_t SPCopy = cpu.SP;
	uint8_t psCopy = cpu.ps;
	STCyclesConfirm(cpu.ps, mem[0x100 + SPCopy], CYCLES, CYCLES);
	EXPECT_EQ(SPCopy - 1, cpu.SP);
}

TEST_F(M6502Test1J, PLAANDstatusflagsTEST)
{
	// given:
	m6502::CPU CPUCopy = cpu;
	cpu.SP = 0xFE;
	cpu.A = 0b01000100;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_PLA;
	mem[0x01ff] = 0b11110000;
	// end - inline a little program
	constexpr int CYCLES = 4;

	uint8_t SPCopy = cpu.SP;
	uint8_t psCopy = cpu.ps;
	STCyclesConfirm(cpu.A, 0b11110000, CYCLES, CYCLES);
	EXPECT_EQ(SPCopy + 1, cpu.SP);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);
}
TEST_F(M6502Test1J, PLPANDstatusflagsTEST)
{
	// given:
	m6502::CPU CPUCopy = cpu;
	cpu.SP = 0xFE;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_PLP;
	mem[0x01ff] = 0b11110000;
	// end - inline a little program
	constexpr int CYCLES = 4;

	uint8_t SPCopy = cpu.SP;
	uint8_t psCopy = cpu.ps;
	STCyclesConfirm(cpu.ps, 0b11110000, CYCLES, CYCLES);
}
TEST_F(M6502Test1J, TAXANDstatusflagsTEST)
{
	// given:
	cpu.X = 0b01001111;
	cpu.A = 0b10000000;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_TAX_IM;
	// end - inline a little program
	constexpr int CYCLES = 2;
	STCyclesConfirm(cpu.X, cpu.A, CYCLES, CYCLES);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
}
TEST_F(M6502Test1J, TAYANDstatusflagsTEST)
{
	// given:
	cpu.Y = 0b01001111;
	cpu.A = 0b10000000;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_TAY_IM;
	// end - inline a little program
	constexpr int CYCLES = 2;

	STCyclesConfirm(cpu.A, cpu.Y, CYCLES, CYCLES);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
}
TEST_F(M6502Test1J, TXAANDstatusflagsTEST)
{
	// given:
	cpu.X = 0b01001111;
	cpu.A = 0b10000000;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_TXA_IM;
	// end - inline a little program
	constexpr int CYCLES = 2;
	STCyclesConfirm(cpu.X, cpu.A, CYCLES, CYCLES);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
}
TEST_F(M6502Test1J, TYAANDstatusflagsTEST)
{
	// given:
	cpu.Y = 0b01001111;
	cpu.A = 0b10000000;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_TYA_IM;
	// end - inline a little program
	constexpr int CYCLES = 2;
	STCyclesConfirm(cpu.A, cpu.Y, CYCLES, CYCLES);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
}
TEST_F(M6502Test1J,INC_ZP_TEST)
{
	// given:

	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_INC_ZP;
	mem[0xFFFD] = 0x0002;
	mem[0x0002] = 0b10000000;
	// end - inline a little program
	constexpr int CYCLES = 5;
	STCyclesConfirm(0b10000001, mem[0x0002], CYCLES, CYCLES);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	
}

TEST_F(M6502Test1J, INC_ZPX_TEST)
{
	// given:
	cpu.X = 0x03;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_INC_ZPX;
	mem[0xFFFD] = 0x0002;
	mem[0x0005] = 0b10000000;
	// end - inline a little program
	constexpr int CYCLES = 6;
	STCyclesConfirm(0b10000001, mem[0x0005], CYCLES, CYCLES);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
}

TEST_F(M6502Test1J, INC_ABS_TEST)
{
	// given:
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_INC_ABS;
	mem[0xFFFD] = 0x02;
	mem[0xFFFE] = 0xFF;
	mem[0xFF02] = 0x31;
	// end - inline a little program
	constexpr int CYCLES = 6;
	STCyclesConfirm(0x32, mem[0xFF02], CYCLES, CYCLES);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
}

TEST_F(M6502Test1J, INC_ABSX_TEST)
{
	// given:
	cpu.X = 0x03;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_INC_ABSX;
	mem[0xFFFD] = 0x02;
	mem[0xFFFE] = 0xFF;
	mem[0xFF05] = 0x31;
	// end - inline a little program
	constexpr int CYCLES = 7;
	STCyclesConfirm(0x32, mem[0xFF05], CYCLES, CYCLES);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
}

TEST_F(M6502Test1J, INC_INX_IMM_TEST)
{
	// given:
	cpu.X = 0x03;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_INX_IM;
	// end - inline a little program
	constexpr int CYCLES = 2;
	STCyclesConfirm(cpu.X, 0x4, CYCLES, CYCLES);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
}
TEST_F(M6502Test1J, INC_INY_IMM_TEST)
{
	// given:
	cpu.Y = 0x03;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_INY_IM;
	// end - inline a little program
	constexpr int CYCLES = 2;
	STCyclesConfirm(cpu.Y, 0x4, CYCLES, CYCLES);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
}
////////////////////
TEST_F(M6502Test1J, DEC_ZP_TEST)
{
	// given:

	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_DEC_ZP;
	mem[0xFFFD] = 0x0002;
	mem[0x0002] = 0b10000000;
	// end - inline a little program
	constexpr int CYCLES = 5;
	STCyclesConfirm(0b01111111, mem[0x0002], CYCLES, CYCLES);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);

}

TEST_F(M6502Test1J, DEC_ZPX_TEST)
{
	// given:
	cpu.X = 0x03;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_DEC_ZPX;
	mem[0xFFFD] = 0x0002;
	mem[0x0005] = 0b10000000;
	// end - inline a little program
	constexpr int CYCLES = 6;
	STCyclesConfirm(0b01111111, mem[0x0005], CYCLES, CYCLES);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
}

TEST_F(M6502Test1J, DEC_ABS_TEST)
{
	// given:
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_DEC_ABS;
	mem[0xFFFD] = 0x02;
	mem[0xFFFE] = 0xFF;
	mem[0xFF02] = 0x31;
	// end - inline a little program
	constexpr int CYCLES = 6;
	STCyclesConfirm(0x30, mem[0xFF02], CYCLES, CYCLES);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
}

TEST_F(M6502Test1J, DEC_ABSX_TEST)
{
	// given:
	cpu.X = 0x03;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_DEC_ABSX;
	mem[0xFFFD] = 0x02;
	mem[0xFFFE] = 0xFF;
	mem[0xFF05] = 0x31;
	// end - inline a little program
	constexpr int CYCLES = 7;
	STCyclesConfirm(0x30, mem[0xFF05], CYCLES, CYCLES);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
}

TEST_F(M6502Test1J, DEX_IMM_TEST)
{
	// given:
	cpu.X = 0x03;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_DEX_IM;
	// end - inline a little program
	constexpr int CYCLES = 2;
	STCyclesConfirm(cpu.X, 0x02, CYCLES, CYCLES);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
}
TEST_F(M6502Test1J, DEY_IMM_TEST)
{
	// given:
	cpu.Y = 0x03;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_DEY_IM;
	// end - inline a little program
	constexpr int CYCLES = 2;
	STCyclesConfirm(cpu.Y, 0x2, CYCLES, CYCLES);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
}