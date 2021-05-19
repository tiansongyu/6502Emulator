#pragma once 
#include <stdio.h>
#include <stdlib.h>

// http://www.obelisk.me.uk/6502/
namespace m6502
{
	using Byte = unsigned char;
	using Word = unsigned short;
	using u32 = unsigned int;
	using s32 = signed int;

	struct Mem;
	struct CPU;
	struct statusFlags;
}
static constexpr m6502::Byte
	NagativeFlagBit = 0b10000000,
	OverFlowFlagBit = 0b01000000,
	BreakFlagBit = 0b00010000,
	UnusedFlagBit = 0b00100000,
	InterruptDisableFlagBit = 0b000000100,
	ZeroBit = 0b00000001;


struct m6502::Mem
{
	static constexpr u32 MAX_MEM = 1024 * 64;
	Byte Data[MAX_MEM];
	void Initialise()
	{
		for (u32 i = 0; i < MAX_MEM; i++) Data[i] = 0;
	}
	Byte operator[](u32 Address) const
	{
		return Data[Address];
	}
	Byte& operator[](u32 Address)
	{
		return Data[Address];
	}
	//写入两个字节
	void WriteWord(const Word Value, const u32 Address, s32& Cycles)
	{
		Data[Address] = Value & 0xFF;
		Data[Address + 1] = (Value >> 8);
		Cycles -= 2;
	}
	void WriteByte(const Byte Value,const u32 Address, s32& Cycles)
	{
		Data[Address] = Value;
		Cycles--;
	}
};
struct m6502::statusFlags
{
	Byte C : 1;
	Byte Z : 1;
	Byte I : 1;
	Byte D : 1;
	Byte B : 1;
	Byte unused : 1;
	Byte V : 1;
	Byte N : 1;
};

struct m6502::CPU
{	
	Word PC;
	Byte SP;

	Byte A, X, Y;

	union
	{
		Byte ps;
		statusFlags Flags;
	};

	void Reset(Mem& mem)
	{
		PC = 0xFFFC; //将PC寄存器初始化 LDX #$FF
		SP = 0x00FF; //将栈寄存器初始化
		Flags.C = Flags.Z = Flags.I = Flags.D = Flags.B = Flags.V = Flags.N = 0;		 //将标志寄存器初始化，
					 //实际上是某个指令实现的初始化，这里暂时这样做
					 //比如D=0可以写成CLD
		A = X = Y = 0;
		mem.Initialise();
	}
	Byte FetchByte(s32& Cycles, Mem& memory)
	{
		Byte Data = memory[PC];
		PC++;
		Cycles--;
		return Data;
	}
	Byte ReadByte(Byte address, s32& Cycles, Mem& memory)
	{
		Byte Data = memory[address];
		Cycles--;
		return Data;
	}
	Byte ReadByte(Word address, s32& Cycles, Mem& memory)
	{
		Byte Data = memory[address];
		Cycles--;
		return Data;
	}
	Word ReadWord(Word address, s32& Cycles, Mem& memory)
	{
		Word Data = memory[address];
		Data |= (memory[address+1] << 8);
		Cycles-=2;
		return Data;
	}
	Word ReadWord(Byte address, s32& Cycles, Mem& memory)
	{
		Word Data = memory[address];
		Data |= (memory[address + 1] << 8);
		Cycles -= 2;
		return Data;
	}
	Word FetchWord(s32& Cycles, Mem& memory)
	{
		//小端存储数据  如果是大端口，需要调换数据顺序
		Word Data = memory[PC];
		PC++;

		Cycles--;
		Data |= (memory[PC] << 8);
		PC++;

		Cycles--;
		return Data;
	}
	Word SPTOAddress()
	{
		return 0x100 | SP;
	}
	void PUSHByteTOAddress(Byte Data, m6502::s32& Cycles, Mem& memory )
	{
		memory.WriteByte(Data, SPTOAddress() , Cycles); //两个周期
		SP--;
		Cycles--;
	}
	void PUSHWordTOAddress(Word Data, m6502::s32& Cycles, Mem& memory)
	{
		memory.WriteWord(Data, SPTOAddress() - 1, Cycles); //两个周期
		SP-=2;
	}
	void PUSHPCTOAddress(Mem&memory,m6502::s32& Cycles)
	{
		PUSHWordTOAddress(PC - 1, Cycles, memory);
	}
	Byte POPByteFROMAddress(m6502::s32& Cycles, Mem& memory)
	{
		Byte Value = ReadByte((Word)(SPTOAddress() + 1), Cycles, memory);
		SP ++;
		Cycles--;
		return Value;
	}
	Word POPPCFROMAddress(m6502::s32& Cycles,Mem& memory )
	{
		Word Value = ReadWord((Word)(SPTOAddress()+1), Cycles, memory);
		SP += 2;
		Cycles--;
		return Value;
	}

