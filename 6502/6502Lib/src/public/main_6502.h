#pragma once 
#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
#include <map>
#include <string>
#include <vector>
// http://www.obelisk.me.uk/6502/

class Bus;
class CPU;
class statusFlags;
static constexpr uint8_t
	NegativeFlagBit = 1 << 7,
	OverFlowFlagBit = 1 << 6,
	BreakFlagBit = 1 << 5,
	UnusedFlagBit = 1 << 4,
	DecimalMode = 1 << 3,
	InterruptDisableFlagBit = 1 << 2,
	ZeroBit = 1 << 1,
	CarryFlag = 1;
class statusFlags
{
public:
	uint8_t C : 1;
	uint8_t Z : 1;
	uint8_t I : 1;
	uint8_t D : 1;
	uint8_t B : 1;
	uint8_t unused : 1;
	uint8_t V : 1;
	uint8_t N : 1;
};

class CPU
{
public:
	Bus* bus = nullptr;

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
INS_CPY_ABS = 0xCC,
//Shifts
INS_ASL_ACC = 0x0A,
INS_ASL_ZP = 0x06,
INS_ASL_ZPX = 0x16,
INS_ASL_ABS = 0x0E,
INS_ASL_ABSX = 0x1E,

INS_LSR_ACC = 0x4A,
INS_LSR_ZP = 0x46,
INS_LSR_ZPX = 0x56,
INS_LSR_ABS = 0x4E,
INS_LSR_ABSX = 0x5E,

INS_ROL_ACC = 0x2A,
INS_ROL_ZP = 0x26,
INS_ROL_ZPX = 0x36,
INS_ROL_ABS = 0x2E,
INS_ROL_ABSX = 0x3E,

INS_ROR_ACC = 0x6A,
INS_ROR_ZP = 0x66,
INS_ROR_ZPX = 0x76,
INS_ROR_ABS = 0x6E,
INS_ROR_ABSX = 0x7E,

//System Functions
INS_NOP = 0xEA,
INS_BRK_IM = 0x00,
INS_RTI_IM = 0x40;

void ConnectBus(Bus* bus);

	//д�������ֽ�
	void WriteWord(const uint16_t Value, const uint32_t Address, int32_t& Cycles);
	//д��һ���ֽ�
	void WriteByte(const uint8_t Value, const uint32_t Address, int32_t& Cycles);

	//��ʼ���ڴ�
	void Reset();
	//��һ���ֽ�ѹջ
	void PushByteToAddress(uint8_t Data, int32_t& Cycles);
	//�������ֽ�ѹջ
	void PUSHWordTOAddress(uint16_t Data, int32_t& Cycles);
	//��һ���ֽڴ�ջ����
	uint8_t PopByteFromAddress(int32_t& Cycles);
	//��PC��ջ����
	uint16_t PopWordFromAddress(int32_t& Cycles);
	//ָ���16���ƴ���
	//��ȡһ���ֽڲ���PC++
	inline uint8_t FetchByte(int32_t& Cycles);
	//��ȡ�����ֽڲ���PC+=2
	inline uint16_t FetchWord(int32_t& Cycles);
	//��λȡ00����λȡaddresss�е�����
	inline uint8_t ReadByte(uint8_t address, int32_t& Cycles);
	//ֱ��ȡaddress�������ֽڵ�����
	inline uint8_t ReadByte(uint16_t address, int32_t& Cycles);
	//��ȡ�����ֽ�����
	inline uint16_t ReadWord(uint16_t address, int32_t& Cycles);
	//address��λΪ00����λȡaddress
	inline uint16_t ReadWord(uint8_t address, int32_t& Cycles);
	//��SP�Ĵ���ת��Ϊ��ַ
	inline uint16_t SPTOAddress();

	//����Register������NagativeFlagBit��ZeroBit��ֵ
	inline void SetStatus(uint8_t _register);

