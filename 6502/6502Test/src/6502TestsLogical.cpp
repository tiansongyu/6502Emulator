#include <gtest/gtest.h>
#include "main_6502.h"

class M6502Test1Logical : public testing::Test
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
TEST_F(M6502Test1Logical, ANDIMMARegister)
{
	// given:
	// start - inline a little program
	cpu.A = 0b10101110;
	mem[0xFFFC] = m6502::CPU::INS_AND_IMM;
	mem[0xFFFD] = 0b00101111;
	// end - inline a little program
	STCyclesConfirm(cpu.A, 0b10101110&0b00101111, 2, 2);
}
TEST_F(M6502Test1Logical, EORIMMARegister)
{
	// given:
	// start - inline a little program
	cpu.A = 0b10101110;
	mem[0xFFFC] = m6502::CPU::INS_EOR_IMM;
	mem[0xFFFD] = 0b00101111;
	// end - inline a little program

	STCyclesConfirm(cpu.A, 0b10101110 ^ 0b00101111, 2, 2);
}
TEST_F(M6502Test1Logical, ORAIMMARegister)
{
	// given:
	// start - inline a little program
	cpu.A = 0b10101110;
	mem[0xFFFC] = m6502::CPU::INS_ORA_IMM;
	mem[0xFFFD] = 0b00101111;
	// end - inline a little program

	STCyclesConfirm(cpu.A, 0b10101110 | 0b00101111, 2, 2);
}
TEST_F(M6502Test1Logical, LDXImmediate_Can_Load_A_Value_Into_The_ARegister)
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
TEST_F(M6502Test1Logical, LDYImmediate_Can_Load_A_Value_Into_The_ARegister)
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
TEST_F(M6502Test1Logical, ANDZeroPageCanLoadAValueIntoTheARegister)
{
	// given:
	// start - inline a little program
	cpu.A = 0x20;
	mem[0xFFFC] = m6502::CPU::INS_AND_ZP;
	mem[0xFFFD] = 0x42;
	mem[0x0042] = 0x37;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(cpu.A, 0x20&0x37, 3, 3);

}
TEST_F(M6502Test1Logical, EORZeroPageCanLoadAValueIntoTheARegister)
{
	// given:
	// start - inline a little program
	cpu.A = 0x20;
	mem[0xFFFC] = m6502::CPU::INS_EOR_ZP;
	mem[0xFFFD] = 0x42;
	mem[0x0042] = 0x37;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(cpu.A, 0x20 ^ 0x37, 3, 3);

}
TEST_F(M6502Test1Logical, ORAZeroPageCanLoadAValueIntoTheARegister)
{
	// given:
	// start - inline a little program
	cpu.A = 0x20;
	mem[0xFFFC] = m6502::CPU::INS_ORA_ZP;
	mem[0xFFFD] = 0x42;
	mem[0x0042] = 0x37;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(cpu.A, 0x20 | 0x37, 3, 3);

}

