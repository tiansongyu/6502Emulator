#pragma once 
#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
// http://www.obelisk.me.uk/6502/

namespace m6502
{
	struct Mem;
	struct CPU;
	struct statusFlags;
}

static constexpr uint8_t
NegativeFlagBit = 1 << 7,
OverFlowFlagBit = 1 << 6,
BreakFlagBit = 1 << 5,
UnusedFlagBit = 1 << 4,
DecimalMode = 1 << 3,
InterruptDisableFlagBit = 1 << 2,
ZeroBit = 1 << 1,
CarryFlag = 1;

struct m6502::Mem
{
	static constexpr uint32_t MAX_MEM = 1024 * 64;
	uint8_t Data[MAX_MEM];
	void Init() { for (uint32_t i = 0; i < MAX_MEM; i++) Data[i] = 0; }
	uint8_t operator[](uint32_t Address) const { return Data[Address]; }
	uint8_t& operator[](uint32_t Address) { return Data[Address]; }
};
struct m6502::statusFlags
{
	uint8_t C : 1;
	uint8_t Z : 1;
	uint8_t I : 1;
	uint8_t D : 1;
	uint8_t B : 1;
	uint8_t unused : 1;
	uint8_t V : 1;
	uint8_t N : 1;
};

struct m6502::CPU
{
	uint16_t PC;
	uint8_t SP,A, X, Y;
	union
	{
		uint8_t ps;
		statusFlags Flags;
	};
	static constexpr uint8_t   //LDA
		INS_LDA_IM = 0xA9,
		INS_LDA_ZP = 0xA5,
		INS_LDA_ZPX = 0xB5,
		INS_LDA_ABS = 0xAD,
		INS_LDA_ABS_X = 0xBD,
		INS_LDA_ABS_Y = 0xB9,
		INS_LDA_IND_X = 0xA1,
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
INS_DEY_IM = 0x88,
//Branches
INS_BCC_REL = 0x90,
INS_BCS_REL = 0xB0,
INS_BEQ_REL = 0xF0,
INS_BMI_REL = 0x30,
INS_BNE_REL = 0xD0,
INS_BPL_REL = 0x10,
INS_BVC_REL = 0x50,
INS_BVS_REL = 0x70,
//Status Flag CHanges
INS_CLC_IM = 0x18,
INS_CLD_IM = 0xD8,
INS_CLI_IM = 0x58,
INS_CLV_IM = 0xB8,
INS_SEC_IM = 0x38,
INS_SED_IM = 0xF8,
INS_SEI_IM = 0x78,
//System Functions
INS_NOP = 0xEA,
//Arithmetic
INS_ADC_IM = 0x69,
INS_ADC_ZP = 0x65,
INS_ADC_ZPX = 0x75,
INS_ADC_ABS = 0x6D,
INS_ADC_ABS_X = 0x7D,
INS_ADC_ABS_Y = 0x79,
INS_ADC_IND_X = 0x61,
INS_ADC_IND_Y = 0x71,

INS_SBC_IM = 0xE9,
INS_SBC_ZP = 0xE5,
INS_SBC_ZPX = 0xF5,
INS_SBC_ABS = 0xED,
INS_SBC_ABS_X = 0xFD,
INS_SBC_ABS_Y = 0xF9,
INS_SBC_IND_X = 0xE1,
INS_SBC_IND_Y = 0xF1,

INS_CMP_IM = 0xC9,
INS_CMP_ZP = 0xC5,
INS_CMP_ZPX = 0xD5,
INS_CMP_ABS = 0xCD,
INS_CMP_ABS_X = 0xDD,
INS_CMP_ABS_Y = 0xD9,
INS_CMP_IND_X = 0xC1,
INS_CMP_IND_Y = 0xD1,

INS_CPX_IM = 0xE0,
INS_CPX_ZP = 0xE4,
INS_CPX_ABS = 0xEC,

INS_CPY_IM = 0xC0,
INS_CPY_ZP = 0xC4,
INS_CPY_ABS = 0xCC;


