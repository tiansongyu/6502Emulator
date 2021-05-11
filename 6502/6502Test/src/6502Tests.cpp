#include <gtest/gtest.h>
#include "main_6502.h"

class M6502Test1 : public testing::Test
{
public:

	m6502::Mem mem;
	m6502::CPU cpu;
	virtual void SetUp()
	{
		cpu.Reset( mem );
	}

	virtual void TearDown()
	{
	}
	void CyclesConfirm( m6502::Byte *_register, const m6502::Byte& DataA, const m6502::s32 Cycles, const m6502::s32 RequestCycles)
	{
		m6502::s32 CyclesUsed = cpu.Execute(Cycles, mem);
		EXPECT_EQ(*_register, DataA);
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
};


TEST_F( M6502Test1, LDAImmediate_Can_Load_A_Value_Into_The_ARegister )
{
	// given:
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_LDA_IM;
	mem[0xFFFD] = 0x84;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.A, 0x84, 2, 2);
	EXPECT_FALSE( cpu.Z );
	EXPECT_TRUE( cpu.N );
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);
}
TEST_F(M6502Test1, LDXImmediate_Can_Load_A_Value_Into_The_ARegister)
{
	// given:
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_LDX_IM;
	mem[0xFFFD] = 0x84;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.X, 0x84, 2, 2);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);
}
TEST_F(M6502Test1, LDYImmediate_Can_Load_A_Value_Into_The_ARegister)
{
	// given:
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_LDY_IM;
	mem[0xFFFD] = 0x84;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.Y, 0x84, 2, 2);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);
}
TEST_F( M6502Test1, LDAZeroPageCanLoadAValueIntoTheARegister )
{
	// given:
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_LDA_ZP;
	mem[0xFFFD] = 0x42;
	mem[0x0042] = 0x37;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.A, 0x37, 3, 3);
	EXPECT_FALSE( cpu.Z );
	EXPECT_FALSE( cpu.N );
	VerfiyCPUFlagCIDBV( cpu, CPUCopy);
}
TEST_F(M6502Test1, LDXZeroPageCanLoadAValueIntoTheARegister)
{
	// given:
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_LDX_ZP;
	mem[0xFFFD] = 0x42;
	mem[0x0042] = 0x37;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.X, 0x37, 3, 3);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);
}
TEST_F(M6502Test1, LDYZeroPageCanLoadAValueIntoTheARegister)
{
	// given:
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_LDY_ZP;
	mem[0xFFFD] = 0x42;
	mem[0x0042] = 0x37;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.Y, 0x37, 3, 3);
	EXPECT_FALSE(cpu.Z);
	EXPECT_FALSE(cpu.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);
}
TEST_F( M6502Test1, LDAZeroPageXCanLoadAValueIntoTheARegister )
{
	// given:
	cpu.X = 0xff;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_LDA_ZPX;
	mem[0xFFFD] = 0x42;
	mem[0x0041] = 0x37;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.A, 0x37,4, 4);
	EXPECT_FALSE( cpu.Z );
	EXPECT_FALSE( cpu.N );
	VerfiyCPUFlagCIDBV( cpu, CPUCopy);

}
TEST_F(M6502Test1, LDAAbsoluteCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_LDA_ABS;
	mem[0xFFFD] = 0x42;
	mem[0xFFFE] = 0x37;
	mem[0x3742] = 0x89;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.A, 0x89, 4, 4);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
	VerfiyCPUFlagCIDBV( cpu, CPUCopy);

}
TEST_F(M6502Test1, LDXAbsoluteCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_LDX_ABS;
	mem[0xFFFD] = 0x42;
	mem[0xFFFE] = 0x37;
	mem[0x3742] = 0x89;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.X, 0x89, 4, 4);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);

}
TEST_F(M6502Test1, LDYAbsoluteCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_LDY_ABS;
	mem[0xFFFD] = 0x42;
	mem[0xFFFE] = 0x37;
	mem[0x3742] = 0x89;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.Y, 0x89, 4, 4);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);

}
TEST_F(M6502Test1, LDAAbsoluteXCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.X = 1;
	mem[0xFFFC] = m6502::CPU::INS_LDA_ABS_X;
	mem[0xFFFD] = 0xf3; // 
	mem[0xFFFE] = 0x01;
	mem[0x01f4] = 0x89;
	// end - inline a little program
	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.A, 0x89, 4,4);

	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
	VerfiyCPUFlagCIDBV( cpu, CPUCopy);

}
TEST_F(M6502Test1, LDXAbsoluteYCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.Y = 1;
	mem[0xFFFC] = m6502::CPU::INS_LDX_ABS_Y;
	mem[0xFFFD] = 0xf3; // 
	mem[0xFFFE] = 0x01;
	mem[0x01f4] = 0x89;
	// end - inline a little program
	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.X, 0x89, 4, 4);

	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);

}
TEST_F(M6502Test1, LDYAbsoluteXCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.X = 1;
	mem[0xFFFC] = m6502::CPU::INS_LDY_ABS_X;
	mem[0xFFFD] = 0xf3; // 
	mem[0xFFFE] = 0x01;
	mem[0x01f4] = 0x89;
	// end - inline a little program
	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.Y, 0x89, 4, 4);

	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);

}
TEST_F(M6502Test1, LDAAbsoluteXCanLoadAValueIntoTheARegisterWhenItCrossThePage)
{
	// start - inline a little program
	cpu.X = 2;
	mem[0xFFFC] = m6502::CPU::INS_LDA_ABS_X;
	mem[0xFFFD] = 0xfe; // 0xfe - 0x101 跨页了需要使用5个周期
	mem[0xFFFE] = 0x01;
	mem[0x0200] = 0x89;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.A, 0x89, 5, 5);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);
}
TEST_F(M6502Test1, LDAAbsoluteYCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.Y = 1;
	mem[0xFFFC] = m6502::CPU::INS_LDA_ABS_Y;
	mem[0xFFFD] = 0xf0;//
	mem[0xFFFE] = 0x01;
	mem[0x01F1] = 0x89;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.A, 0x89, 4, 4);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
	VerfiyCPUFlagCIDBV(cpu,CPUCopy);

}
TEST_F(M6502Test1, LDAAbsoluteYCanLoadAValueIntoTheARegisterWhenItCrossThePage)
{
	// start - inline a little program
	cpu.Y = 1;
	mem[0xFFFC] = m6502::CPU::INS_LDA_ABS_Y;
	mem[0xFFFD] = 0xfe;//跨页了
	mem[0xFFFE] = 0x01;
	mem[0x01FF] = 0x89;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.A, 0x89, 5, 5);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
	VerfiyCPUFlagCIDBV(cpu,CPUCopy);

}
TEST_F(M6502Test1, LDXAbsoluteYCanLoadAValueIntoTheARegisterWhenItCrossThePage)
{
	// start - inline a little program
	cpu.Y = 1;
	mem[0xFFFC] = m6502::CPU::INS_LDX_ABS_Y;
	mem[0xFFFD] = 0xfe;//跨页了
	mem[0xFFFE] = 0x01;
	mem[0x01FF] = 0x89;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.X, 0x89, 5, 5);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);

}
TEST_F(M6502Test1, LDYAbsoluteXCanLoadAValueIntoTheARegisterWhenItCrossThePage)
{
	// start - inline a little program
	cpu.X = 1;
	mem[0xFFFC] = m6502::CPU::INS_LDY_ABS_X;
	mem[0xFFFD] = 0xfe;//跨页了
	mem[0xFFFE] = 0x01;
	mem[0x01FF] = 0x89;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.Y, 0x89, 5, 5);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);

}
TEST_F(M6502Test1, LDAInd_XCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.X = 1;
	mem[0xFFFC] = m6502::CPU::INS_LDA_IND_X;
	mem[0xFFFD] = 0x03;
	mem[0x0004] = 0x83;
	mem[0x0005] = 0x84;
	mem[0x8483] = 0x89;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.A, 0x89, 6, 6);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
	VerfiyCPUFlagCIDBV(cpu,CPUCopy);

}
TEST_F(M6502Test1, LDAInd_YCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.Y = 0x01;
	mem[0xFFFC] = m6502::CPU::INS_LDA_IND_Y;
	mem[0xFFFD] = 0x03; // 0x03
	mem[0x0003] = 0x82;
	mem[0x0004] = 0x84; // 0x8482 + 0x1 =  0x8483
	mem[0x8483] = 0x89;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.A, 0x89, 5, 5);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);

}
TEST_F(M6502Test1, LDAInd_YCanLoadAValueIntoTheARegisterWhenItCroessThePage)
{
	// start - inline a little program
	cpu.Y = 0xff;
	mem[0xFFFC] = m6502::CPU::INS_LDA_IND_Y;
	mem[0xFFFD] = 0x03; // 0x03
	mem[0x0003] = 0x82;
	mem[0x0004] = 0x84; // 0x8284 + 0xff =  0x8581
	mem[0x8581] = 0x89;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.A, 0x89,6, 6);
	EXPECT_FALSE(cpu.Z);
	EXPECT_TRUE(cpu.N);
	VerfiyCPUFlagCIDBV(cpu,CPUCopy);

}