	/* CPU�ж�ָ���*/
	int32_t Execute(int32_t Cycles);
	/* Zero Page��������*/
	uint8_t AddZeroPage(int32_t& Cycles);
	/* Zero Page + X or Y ��������*/
	uint8_t AddZeroPageAdd(int32_t& Cycles, uint8_t _register);
	/* Absolute address */
	uint8_t AbsoluteAddress(int32_t& Cycles);
	/* Absolute address + register*/
	uint8_t AbsoluteAddress_Register(int32_t& Cycles, uint8_t _register);
	/* Absolute address + register*/
	uint8_t AbsoluteAddress_RegisterShifts(int32_t& Cycles, uint8_t _register);
	/* Indirect address X */
	uint8_t IndirectAddressX(int32_t& Cycles);
	/* Indirect address Y*/
	uint8_t IndirectAddressY(int32_t& Cycles);
	/* Zero Page ST*  */
	void ZeroPageST(uint8_t _register, int32_t& Cycles);
	/* Add Zero Page register ST*  */
	void ZeroPageSTRegister(uint8_t main_register, uint8_t __register, int32_t& Cycles);
	/* Absolute address */
	void STAbsoluteAddress(uint8_t _register, int32_t& Cycles);
	/* Absolute address + register*/
	void STAbsoluteAddress_Register(uint8_t main_register, uint8_t __register, int32_t& Cycles);
	void STIndirectAddressX(int32_t& Cycles);
	void STIndirectAddressY(int32_t& Cycles);
	/* Relative mode */
	void RelativeModeClearIsJmp(const uint8_t FlagRegister, int32_t& Cycles);
	void RelativeModeSetIsJmp(const uint8_t FlagRegister, int32_t& Cycles);
	//反汇编模块
	//from olc https://github.com/OneLoneCoder/olcNES
	std::map<uint16_t, std::string> disassemble(uint16_t nStart, uint16_t nStop);
};
/*
struct INSTRUCTION
{
	std::string name;
	uint8_t(olc6502::* operate)(void) = nullptr;
	uint8_t(olc6502::* addrmode)(void) = nullptr;
	uint8_t     cycles = 0;
};

std::vector<INSTRUCTION> lookup;
lookup =
{
	{ "BRK", &a::BRK, &a::IMM, 7 },{ "ORA", &a::ORA, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 3 },{ "ORA", &a::ORA, &a::ZP0, 3 },{ "ASL", &a::ASL, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PHP", &a::PHP, &a::IMP, 3 },{ "ORA", &a::ORA, &a::IMM, 2 },{ "ASL", &a::ASL, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::NOP, &a::IMP, 4 },{ "ORA", &a::ORA, &a::ABS, 4 },{ "ASL", &a::ASL, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
	{ "BPL", &a::BPL, &a::REL, 2 },{ "ORA", &a::ORA, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "ORA", &a::ORA, &a::ZPX, 4 },{ "ASL", &a::ASL, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "CLC", &a::CLC, &a::IMP, 2 },{ "ORA", &a::ORA, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "ORA", &a::ORA, &a::ABX, 4 },{ "ASL", &a::ASL, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
	{ "JSR", &a::JSR, &a::ABS, 6 },{ "AND", &a::AND, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "BIT", &a::BIT, &a::ZP0, 3 },{ "AND", &a::AND, &a::ZP0, 3 },{ "ROL", &a::ROL, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PLP", &a::PLP, &a::IMP, 4 },{ "AND", &a::AND, &a::IMM, 2 },{ "ROL", &a::ROL, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "BIT", &a::BIT, &a::ABS, 4 },{ "AND", &a::AND, &a::ABS, 4 },{ "ROL", &a::ROL, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
	{ "BMI", &a::BMI, &a::REL, 2 },{ "AND", &a::AND, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "AND", &a::AND, &a::ZPX, 4 },{ "ROL", &a::ROL, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "SEC", &a::SEC, &a::IMP, 2 },{ "AND", &a::AND, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "AND", &a::AND, &a::ABX, 4 },{ "ROL", &a::ROL, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
	{ "RTI", &a::RTI, &a::IMP, 6 },{ "EOR", &a::EOR, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 3 },{ "EOR", &a::EOR, &a::ZP0, 3 },{ "LSR", &a::LSR, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PHA", &a::PHA, &a::IMP, 3 },{ "EOR", &a::EOR, &a::IMM, 2 },{ "LSR", &a::LSR, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "JMP", &a::JMP, &a::ABS, 3 },{ "EOR", &a::EOR, &a::ABS, 4 },{ "LSR", &a::LSR, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
	{ "BVC", &a::BVC, &a::REL, 2 },{ "EOR", &a::EOR, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "EOR", &a::EOR, &a::ZPX, 4 },{ "LSR", &a::LSR, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "CLI", &a::CLI, &a::IMP, 2 },{ "EOR", &a::EOR, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "EOR", &a::EOR, &a::ABX, 4 },{ "LSR", &a::LSR, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
	{ "RTS", &a::RTS, &a::IMP, 6 },{ "ADC", &a::ADC, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 3 },{ "ADC", &a::ADC, &a::ZP0, 3 },{ "ROR", &a::ROR, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PLA", &a::PLA, &a::IMP, 4 },{ "ADC", &a::ADC, &a::IMM, 2 },{ "ROR", &a::ROR, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "JMP", &a::JMP, &a::IND, 5 },{ "ADC", &a::ADC, &a::ABS, 4 },{ "ROR", &a::ROR, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
	{ "BVS", &a::BVS, &a::REL, 2 },{ "ADC", &a::ADC, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "ADC", &a::ADC, &a::ZPX, 4 },{ "ROR", &a::ROR, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "SEI", &a::SEI, &a::IMP, 2 },{ "ADC", &a::ADC, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "ADC", &a::ADC, &a::ABX, 4 },{ "ROR", &a::ROR, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
	{ "???", &a::NOP, &a::IMP, 2 },{ "STA", &a::STA, &a::IZX, 6 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 6 },{ "STY", &a::STY, &a::ZP0, 3 },{ "STA", &a::STA, &a::ZP0, 3 },{ "STX", &a::STX, &a::ZP0, 3 },{ "???", &a::XXX, &a::IMP, 3 },{ "DEY", &a::DEY, &a::IMP, 2 },{ "???", &a::NOP, &a::IMP, 2 },{ "TXA", &a::TXA, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "STY", &a::STY, &a::ABS, 4 },{ "STA", &a::STA, &a::ABS, 4 },{ "STX", &a::STX, &a::ABS, 4 },{ "???", &a::XXX, &a::IMP, 4 },
	{ "BCC", &a::BCC, &a::REL, 2 },{ "STA", &a::STA, &a::IZY, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 6 },{ "STY", &a::STY, &a::ZPX, 4 },{ "STA", &a::STA, &a::ZPX, 4 },{ "STX", &a::STX, &a::ZPY, 4 },{ "???", &a::XXX, &a::IMP, 4 },{ "TYA", &a::TYA, &a::IMP, 2 },{ "STA", &a::STA, &a::ABY, 5 },{ "TXS", &a::TXS, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 5 },{ "???", &a::NOP, &a::IMP, 5 },{ "STA", &a::STA, &a::ABX, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "???", &a::XXX, &a::IMP, 5 },
	{ "LDY", &a::LDY, &a::IMM, 2 },{ "LDA", &a::LDA, &a::IZX, 6 },{ "LDX", &a::LDX, &a::IMM, 2 },{ "???", &a::XXX, &a::IMP, 6 },{ "LDY", &a::LDY, &a::ZP0, 3 },{ "LDA", &a::LDA, &a::ZP0, 3 },{ "LDX", &a::LDX, &a::ZP0, 3 },{ "???", &a::XXX, &a::IMP, 3 },{ "TAY", &a::TAY, &a::IMP, 2 },{ "LDA", &a::LDA, &a::IMM, 2 },{ "TAX", &a::TAX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "LDY", &a::LDY, &a::ABS, 4 },{ "LDA", &a::LDA, &a::ABS, 4 },{ "LDX", &a::LDX, &a::ABS, 4 },{ "???", &a::XXX, &a::IMP, 4 },
	{ "BCS", &a::BCS, &a::REL, 2 },{ "LDA", &a::LDA, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 5 },{ "LDY", &a::LDY, &a::ZPX, 4 },{ "LDA", &a::LDA, &a::ZPX, 4 },{ "LDX", &a::LDX, &a::ZPY, 4 },{ "???", &a::XXX, &a::IMP, 4 },{ "CLV", &a::CLV, &a::IMP, 2 },{ "LDA", &a::LDA, &a::ABY, 4 },{ "TSX", &a::TSX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 4 },{ "LDY", &a::LDY, &a::ABX, 4 },{ "LDA", &a::LDA, &a::ABX, 4 },{ "LDX", &a::LDX, &a::ABY, 4 },{ "???", &a::XXX, &a::IMP, 4 },
	{ "CPY", &a::CPY, &a::IMM, 2 },{ "CMP", &a::CMP, &a::IZX, 6 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "CPY", &a::CPY, &a::ZP0, 3 },{ "CMP", &a::CMP, &a::ZP0, 3 },{ "DEC", &a::DEC, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "INY", &a::INY, &a::IMP, 2 },{ "CMP", &a::CMP, &a::IMM, 2 },{ "DEX", &a::DEX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "CPY", &a::CPY, &a::ABS, 4 },{ "CMP", &a::CMP, &a::ABS, 4 },{ "DEC", &a::DEC, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
	{ "BNE", &a::BNE, &a::REL, 2 },{ "CMP", &a::CMP, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "CMP", &a::CMP, &a::ZPX, 4 },{ "DEC", &a::DEC, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "CLD", &a::CLD, &a::IMP, 2 },{ "CMP", &a::CMP, &a::ABY, 4 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "CMP", &a::CMP, &a::ABX, 4 },{ "DEC", &a::DEC, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
	{ "CPX", &a::CPX, &a::IMM, 2 },{ "SBC", &a::SBC, &a::IZX, 6 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "CPX", &a::CPX, &a::ZP0, 3 },{ "SBC", &a::SBC, &a::ZP0, 3 },{ "INC", &a::INC, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "INX", &a::INX, &a::IMP, 2 },{ "SBC", &a::SBC, &a::IMM, 2 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "???", &a::SBC, &a::IMP, 2 },{ "CPX", &a::CPX, &a::ABS, 4 },{ "SBC", &a::SBC, &a::ABS, 4 },{ "INC", &a::INC, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
	{ "BEQ", &a::BEQ, &a::REL, 2 },{ "SBC", &a::SBC, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "SBC", &a::SBC, &a::ZPX, 4 },{ "INC", &a::INC, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "SED", &a::SED, &a::IMP, 2 },{ "SBC", &a::SBC, &a::ABY, 4 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "SBC", &a::SBC, &a::ABX, 4 },{ "INC", &a::INC, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
};
*/