	//写入两个字节
	void WriteWord(const uint16_t Value, const uint32_t Address, int32_t& Cycles, Mem& memory);
	//写入一个字节
	void WriteByte(const uint8_t Value, const uint32_t Address, int32_t& Cycles, Mem& memory);

	//初始换内存
	void Reset(Mem& mem);
	//将一个字节压栈
	void PUSHByteTOAddress(uint8_t Data, int32_t& Cycles, Mem& memory);
	//将两个字节压栈
	void PUSHWordTOAddress(uint16_t Data, int32_t& Cycles, Mem& memory);
	//将PC压栈
	void PUSHPCTOAddress(Mem& memory, int32_t& Cycles);
	//将一个字节从栈弹出
	uint8_t POPByteFROMAddress(int32_t& Cycles, Mem& memory);
	//将PC从栈弹出
	uint16_t POPPCFROMAddress(int32_t& Cycles, Mem& memory);
	//指令的16进制代码
	//获取一个字节并且PC++
	inline uint8_t FetchByte(int32_t& Cycles, Mem& memory);
	//获取两个字节并且PC+=2
	inline uint16_t FetchWord(int32_t& Cycles, Mem& memory);
	//高位取00，低位取addresss中的内容
	inline uint8_t ReadByte(uint8_t address, int32_t& Cycles, Mem& memory);
	//直接取address中两个字节的内容
	inline uint8_t ReadByte(uint16_t address, int32_t& Cycles, Mem& memory);
	//获取两个字节内容
	inline uint16_t ReadWord(uint16_t address, int32_t& Cycles, Mem& memory);
	//address高位为00，地位取address
	inline uint16_t ReadWord(uint8_t address, int32_t& Cycles, Mem& memory);
	//将SP寄存器转换为地址
	inline uint16_t SPTOAddress();

	//根据Register，设置NagativeFlagBit和ZeroBit的值
	inline void SetStatus(uint8_t _register);

	/* CPU判断指令函数*/
	int32_t Execute(int32_t Cycles, Mem& memory);
	/* Zero Page处理函数*/
	uint8_t AddZeroPage(int32_t& Cycles, Mem& memory);
	/* Zero Page + X or Y 处理函数*/
	uint8_t AddZeroPageAdd(int32_t& Cycles, Mem& memory, uint8_t _register);
	/* Absolute address */
	uint8_t AbsoluteAddress(int32_t& Cycles, Mem& memory);
	/* Absolute address + register*/
	uint8_t AbsoluteAddress_Register(int32_t& Cycles, Mem& memory, uint8_t _register);
	/* Indirect address X */
	uint8_t IndirectAddressX(int32_t& Cycles, Mem& memory);
	/* Indirect address Y*/
	uint8_t IndirectAddressY(int32_t& Cycles, Mem& memory);
	/* Zero Page ST*  */
	void ZeroPageST_(uint8_t _register, int32_t& Cycles, Mem& memory);
	/* Add Zero Page register ST*  */
	void ZeroPageST_Register(uint8_t main_register, uint8_t __register, int32_t& Cycles, Mem& memory);
	/* Absolute address */
	void ST_AbsoluteAddress(uint8_t _register, int32_t& Cycles, Mem& memory);
	/* Absolute address + register*/
	void ST_AbsoluteAddress_Register(uint8_t main_register, uint8_t __register, int32_t& Cycles, Mem& memory);
	void ST_IndirectAddressX(int32_t& Cycles, Mem& memory);
	void ST_IndirectAddressY(int32_t& Cycles, Mem& memory);
	/* Relative mode */
	void RelativeModeClearIsJmp(const uint8_t FlagRegister, int32_t& Cycles, Mem& memory);
	void RelativeModeSetIsJmp(const uint8_t FlagRegister, int32_t& Cycles, Mem& memory);
};