	static constexpr Byte   //LDA
		INS_LDA_IM = 0xA9,
		INS_LDA_ZP = 0xA5,
		INS_LDA_ZPX = 0xB5,
		INS_LDA_ABS = 0xAD,
		INS_LDA_ABS_X = 0xBD,
		INS_LDA_ABS_Y = 0xB9,
		INS_LDA_IND_X = 0x61,
		INS_LDA_IND_Y = 0xB1,
		//LDX
		INS_LDX_IM = 0xA2,
		INS_LDX_ZP = 0xA6,
		INS_LDX_ZPY = 0xB6,
		INS_LDX_ABS = 0xAE,
		INS_LDX_ABS_Y = 0xBE,
		// LDY
		INS_LDY_IM = 0xA0,
		INS_LDY_ZP = 0xA4,
		INS_LDY_ZPX = 0xB4,
		INS_LDY_ABS = 0xAC,
		INS_LDY_ABS_X = 0xBC,
		//STA
		INS_STA_ZP = 0x85,
		INS_STA_ZPX = 0x95,
		INS_STA_ABS = 0x8D,
		INS_STA_ABS_X = 0x9D,
		INS_STA_ABS_Y = 0x99,
		INS_STA_IND_X = 0x81,
		INS_STA_IND_Y = 0x91,
		//STX
		// 							INS_LDX_IM = 0xA2,
		INS_STX_ZP = 0x86,
		INS_STX_ZPY = 0x96,
		INS_STX_ABS = 0x8E,
		//STY
		INS_STY_ZP = 0x84,
		INS_STY_ZPX = 0x94,
		INS_STY_ABS = 0x8C,
		//JMP
		INS_JMP = 0x4C,
		INS_RTS = 0x60,
		INS_JSR = 0x20,
		//TSX
		INS_TSX = 0xBA,
		INS_TXS = 0x9A,
		INS_PHA = 0x48,
		INS_PHP = 0x08,
		INS_PLA = 0x68,
		INS_PLP = 0x28,
		//Logical AND
		INS_AND_IMM = 0x29,
		INS_AND_ZP = 0x25,
		INS_AND_ZPX = 0x35,
		INS_AND_ABS = 0x2D,
		INS_AND_ABSX = 0x3D,
		INS_AND_ABSY = 0x39,
		INS_AND_INDX = 0x21,
		INS_AND_INDY = 0x31,
		//Logical EOR
		INS_EOR_IMM = 0x49,
		INS_EOR_ZP = 0x45,
		INS_EOR_ZPX = 0x55,
		INS_EOR_ABS = 0x4D,
		INS_EOR_ABSX = 0x5D,
		INS_EOR_ABSY = 0x59,
		INS_EOR_INDX = 0x41,
		INS_EOR_INDY = 0x51,
		//Logical ORA
		INS_ORA_IMM = 0x09,
		INS_ORA_ZP = 0x05,
		INS_ORA_ZPX = 0x15,
		INS_ORA_ABS = 0x0D,
		INS_ORA_ABSX = 0x1D,
		INS_ORA_ABSY = 0x19,
		INS_ORA_INDX = 0x01,
		INS_ORA_INDY = 0x11,
		//BIT test
		INS_BIT_ZP = 0x24,
		INS_BIT_ABS = 0x2c,
		//TRANSFER REGISTER
		INS_TAX_IM = 0xAA,
		INS_TAY_IM = 0xA8,
		INS_TXA_IM = 0x8A,
		INS_TYA_IM = 0x98,
		// INCREMENTS DECREMENTS
		INS_INC_ZP = 0xE6,
		INS_INC_ZPX = 0xF6,
		INS_INC_ABS = 0xEE,
		INS_INC_ABSX = 0xFE,
		INS_INX_IM = 0xE8,
		INS_INY_IM = 0xC8,
		INS_DEC_ZP = 0xC6,
		INS_DEC_ZPX = 0xD6,
		INS_DEC_ABS = 0xCE,
		INS_DEC_ABSX = 0xDE,
		INS_DEX_IM = 0xCA,
		INS_DEY_IM = 0x88;
		

	void SetStatus(const m6502::Byte _register)
	{
		Flags.Z = (_register == 0);
		Flags.N = (_register & 0b10000000) > 0;
	}

	/* CPU判断指令函数*/
	s32 Execute(s32 Cycles, Mem& memory);

	/* Zero Page处理函数*/
	Byte AddZeroPage(s32& Cycles, Mem& memory);
	
	/* Zero Page + X or Y 处理函数*/
	Byte AddZeroPageAdd(s32& Cycles, Mem& memory,Byte _register);

	/* Absolute address */
	Byte AbsoluteAddress(s32& Cycles, Mem& memory);

	/* Absolute address + register*/
	Byte AbsoluteAddress_Register(s32& Cycles, Mem& memory, Byte _register);

	/* Indirect address X */
	Byte IndirectAddressX(s32& Cycles, Mem& memory);

	/* Indirect address Y*/
	Byte IndirectAddressY(s32& Cycles, Mem& memory);

	/* Zero Page ST*  */
	void ZeroPageST_(Byte _register, s32& Cycles, Mem& memory);

	/* Add Zero Page register ST*  */
	void ZeroPageST_Register(Byte main_register, Byte __register, s32& Cycles, Mem& memory);

	/* Absolute address */
	void ST_AbsoluteAddress(Byte _register, s32& Cycles, Mem& memory);

	/* Absolute address + register*/
	void ST_AbsoluteAddress_Register(Byte main_register, Byte __register, s32& Cycles, Mem& memory);

	void ST_IndirectAddressX(s32& Cycles, Mem& memory);
	void ST_IndirectAddressY(s32& Cycles, Mem& memory);

};