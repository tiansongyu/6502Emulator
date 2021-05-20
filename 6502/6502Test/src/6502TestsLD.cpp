#include <gtest/gtest.h>
#include "main_6502.h"

class M6502Test1LD : public testing::Test
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
TEST_F(M6502Test1LD, LDAImmediate_Can_Load_A_Value_Into_The_ARegister)
{
	// given:
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_LDA_IM;
	mem[0xFFFD] = 0x84;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.A, 0x84, 2, 2);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);
}
TEST_F(M6502Test1LD, LDXImmediate_Can_Load_A_Value_Into_The_ARegister)
{
	// given:
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_LDX_IM;
	mem[0xFFFD] = 0x84;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.X, 0x84, 2, 2);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);
}
TEST_F(M6502Test1LD, LDYImmediate_Can_Load_A_Value_Into_The_ARegister)
{
	// given:
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_LDY_IM;
	mem[0xFFFD] = 0x84;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.Y, 0x84, 2, 2);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);
}
TEST_F(M6502Test1LD, LDAZeroPageCanLoadAValueIntoTheARegister)
{
	// given:
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_LDA_ZP;
	mem[0xFFFD] = 0x42;
	mem[0x0042] = 0x37;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.A, 0x37, 3, 3);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);
}
TEST_F(M6502Test1LD, LDXZeroPageCanLoadAValueIntoTheARegister)
{
	// given:
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_LDX_ZP;
	mem[0xFFFD] = 0x42;
	mem[0x0042] = 0x37;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.X, 0x37, 3, 3);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);
}
TEST_F(M6502Test1LD, LDYZeroPageCanLoadAValueIntoTheARegister)
{
	// given:
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_LDY_ZP;
	mem[0xFFFD] = 0x42;
	mem[0x0042] = 0x37;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.Y, 0x37, 3, 3);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);
}
TEST_F(M6502Test1LD, LDAZeroPageXCanLoadAValueIntoTheARegister)
{
	// given:
	cpu.X = 0xff;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_LDA_ZPX;
	mem[0xFFFD] = 0x42;
	mem[0x0041] = 0x37;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.A, 0x37, 4, 4);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);

}
TEST_F(M6502Test1LD, LDAAbsoluteCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_LDA_ABS;
	mem[0xFFFD] = 0x42;
	mem[0xFFFE] = 0x37;
	mem[0x3742] = 0x89;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.A, 0x89, 4, 4);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);

}
TEST_F(M6502Test1LD, LDXAbsoluteCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_LDX_ABS;
	mem[0xFFFD] = 0x42;
	mem[0xFFFE] = 0x37;
	mem[0x3742] = 0x89;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.X, 0x89, 4, 4);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);

}
TEST_F(M6502Test1LD, LDYAbsoluteCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_LDY_ABS;
	mem[0xFFFD] = 0x42;
	mem[0xFFFE] = 0x37;
	mem[0x3742] = 0x89;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.Y, 0x89, 4, 4);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);

}
TEST_F(M6502Test1LD, LDAAbsoluteXCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.X = 1;
	mem[0xFFFC] = m6502::CPU::INS_LDA_ABS_X;
	mem[0xFFFD] = 0xf3; // 
	mem[0xFFFE] = 0x01;
	mem[0x01f4] = 0x89;
	// end - inline a little program
	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.A, 0x89, 4, 4);

	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);

}
TEST_F(M6502Test1LD, LDXAbsoluteYCanLoadAValueIntoTheARegister)
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

	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);

}
TEST_F(M6502Test1LD, LDYAbsoluteXCanLoadAValueIntoTheARegister)
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

	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);

}
TEST_F(M6502Test1LD, LDAAbsoluteXCanLoadAValueIntoTheARegisterWhenItCrossThePage)
{
	// start - inline a little program
	cpu.X = 2;
	mem[0xFFFC] = m6502::CPU::INS_LDA_ABS_X;
	mem[0xFFFD] = 0xfe; // 0xfe - 0x101 ��ҳ����Ҫʹ��5������
	mem[0xFFFE] = 0x01;
	mem[0x0200] = 0x89;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.A, 0x89, 5, 5);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);
}
TEST_F(M6502Test1LD, LDAAbsoluteYCanLoadAValueIntoTheARegister)
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
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);

}
TEST_F(M6502Test1LD, LDAAbsoluteYCanLoadAValueIntoTheARegisterWhenItCrossThePage)
{
	// start - inline a little program
	cpu.Y = 1;
	mem[0xFFFC] = m6502::CPU::INS_LDA_ABS_Y;
	mem[0xFFFD] = 0xff;
	mem[0xFFFE] = 0x01;
	mem[0x0200] = 0x89;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.A, 0x89, 5, 5);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);

}
TEST_F(M6502Test1LD, LDXAbsoluteYCanLoadAValueIntoTheARegisterWhenItCrossThePage)
{
	// start - inline a little program
	cpu.Y = 1;
	mem[0xFFFC] = m6502::CPU::INS_LDX_ABS_Y;
	mem[0xFFFD] = 0xff;
	mem[0xFFFE] = 0x01;
	mem[0x0200] = 0x89;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.X, 0x89, 5, 5);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);

}
TEST_F(M6502Test1LD, LDYAbsoluteXCanLoadAValueIntoTheARegisterWhenItCrossThePage)
{
	// start - inline a little program
	cpu.X = 1;
	mem[0xFFFC] = m6502::CPU::INS_LDY_ABS_X;
	mem[0xFFFD] = 0xff;//��ҳ��
	mem[0xFFFE] = 0x01;
	mem[0x0200] = 0x89;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	CyclesConfirm(&cpu.Y, 0x89, 5, 5);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);

}
TEST_F(M6502Test1LD, LDAInd_XCanLoadAValueIntoTheARegister)
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
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);

}
TEST_F(M6502Test1LD, LDAInd_YCanLoadAValueIntoTheARegister)
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
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);

}
TEST_F(M6502Test1LD, LDAInd_YCanLoadAValueIntoTheARegisterWhenItCroessThePage)
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
	CyclesConfirm(&cpu.A, 0x89, 6, 6);
	EXPECT_FALSE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	VerfiyCPUFlagCIDBV(cpu, CPUCopy);

}