TEST_F(M6502Test1Logical, BITZeroPageCanLoadAValueIntoTheARegister)
{
	// given:
	// start - inline a little program
	cpu.A = 0b01000000;
	mem[0xFFFC] = m6502::CPU::INS_BIT_ZP;
	mem[0xFFFD] = 0x42;
	mem[0x0042] = 0b01001001;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(1,1., 3, 3);
	EXPECT_FALSE(CPUCopy.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_TRUE(cpu.Flags.V);
}
TEST_F(M6502Test1Logical, BIT_ZeroPageCanLoadAValueIntoTheARegister_NTRUE_VFALSE)
{
	// given:
	// start - inline a little program
	cpu.A = 0b01000000;
	mem[0xFFFC] = m6502::CPU::INS_BIT_ZP;
	mem[0xFFFD] = 0x42;
	mem[0x0042] = 0b10001001;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(1, 1., 3, 3);
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_TRUE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.V);
}
TEST_F(M6502Test1Logical, BIT_ZeroPageCanLoadAValueIntoTheARegister_ZERO)
{
	// given:
	// start - inline a little program
	cpu.A = 0b10000000;
	mem[0xFFFC] = m6502::CPU::INS_BIT_ZP;
	mem[0xFFFD] = 0x42;
	mem[0x0042] = 0b00001001;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(1, 1., 3, 3);
	EXPECT_TRUE(cpu.Flags.Z);
	EXPECT_FALSE(cpu.Flags.N);
	EXPECT_FALSE(cpu.Flags.V);
}
TEST_F(M6502Test1Logical, LDXZeroPageCanLoadAValueIntoTheARegister)
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
TEST_F(M6502Test1Logical, LDYZeroPageCanLoadAValueIntoTheARegister)
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
TEST_F(M6502Test1Logical, ANDZeroPageXCanLoadAValueIntoTheARegister)
{
	// given:
	cpu.A = 0x20;
	cpu.X = 0xff;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_AND_ZPX;
	mem[0xFFFD] = 0x42;
	mem[0x0041] = 0x37;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(cpu.A, 0x20&0x37, 4, 4);
}
TEST_F(M6502Test1Logical, EORZeroPageXCanLoadAValueIntoTheARegister)
{
	// given:
	cpu.A = 0x20;
	cpu.X = 0xff;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_EOR_ZPX;
	mem[0xFFFD] = 0x42;
	mem[0x0041] = 0x37;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(cpu.A, 0x20 ^ 0x37, 4, 4);
}
TEST_F(M6502Test1Logical, ORAZeroPageXCanLoadAValueIntoTheARegister)
{
	// given:
	cpu.A = 0x20;
	cpu.X = 0xff;
	// start - inline a little program
	mem[0xFFFC] = m6502::CPU::INS_ORA_ZPX;
	mem[0xFFFD] = 0x42;
	mem[0x0041] = 0x37;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(cpu.A, 0x20 | 0x37, 4, 4);
}
TEST_F(M6502Test1Logical, ANDAbsoluteCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.A = 0x20;
	mem[0xFFFC] = m6502::CPU::INS_AND_ABS;
	mem[0xFFFD] = 0x42;
	mem[0xFFFE] = 0x37;
	mem[0x3742] = 0x89;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(cpu.A, 0x20&0x89, 4, 4);
}
TEST_F(M6502Test1Logical, EORAbsoluteCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.A = 0x20;
	mem[0xFFFC] = m6502::CPU::INS_EOR_ABS;
	mem[0xFFFD] = 0x42;
	mem[0xFFFE] = 0x37;
	mem[0x3742] = 0x89;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(cpu.A, 0x20 ^ 0x89, 4, 4);
}
TEST_F(M6502Test1Logical, ORAAbsoluteCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.A = 0x20;
	mem[0xFFFC] = m6502::CPU::INS_ORA_ABS;
	mem[0xFFFD] = 0x42;
	mem[0xFFFE] = 0x37;
	mem[0x3742] = 0x89;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(cpu.A, 0x20 | 0x89, 4, 4);
}
TEST_F(M6502Test1Logical, LDXAbsoluteCanLoadAValueIntoTheARegister)
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
TEST_F(M6502Test1Logical, LDYAbsoluteCanLoadAValueIntoTheARegister)
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
TEST_F(M6502Test1Logical, ANDAbsoluteXCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.A = 0x30;
	cpu.X = 1;
	mem[0xFFFC] = m6502::CPU::INS_AND_ABSX;
	mem[0xFFFD] = 0xf3; // 
	mem[0xFFFE] = 0x01;
	mem[0x01f4] = 0x89;
	// end - inline a little program
	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(cpu.A, 0x30&0x89, 4, 4);
}
TEST_F(M6502Test1Logical, EORAbsoluteXCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.A = 0x30;
	cpu.X = 1;
	mem[0xFFFC] = m6502::CPU::INS_EOR_ABSX;
	mem[0xFFFD] = 0xf3; // 
	mem[0xFFFE] = 0x01;
	mem[0x01f4] = 0x89;
	// end - inline a little program
	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(cpu.A, 0x30 ^ 0x89, 4, 4);
}
TEST_F(M6502Test1Logical, ORAAbsoluteXCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.A = 0x30;
	cpu.X = 1;
	mem[0xFFFC] = m6502::CPU::INS_ORA_ABSX;
	mem[0xFFFD] = 0xf3; // 
	mem[0xFFFE] = 0x01;
	mem[0x01f4] = 0x89;
	// end - inline a little program
	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(cpu.A, 0x30 | 0x89, 4, 4);
}
TEST_F(M6502Test1Logical, ANDAbsoluteYCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.A = 0x40;
	cpu.Y = 1;
	mem[0xFFFC] = m6502::CPU::INS_AND_ABSY;
	mem[0xFFFD] = 0xf3; // 
	mem[0xFFFE] = 0x01;
	mem[0x01f4] = 0x89;
	// end - inline a little program
	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(cpu.A, 0x40&0x89, 4, 4);
}
TEST_F(M6502Test1Logical, EORAbsoluteYCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.A = 0x40;
	cpu.Y = 1;
	mem[0xFFFC] = m6502::CPU::INS_EOR_ABSY;
	mem[0xFFFD] = 0xf3; // 
	mem[0xFFFE] = 0x01;
	mem[0x01f4] = 0x89;
	// end - inline a little program
	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(cpu.A, 0x40 ^ 0x89, 4, 4);
}
TEST_F(M6502Test1Logical, ORAAbsoluteYCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.A = 0x40;
	cpu.Y = 1;
	mem[0xFFFC] = m6502::CPU::INS_ORA_ABSY;
	mem[0xFFFD] = 0xf3; // 
	mem[0xFFFE] = 0x01;
	mem[0x01f4] = 0x89;
	// end - inline a little program
	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(cpu.A, 0x40 | 0x89, 4, 4);
}
TEST_F(M6502Test1Logical, LDYAbsoluteXCanLoadAValueIntoTheARegister)
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
TEST_F(M6502Test1Logical, LDAAbsoluteXCanLoadAValueIntoTheARegisterWhenItCrossThePage)
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
TEST_F(M6502Test1Logical, LDAAbsoluteYCanLoadAValueIntoTheARegister)
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
TEST_F(M6502Test1Logical, LDAAbsoluteYCanLoadAValueIntoTheARegisterWhenItCrossThePage)
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
TEST_F(M6502Test1Logical, LDXAbsoluteYCanLoadAValueIntoTheARegisterWhenItCrossThePage)
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
TEST_F(M6502Test1Logical, LDYAbsoluteXCanLoadAValueIntoTheARegisterWhenItCrossThePage)
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
TEST_F(M6502Test1Logical, ANDInd_XCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.A = 0x22;
	cpu.X = 1;
	mem[0xFFFC] = m6502::CPU::INS_AND_INDX;
	mem[0xFFFD] = 0x03;
	mem[0x0004] = 0x83;
	mem[0x0005] = 0x84;
	mem[0x8483] = 0x89;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(cpu.A, 0x22&0x89, 6, 6);
}
TEST_F(M6502Test1Logical, EORInd_XCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.A = 0x22;
	cpu.X = 1;
	mem[0xFFFC] = m6502::CPU::INS_EOR_INDX;
	mem[0xFFFD] = 0x03;
	mem[0x0004] = 0x83;
	mem[0x0005] = 0x84;
	mem[0x8483] = 0x89;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(cpu.A, 0x22 ^ 0x89, 6, 6);
}
TEST_F(M6502Test1Logical, ORAInd_XCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.A = 0x22;
	cpu.X = 1;
	mem[0xFFFC] = m6502::CPU::INS_ORA_INDX;
	mem[0xFFFD] = 0x03;
	mem[0x0004] = 0x83;
	mem[0x0005] = 0x84;
	mem[0x8483] = 0x89;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(cpu.A, 0x22 | 0x89, 6, 6);
}
TEST_F(M6502Test1Logical, ANDInd_YCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.A = 0x33;
	cpu.Y = 0x01;
	mem[0xFFFC] = m6502::CPU::INS_AND_INDY;
	mem[0xFFFD] = 0x03; // 0x03
	mem[0x0003] = 0x82;
	mem[0x0004] = 0x84; // 0x8482 + 0x1 =  0x8483
	mem[0x8483] = 0x89;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(cpu.A, 0x33&0x89, 5, 5);
}
TEST_F(M6502Test1Logical, EORInd_YCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.A = 0x33;
	cpu.Y = 0x01;
	mem[0xFFFC] = m6502::CPU::INS_EOR_INDY;
	mem[0xFFFD] = 0x03; // 0x03
	mem[0x0003] = 0x82;
	mem[0x0004] = 0x84; // 0x8482 + 0x1 =  0x8483
	mem[0x8483] = 0x89;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(cpu.A, 0x33 ^ 0x89, 5, 5);
}
TEST_F(M6502Test1Logical, ORAInd_YCanLoadAValueIntoTheARegister)
{
	// start - inline a little program
	cpu.A = 0x33;
	cpu.Y = 0x01;
	mem[0xFFFC] = m6502::CPU::INS_ORA_INDY;
	mem[0xFFFD] = 0x03; // 0x03
	mem[0x0003] = 0x82;
	mem[0x0004] = 0x84; // 0x8482 + 0x1 =  0x8483
	mem[0x8483] = 0x89;
	// end - inline a little program

	m6502::CPU CPUCopy = cpu;
	STCyclesConfirm(cpu.A, 0x33 | 0x89, 5, 5);
}
TEST_F(M6502Test1Logical, LDAInd_YCanLoadAValueIntoTheARegisterWhenItCroessThePage)
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