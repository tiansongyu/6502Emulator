#include <gtest/gtest.h>
#include "main_6502.h"

class M6502Test1 : public testing::Test
{
public:
	Mem mem;
	CPU cpu;
	virtual void SetUp()
	{
		cpu.Reset( mem );
	}

	virtual void TearDown()
	{
	}
	void CyclesConfirm(CPU& cpu, const Byte& DataA, const s32 Cycles, const s32 RequestCycles)
	{
		s32 CyclesUsed = cpu.Execute(Cycles, mem);
		EXPECT_EQ(cpu.A, DataA);
		EXPECT_EQ(CyclesUsed, RequestCycles);
	}
	void VerfiyCPUFlagCIDBV(const CPU& cpu, const CPU& CPUCopy)
	{
		EXPECT_EQ(cpu.C, CPUCopy.C);
		EXPECT_EQ(cpu.I, CPUCopy.I);
		EXPECT_EQ(cpu.D, CPUCopy.D);
		EXPECT_EQ(cpu.B, CPUCopy.B);
		EXPECT_EQ(cpu.V, CPUCopy.V);
	}
};


TEST_F( M6502Test1, LDAImmediate_Can_Load_A_Value_Into_The_ARegister )
{
	// given:
	// start - inline a little program
	mem[0xFFFC] = CPU::INS_LDA_IM;
	mem[0xFFFD] = 0x84;
	// end - inline a little program

	CPU CPUCopy = cpu;
	CyclesConfirm(cpu, 0x84, 2, 2);
	EXPECT_FALSE( cpu.Z );
	EXPECT_TRUE( cpu.N );
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);
}

TEST_F( M6502Test1, LDAZeroPageCanLoadAValueIntoTheARegister )
{
	// given:
	// start - inline a little program
	mem[0xFFFC] = CPU::INS_LDA_ZP;
	mem[0xFFFD] = 0x42;
	mem[0x0042] = 0x37;
	// end - inline a little program

	CPU CPUCopy = cpu;
	CyclesConfirm(cpu, 0x37, 3, 3);
	EXPECT_FALSE( cpu.Z );
	EXPECT_FALSE( cpu.N );
	VerfiyCPUFlagCIDBV( cpu, CPUCopy);
}

TEST_F( M6502Test1, LDAZeroPageXCanLoadAValueIntoTheARegister )
{
	// given:
	cpu.X = 0xff;
	// start - inline a little program
	mem[0xFFFC] = CPU::INS_LDA_ZPX;
	mem[0xFFFD] = 0x42;
	mem[0x0041] = 0x37;
	// end - inline a little program

	CPU CPUCopy = cpu;
	CyclesConfirm(cpu, 0x37,4, 4);
	EXPECT_FALSE( cpu.Z );
	EXPECT_FALSE( cpu.N );
	VerfiyCPUFlagCIDBV( cpu, CPUCopy);

}
TEST_F(M6502Test1, LDAAbsoluteCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	mem[0xFFFC] = CPU::INS_LDA_ABS;
	mem[0xFFFD] = 0x42;
	mem[0xFFFE] = 0x37;
	mem[0x3742] = 0x89;
	// end - inline a little program

	CPU CPUCopy = cpu;
	CyclesConfirm(cpu, 0x89, 4, 4);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
	VerfiyCPUFlagCIDBV( cpu,  CPUCopy);

}
TEST_F(M6502Test1, LDAAbsoluteXCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.X = 1;
	mem[0xFFFC] = CPU::INS_LDA_ABS_X;
	mem[0xFFFD] = 0xf3; // 
	mem[0xFFFE] = 0x01;
	mem[0x01f4] = 0x89;
	// end - inline a little program
	CPU CPUCopy = cpu;
	CyclesConfirm(cpu, 0x89, 4,4);

	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
	VerfiyCPUFlagCIDBV( cpu, CPUCopy);

}
TEST_F(M6502Test1, LDAAbsoluteXCanLoadAValueIntoTheARegisterWhenItCrossThePage)
{
	// start - inline a little program
	cpu.X = 2;
	mem[0xFFFC] = CPU::INS_LDA_ABS_X;
	mem[0xFFFD] = 0xfe; // 0xfe - 0x101 跨页了需要使用5个周期
	mem[0xFFFE] = 0x01;
	mem[0x0200] = 0x89;
	// end - inline a little program

	CPU CPUCopy = cpu;
	CyclesConfirm(cpu, 0x89, 5, 5);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);

}
TEST_F(M6502Test1, LDAAbsoluteYCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.Y = 1;
	mem[0xFFFC] = CPU::INS_LDA_ABS_Y;
	mem[0xFFFD] = 0xf0;//
	mem[0xFFFE] = 0x01;
	mem[0x01F1] = 0x89;
	// end - inline a little program

	CPU CPUCopy = cpu;
	CyclesConfirm(cpu, 0x89, 4, 4);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
	VerfiyCPUFlagCIDBV(cpu,CPUCopy);

}
TEST_F(M6502Test1, LDAAbsoluteYCanLoadAValueIntoTheARegisterWhenItCrossThePage)
{
	// start - inline a little program
	cpu.Y = 1;
	mem[0xFFFC] = CPU::INS_LDA_ABS_Y;
	mem[0xFFFD] = 0xfe;//跨页了
	mem[0xFFFE] = 0x01;
	mem[0x01FF] = 0x89;
	// end - inline a little program

	CPU CPUCopy = cpu;
	CyclesConfirm(cpu, 0x89, 5, 5);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
	VerfiyCPUFlagCIDBV(cpu,CPUCopy);

}
TEST_F(M6502Test1, LDAInd_XCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.X = 1;
	mem[0xFFFC] = CPU::INS_LDA_IND_X;
	mem[0xFFFD] = 0x03;
	mem[0x0004] = 0x83;
	mem[0x0005] = 0x84;
	mem[0x8483] = 0x89;
	// end - inline a little program

	CPU CPUCopy = cpu;
	CyclesConfirm(cpu, 0x89, 6, 6);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
	VerfiyCPUFlagCIDBV(cpu,CPUCopy);

}
TEST_F(M6502Test1, LDAInd_YCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.Y = 0x01;
	mem[0xFFFC] = CPU::INS_LDA_IND_Y;
	mem[0xFFFD] = 0x03; // 0x03
	mem[0x0003] = 0x82;
	mem[0x0004] = 0x84; // 0x8482 + 0x1 =  0x8483
	mem[0x8483] = 0x89;
	// end - inline a little program

	CPU CPUCopy = cpu;
	CyclesConfirm(cpu, 0x89, 5, 5);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);

}
TEST_F(M6502Test1, LDAInd_YCanLoadAValueIntoTheARegisterWhenItCroessThePage)
{
	// start - inline a little program
	cpu.Y = 0xff;
	mem[0xFFFC] = CPU::INS_LDA_IND_Y;
	mem[0xFFFD] = 0x03; // 0x03
	mem[0x0003] = 0x82;
	mem[0x0004] = 0x84; // 0x8284 + 0xff =  0x8581
	mem[0x8581] = 0x89;
	// end - inline a little program

	CPU CPUCopy = cpu;
	CyclesConfirm(cpu, 0x89,6, 6);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
	VerfiyCPUFlagCIDBV(cpu,CPUCopy);

}