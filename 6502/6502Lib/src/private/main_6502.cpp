#include "main_6502.h"
using namespace m6502;
int32_t CPU::Execute(int32_t Cycles, Mem& memory)
{
	auto ADC = [this](uint8_t Value)
	{
		const bool IsTheSameFlag = !((A ^ Value) & NegativeFlagBit);
		uint32_t Sum = A;
		Sum += Value;
		Sum += Flags.C;
		A = Sum & 0xFF;
		Flags.C = Sum > 0xFF;
		Flags.V = IsTheSameFlag && ((A ^ Value) & NegativeFlagBit);
		SetStatus(A);
	};
	auto SBC = [this](uint8_t Value)
	{
		const bool IsTheSameFlag = !((A ^ Value) & NegativeFlagBit);
		uint32_t Sum = A;
		Sum += (~Value & 0xFF);
		Sum += (~Flags.C & 1);
		A = Sum & 0xFF;
		Flags.C = Sum > 0xFF;
		Flags.V = IsTheSameFlag && ((A ^ Value) & NegativeFlagBit);
		SetStatus(A);
	};
	auto CompareRegister = [this](uint8_t Value,uint8_t Register)
	{
		Flags.Z = Register == Value;
		Flags.C = Register >= Value;
		Flags.N = ((Register - Value) & NegativeFlagBit > 0);
	};

	int32_t CyclesRequests = Cycles;
	while (Cycles > 0)
	{
		uint8_t Ins = FetchByte(Cycles, memory);
		switch (Ins)
		{
		case INS_LDA_IM: //立即寻址 Immediate
		{
			A = FetchByte(Cycles, memory);
			SetStatus(A);
		}break;
		case INS_AND_IMM: //立即寻址 AND Immediate
		{
			A &= FetchByte(Cycles, memory);
			SetStatus(A);
		}break;
		case INS_EOR_IMM: //立即寻址 AND Immediate
		{
			A ^= FetchByte(Cycles, memory);
			SetStatus(A);
		}break;
		case INS_ORA_IMM: //立即寻址 AND Immediate
		{
			A |= FetchByte(Cycles, memory);
			SetStatus(A);
		}break;
		case INS_LDX_IM: //立即寻址 Immediate
		{
			X = FetchByte(Cycles, memory);
			SetStatus(X);
		}break;
		case INS_LDY_IM: //立即寻址 Immediate
		{
			Y = FetchByte(Cycles, memory);
			SetStatus(Y);
		}break;
		case INS_TAX_IM: //立即寻址 Immediate
		{
			X = A;
			Cycles--;
			SetStatus(X);
		}break;
		case INS_TAY_IM: //立即寻址 Immediate
		{
			Y = A;
			Cycles--;
			SetStatus(Y);
		}break;
		case INS_TXA_IM: //立即寻址 Immediate
		{
			A = X;
			Cycles--;
			SetStatus(A);
		}break;
		case INS_TYA_IM: //立即寻址 Immediate
		{
			A = Y;
			Cycles--;
			SetStatus(A);
		}break;
		case INS_INX_IM: //立即寻址 Immediate
		{
			X++;
			Cycles--;
			SetStatus(X);
		}break;
		case INS_INY_IM: //立即寻址 Immediate
		{
			Y++;
			Cycles--;
			SetStatus(Y);
		}break;
		case INS_DEX_IM: //立即寻址 Immediate
		{
			X--;
			Cycles--;
			SetStatus(X);
		}break;
		case INS_DEY_IM: //立即寻址 Immediate
		{
			Y--;
			Cycles--;
			SetStatus(Y);
		}break;
		case INS_LDA_ZP: //零页寻址 ..zero page
		{
			A = AddZeroPage(Cycles, memory);
			SetStatus(A);
		}break;
		case INS_AND_ZP: //零页寻址 ..zero page
		{
			A &= AddZeroPage(Cycles, memory);
			SetStatus(A);
		}break;
		case INS_EOR_ZP: //零页寻址 ..zero page
		{
			A ^= AddZeroPage(Cycles, memory);
			SetStatus(A);
		}break;
		case INS_BIT_ZP:
		{
			uint8_t Value = AddZeroPage(Cycles, memory);
			Flags.Z = !(Value & A);
			Flags.N = (Value & NegativeFlagBit) != 0;
			Flags.V = (Value & OverFlowFlagBit) != 0;
		}break;
		case INS_ORA_ZP: //零页寻址 ..zero page
		{
			A |= AddZeroPage(Cycles, memory);
			SetStatus(A);
		}break;
		case INS_LDX_ZP: //零页寻址 ..zero page
		{
			X = AddZeroPage(Cycles, memory);

			SetStatus(X);
		}break;
		case INS_LDY_ZP: //零页寻址 ..zero page
		{
			Y = AddZeroPage(Cycles, memory);
			SetStatus(Y);
		}break;
		//INC START
		case INS_INC_ZP: //零页寻址 ..zero page
		{
			uint8_t Value = AddZeroPage(Cycles, memory) + 1;
			uint8_t Address = memory[PC - 1];
			Cycles--;
			//Byte Address = ReadByte(memory[PC - 1], Cycles, memory);
			WriteByte(Value, Address, Cycles, memory);
			SetStatus(Value);
		}break;
		case INS_DEC_ZP: //零页寻址 ..zero page
		{
			uint8_t Value = AddZeroPage(Cycles, memory) - 1;
			uint8_t Address = memory[PC - 1];
			Cycles--;
			//Byte Address = ReadByte(memory[PC - 1], Cycles, memory);
			WriteByte(Value, Address, Cycles, memory);
			SetStatus(Value);
		}break;
		// INC END
		case INS_LDA_ZPX: //零页寻址 ..zero page x
		{
			A = AddZeroPageAdd(Cycles, memory, X);
			SetStatus(A);
		}break;
		case INS_INC_ZPX: //零页寻址 ..zero page x
		{
			uint8_t Value = AddZeroPageAdd(Cycles, memory, X) + 1;
			uint8_t Address = memory[PC - 1] + X;
			Cycles--;
			WriteByte(Value, Address, Cycles, memory);
			SetStatus(Value);
		}break;
		case INS_DEC_ZPX: //零页寻址 
		{
			uint8_t Value = AddZeroPageAdd(Cycles, memory, X) - 1;
			uint8_t Address = memory[PC - 1] + X;
			Cycles--;
			WriteByte(Value, Address, Cycles, memory);
			SetStatus(Value);
		}break;
		case INS_AND_ZPX: //零页寻址 ..zero page x
		{
			A &= AddZeroPageAdd(Cycles, memory, X);
			SetStatus(A);
		}break;
		case INS_EOR_ZPX: //零页寻址 ..zero page x
		{
			A ^= AddZeroPageAdd(Cycles, memory, X);
			SetStatus(A);
		}break;
		case INS_ORA_ZPX: //零页寻址 ..zero page x
		{
			A |= AddZeroPageAdd(Cycles, memory, X);
			SetStatus(A);
		}break;
		case INS_LDX_ZPY: //零页寻址 ..zero page x
		{
			X = AddZeroPageAdd(Cycles, memory, Y);
			SetStatus(X);
		}break;
		case INS_LDY_ZPX: //零页寻址 ..zero page x
		{
			Y = AddZeroPageAdd(Cycles, memory, X);
			SetStatus(Y);
		}break;
		case INS_LDA_ABS:
		{
			A = AbsoluteAddress(Cycles, memory);
			SetStatus(A);
		}break;
		case INS_AND_ABS:
		{
			A &= AbsoluteAddress(Cycles, memory);
			SetStatus(A);
		}break;
		case INS_EOR_ABS:
		{
			A ^= AbsoluteAddress(Cycles, memory);
			SetStatus(A);
		}break;
		case INS_ORA_ABS:
		{
			A |= AbsoluteAddress(Cycles, memory);
			SetStatus(A);
		}break;
		case INS_BIT_ABS:
		{
			uint8_t Value = AbsoluteAddress(Cycles, memory);
			Flags.Z = !(Value & A);
			Flags.N = (Value & NegativeFlagBit) != 0;
			Flags.V = (Value & OverFlowFlagBit) != 0;
		}break;
		case INS_LDX_ABS:
		{
			X = AbsoluteAddress(Cycles, memory);
			SetStatus(X);
		}break;
		case INS_LDY_ABS:
		{
			Y = AbsoluteAddress(Cycles, memory);
			SetStatus(Y);
		}break;
		case INS_INC_ABS:
		{
			uint8_t Value = AbsoluteAddress(Cycles, memory) + 1;
			uint16_t Address = ReadWord((uint16_t)(PC - 2), Cycles, memory);
			//这里不知道为什么，貌似真实的CPU写入不需要消耗周期
			WriteByte(Value, Address, Cycles, memory);
			Cycles++;
			//FIX ME
			SetStatus(Value);
		}break;
		case INS_DEC_ABS:
		{
			uint8_t Value = AbsoluteAddress(Cycles, memory) - 1;
			uint16_t Address = ReadWord((uint16_t)(PC - 2), Cycles, memory);
			//这里不知道为什么，貌似真实的CPU写入不需要消耗周期
			WriteByte(Value, Address, Cycles, memory);
			Cycles++;
			//FIX ME
			SetStatus(Value);
		}break;
		case INS_LDA_ABS_X:
		{
			A = AbsoluteAddress_Register(Cycles, memory, X);
			SetStatus(A);
		}break;
		case INS_AND_ABSX:
		{
			A &= AbsoluteAddress_Register(Cycles, memory, X);
			SetStatus(A);
		}break;
		case INS_EOR_ABSX:
		{
			A ^= AbsoluteAddress_Register(Cycles, memory, X);
			SetStatus(A);
		}break;
		case INS_ORA_ABSX:
		{
			A |= AbsoluteAddress_Register(Cycles, memory, X);
			SetStatus(A);
		}break;
		case INS_INC_ABSX:
		{
			uint8_t Value = AbsoluteAddress_Register(Cycles, memory, X) + 1;
			uint16_t Address = ReadWord(uint16_t(PC - 2), Cycles, memory) + X;
			WriteByte(Value, Address, Cycles, memory);
			SetStatus(Value);
		}break;
		case INS_DEC_ABSX:
		{
			uint8_t Value = AbsoluteAddress_Register(Cycles, memory, X) - 1;
			uint16_t Address = ReadWord(uint16_t(PC - 2), Cycles, memory) + X;
			WriteByte(Value, Address, Cycles, memory);
			SetStatus(Value);
		}break;
		case INS_LDA_ABS_Y:
		{
			A = AbsoluteAddress_Register(Cycles, memory, Y);
			SetStatus(A);
		}break;
		case INS_AND_ABSY:
		{
			A &= AbsoluteAddress_Register(Cycles, memory, Y);
			SetStatus(A);
		}break;
		case INS_EOR_ABSY:
		{
			A ^= AbsoluteAddress_Register(Cycles, memory, Y);
			SetStatus(A);
		}break;
		case INS_ORA_ABSY:
		{
			A |= AbsoluteAddress_Register(Cycles, memory, Y);
			SetStatus(A);
		}break;
		case INS_LDX_ABS_Y:
		{
			X = AbsoluteAddress_Register(Cycles, memory, Y);
			SetStatus(X);
		}break;
		case INS_LDY_ABS_X:
		{
			Y = AbsoluteAddress_Register(Cycles, memory, X);
			SetStatus(Y);
		}break;
		case INS_LDA_IND_X:
		{
			A = IndirectAddressX(Cycles, memory);
			SetStatus(A);
		}break;
		case INS_AND_INDX:
		{
			A &= IndirectAddressX(Cycles, memory);
			SetStatus(A);
		}break;
		case INS_EOR_INDX:
		{
			A ^= IndirectAddressX(Cycles, memory);
			SetStatus(A);
		}break;
		case INS_ORA_INDX:
		{
			A |= IndirectAddressX(Cycles, memory);
			SetStatus(A);
		}break;
		case INS_LDA_IND_Y:
		{
			A = IndirectAddressY(Cycles, memory);
			SetStatus(A);
		}break;
		case INS_AND_INDY:
		{
			A &= IndirectAddressY(Cycles, memory);
			SetStatus(A);
		}break;
		case INS_EOR_INDY:
		{
			A ^= IndirectAddressY(Cycles, memory);
			SetStatus(A);
		}break;
		case INS_ORA_INDY:
		{
			A |= IndirectAddressY(Cycles, memory);
			SetStatus(A);
		}break;
		case INS_STA_ZP:
		{
			ZeroPageST_(A, Cycles, memory);
		}break;
		case INS_STA_ZPX:
		{
			ZeroPageST_Register(A, X, Cycles, memory);
		}break;
		case INS_STX_ZP:
		{
			ZeroPageST_(X, Cycles, memory);
		}break;
		case INS_STX_ZPY:
		{
			ZeroPageST_Register(X, Y, Cycles, memory);
		}break;
		case INS_STY_ZP:
		{
			ZeroPageST_(Y, Cycles, memory);
		}break;
		case INS_STY_ZPX:
		{
			ZeroPageST_Register(Y, X, Cycles, memory);
		}break;
		case INS_STA_ABS:
		{
			ST_AbsoluteAddress(A, Cycles, memory);
		}break;
		case INS_STX_ABS:
		{
			ST_AbsoluteAddress(X, Cycles, memory);
		}break;
		case INS_STY_ABS:
		{
			ST_AbsoluteAddress(Y, Cycles, memory);
		}break;
		case INS_STA_ABS_X:
		{
			ST_AbsoluteAddress_Register(A, X, Cycles, memory);
		}break;
		case INS_STA_ABS_Y:
		{
			ST_AbsoluteAddress_Register(A, Y, Cycles, memory);
		}break;
		case INS_STA_IND_X:
		{
			ST_IndirectAddressX(Cycles, memory);
		}break;
		case INS_STA_IND_Y:
		{
			ST_IndirectAddressY(Cycles, memory);
		}break;
		case INS_JMP:
		{
			PC = FetchWord(Cycles, memory);
		}break;
		case INS_JSR:
		{
			uint16_t SubAddress = FetchWord(Cycles, memory); //两个周期
			PUSHPCTOAddress(memory, Cycles);
			PC = SubAddress;
			Cycles--;
		}break;
		case INS_RTS:
		{
			uint16_t ReturnAddress = POPPCFROMAddress(Cycles, memory);
			PC = ReturnAddress + 1;
			Cycles -= 2;
		}break;
		case INS_TSX:
		{
			X = SP;
			Cycles--;
			SetStatus(X);
		}break;
		case INS_TXS:
		{
			SP = X;
			Cycles--;
		}break;
		case INS_PHA:
		{
			PUSHByteTOAddress(A, Cycles, memory);
		}break;
		case INS_PHP:
		{
			PUSHByteTOAddress(ps, Cycles, memory);

		}break;
		case INS_PLA:
		{
			uint8_t Data = POPByteFROMAddress(Cycles, memory);
			A = Data;
			Cycles--;
			SetStatus(A);
		}break;
		case INS_PLP:
		{
			uint8_t Data = POPByteFROMAddress(Cycles, memory);
			ps = Data;
			Cycles--;
		}break;
		case INS_BCC_REL:
		{
			RelativeModeClearIsJmp(CarryFlag, Cycles, memory);
		}break;
		case INS_BCS_REL:
		{
			RelativeModeSetIsJmp(CarryFlag, Cycles, memory);
		}break;
		case INS_BEQ_REL:
		{
			RelativeModeSetIsJmp(ZeroBit, Cycles, memory);
		}break;
		case INS_BMI_REL:
		{
			RelativeModeSetIsJmp(NegativeFlagBit, Cycles, memory);
		}break;
		case INS_BNE_REL:
		{
			RelativeModeClearIsJmp(ZeroBit, Cycles, memory);
		}break;
		case INS_BPL_REL:
		{
			RelativeModeClearIsJmp(NegativeFlagBit, Cycles, memory);
		}break;
		case INS_BVC_REL:
		{
			RelativeModeClearIsJmp(OverFlowFlagBit, Cycles, memory);
		}break;
		case INS_BVS_REL:
		{
			RelativeModeSetIsJmp(OverFlowFlagBit, Cycles, memory);
		}break;
		case INS_CLC_IM:
		{
			Flags.C = 0;
			Cycles--;
		}break;
		case INS_CLD_IM:
		{
			Flags.D = 0;
			Cycles--;
		}break;
		case INS_CLI_IM:
		{
			Flags.I = 0;
			Cycles--;
		}break;
		case INS_CLV_IM:
		{
			Flags.V = 0;
			Cycles--;
		}break;
		case INS_SEC_IM:
		{
			Flags.C = 1;
			Cycles--;
		}break;
		case INS_SED_IM:
		{
			Flags.D = 1;
			Cycles--;
		}break;
		case INS_SEI_IM:
		{
			Flags.I = 1;
			Cycles--;
		}break;
		case INS_NOP:
		{
			Cycles--;
		}break;
		case INS_ADC_IM:
		{
			uint32_t Value = FetchByte(Cycles, memory);
			ADC(Value);
		}break;
		case INS_ADC_ZP:
		{
			uint32_t Value = AddZeroPage(Cycles, memory);
			ADC(Value);
		}break;
		case INS_ADC_ZPX:
		{
			uint32_t Value = AddZeroPageAdd(Cycles, memory,X);
			ADC(Value);
		}break;
		case INS_ADC_ABS:
		{
			uint32_t Value = AbsoluteAddress(Cycles, memory);
			ADC(Value);
		}break;
		case INS_ADC_ABS_X:
		{
			uint32_t Value = AbsoluteAddress_Register(Cycles, memory,X);
			ADC(Value);
		}break;
		case INS_ADC_ABS_Y:
		{
			uint32_t Value = AbsoluteAddress_Register(Cycles, memory, Y);
			ADC(Value);
		}break;
		case INS_ADC_IND_X:
		{
			uint32_t Value = IndirectAddressX(Cycles, memory);
			ADC(Value);
		}break;
		case INS_ADC_IND_Y:
		{
			uint32_t Value = IndirectAddressY(Cycles, memory);
			ADC(Value);
		}break;
//////////////
		case INS_SBC_IM:
		{
			uint32_t Value = FetchByte(Cycles, memory);
			SBC(Value);
		}break;
		case INS_SBC_ZP:
		{
			uint32_t Value = AddZeroPage(Cycles, memory);
			SBC(Value);
		}break;
		case INS_SBC_ZPX:
		{
			uint32_t Value = AddZeroPageAdd(Cycles, memory, X);
			SBC(Value);
		}break;
		case INS_SBC_ABS:
		{
			uint32_t Value = AbsoluteAddress(Cycles, memory);
			SBC(Value);
		}break;
		case INS_SBC_ABS_X:
		{
			uint32_t Value = AbsoluteAddress_Register(Cycles, memory, X);
			SBC(Value);
		}break;
		case INS_SBC_ABS_Y:
		{
			uint32_t Value = AbsoluteAddress_Register(Cycles, memory, Y);
			SBC(Value);
		}break;
		case INS_SBC_IND_X:
		{
			uint32_t Value = IndirectAddressX(Cycles, memory);
			SBC(Value);
		}break;
		case INS_SBC_IND_Y:
		{
			uint32_t Value = IndirectAddressY(Cycles, memory);
			SBC(Value);
		}break;
		//////////////
		case INS_CMP_IM:
		{
			uint32_t Value = FetchByte(Cycles, memory);
			CompareRegister(Value,A);
		}break;
		case INS_CMP_ZP:
		{
			uint32_t Value = AddZeroPage(Cycles, memory);
			CompareRegister(Value, A);
		}break;
		case INS_CMP_ZPX:
		{
			uint32_t Value = AddZeroPageAdd(Cycles, memory, X);
			CompareRegister(Value, A);
		}break;
		case INS_CMP_ABS:
		{
			uint32_t Value = AbsoluteAddress(Cycles, memory);
			CompareRegister(Value, A);
		}break;
		case INS_CMP_ABS_X:
		{
			uint32_t Value = AbsoluteAddress_Register(Cycles, memory, X);
			CompareRegister(Value, A);
		}break;
		case INS_CMP_ABS_Y:
		{
			uint32_t Value = AbsoluteAddress_Register(Cycles, memory, Y);
			CompareRegister(Value, A);
		}break;
		case INS_CMP_IND_X:
		{
			uint32_t Value = IndirectAddressX(Cycles, memory);
			CompareRegister(Value, A);
		}break;
		case INS_CMP_IND_Y:
		{
			uint32_t Value = IndirectAddressY(Cycles, memory);
			CompareRegister(Value, A);
		}break;
		case INS_CPX_IM:
		{
			uint32_t Value = FetchByte(Cycles, memory);
			CompareRegister(Value, X);
		}break;
		case INS_CPX_ZP:
		{
			uint32_t Value = AddZeroPage(Cycles, memory);
			CompareRegister(Value, X);
		}break;
		case INS_CPX_ABS:
		{
			uint32_t Value = AbsoluteAddress(Cycles, memory);
			CompareRegister(Value, X);
		}break;
		case INS_CPY_IM:
		{
			uint32_t Value = FetchByte(Cycles, memory);
			CompareRegister(Value, Y);
		}break;
		case INS_CPY_ZP:
		{
			uint32_t Value = AddZeroPage(Cycles, memory);
			CompareRegister(Value, Y);
		}break;
		case INS_CPY_ABS:
		{
			uint32_t Value = AbsoluteAddress(Cycles, memory);
			CompareRegister(Value, Y);
		}break;
		default:
		{
			printf("没有设置 0x%x 指令\n", Ins);
			Cycles++;//由于前面的使用了一次周期读取无用数据，所以要加回来
			//goto finish;
			throw - 1;
		}break;
		}
	}
	return CyclesRequests - Cycles;
}

void CPU::Reset(Mem& mem)
{
	PC = 0xFFFC; //将PC寄存器初始化 LDX #$FF
	SP = 0x00FF; //将栈寄存器初始化
	Flags.C = Flags.Z = Flags.I = Flags.D = Flags.B = Flags.V = Flags.N = 0;		 //将标志寄存器初始化，
				 //实际上是某个指令实现的初始化，这里暂时这样做
				 //比如D=0可以写成CLD
	A = X = Y = 0;
	mem.Init();
}

void CPU::SetStatus(uint8_t _register)
{
	Flags.Z = (_register == 0);
	Flags.N = (_register & 0b10000000) > 0;
}
uint8_t CPU::FetchByte(int32_t& Cycles, Mem& memory)
{
	uint8_t Data = memory[PC];
	PC++;
	Cycles--;
	return Data;
}
uint8_t CPU::ReadByte(uint8_t address, int32_t& Cycles, Mem& memory)
{
	uint8_t Data = memory[address];
	Cycles--;
	return Data;
}
uint8_t CPU::ReadByte(uint16_t address, int32_t& Cycles, Mem& memory)
{
	uint8_t Data = memory[address];
	Cycles--;
	return Data;
}
uint16_t CPU::ReadWord(uint16_t address, int32_t& Cycles, Mem& memory)
{
	uint16_t Data = memory[address];
	Data |= (memory[address + 1] << 8);
	Cycles -= 2;
	return Data;
}
uint16_t CPU::ReadWord(uint8_t address, int32_t& Cycles, Mem& memory)
{
	uint16_t Data = memory[address];
	Data |= (memory[address + 1] << 8);
	Cycles -= 2;
	return Data;
}
uint16_t CPU::FetchWord(int32_t& Cycles, Mem& memory)
{
	//小端存储数据  如果是大端口，需要调换数据顺序
	uint16_t Data = memory[PC++];
	Cycles--;
	Data |= (memory[PC++] << 8);
	Cycles--;
	return Data;
}
uint16_t CPU::SPTOAddress()
{
	return 0x100 | SP;
}
//写入两个字节
void CPU::WriteWord(const uint16_t Value, const uint32_t Address, int32_t& Cycles, Mem& memory)
{
	memory.Data[Address] = Value & 0xFF;
	memory.Data[Address + 1] = (Value >> 8);
	Cycles -= 2;
}
void CPU::WriteByte(const uint8_t Value, const uint32_t Address, int32_t& Cycles, Mem& memory)
{
	memory.Data[Address] = Value;
	Cycles--;
}

void CPU::PUSHByteTOAddress(uint8_t Data, int32_t& Cycles, Mem& memory)
{
	WriteByte(Data, SPTOAddress(), Cycles, memory); //两个周期
	SP--;
	Cycles--;
}
void CPU::PUSHWordTOAddress(uint16_t Data, int32_t& Cycles, Mem& memory)
{
	WriteWord(Data, SPTOAddress() - 1, Cycles, memory); //两个周期
	SP -= 2;
}
void CPU::PUSHPCTOAddress(Mem& memory, int32_t& Cycles)
{
	PUSHWordTOAddress(PC - 1, Cycles, memory);
}
uint8_t CPU::POPByteFROMAddress(int32_t& Cycles, Mem& memory)
{
	uint8_t Value = ReadByte((uint16_t)(SPTOAddress() + 1), Cycles, memory);
	SP++;
	Cycles--;
	return Value;
}
uint16_t CPU::POPPCFROMAddress(int32_t& Cycles, Mem& memory)
{
	uint16_t Value = ReadWord((uint16_t)(SPTOAddress() + 1), Cycles, memory);
	SP += 2;
	Cycles--;
	return Value;
}
uint8_t CPU::AddZeroPage(int32_t& Cycles, Mem& memory)
{
	return ReadByte(FetchByte(Cycles, memory), Cycles, memory);
}
uint8_t CPU::AddZeroPageAdd(int32_t& Cycles, Mem& memory, uint8_t _register)
{
	uint8_t ZeroPageAddress = FetchByte(Cycles, memory);
	ZeroPageAddress += _register;
	Cycles--;
	return  ReadByte(ZeroPageAddress, Cycles, memory);
}
uint8_t CPU::AbsoluteAddress(int32_t& Cycles, Mem& memory)
{
	return ReadByte(FetchWord(Cycles, memory), Cycles, memory);
}
uint8_t CPU::AbsoluteAddress_Register(int32_t& Cycles, Mem& memory, uint8_t _register)
{
	uint16_t AbsAddress = FetchWord(Cycles, memory);
	auto Copy_AbsAddress = AbsAddress % 0x100;
	// 下面有更好的写法，这里暂时这样写 https://github.com/davepoo/6502Emulator/blob/bb278fbf80d6e4f0f3bb60826afc2551977d9ead/6502/6502Lib/src/private/m6502.cpp#L1068 
	AbsAddress += _register;
	if (Copy_AbsAddress + _register > 0xff)
		Cycles--;
	return ReadByte(AbsAddress, Cycles, memory);
}
uint8_t CPU::IndirectAddressX(int32_t& Cycles, Mem& memory)
{
	uint8_t IndAddress = FetchByte(Cycles, memory);
	IndAddress += X;
	Cycles--;
	uint16_t ind_address_ = ReadWord(
		IndAddress,
		Cycles,
		memory);
	return  ReadByte(ind_address_, Cycles, memory);
}
uint8_t CPU::IndirectAddressY(int32_t& Cycles, Mem& memory)
{
	uint8_t IndAddress = FetchByte(Cycles, memory);
	uint16_t ind_address_ = ReadWord(
		IndAddress,
		Cycles,
		memory);
	auto Copy_ind_address_ = ind_address_ % 0x100;
	ind_address_ += Y;
	if (Copy_ind_address_ + Y > 0xff)
		Cycles--;
	return  ReadByte(ind_address_, Cycles, memory);
}
void CPU::ZeroPageST_(uint8_t main_register, int32_t& Cycles, Mem& memory)
{
	uint8_t Address = FetchByte(Cycles, memory);
	WriteByte(main_register, Address, Cycles, memory);
}
void CPU::ZeroPageST_Register(uint8_t main_register, uint8_t __register, int32_t& Cycles, Mem& memory)
{
	uint8_t Address = FetchByte(Cycles, memory);
	Address += __register;
	Cycles--;
	WriteByte(main_register, Address, Cycles, memory);
}
void CPU::ST_AbsoluteAddress(uint8_t main_register, int32_t& Cycles, Mem& memory)
{
	uint16_t Address = FetchWord(Cycles, memory);
	WriteByte(main_register, Address, Cycles, memory);
}
void CPU::ST_AbsoluteAddress_Register(uint8_t main_register, uint8_t __register, int32_t& Cycles, Mem& memory)
{
	uint16_t Address = FetchWord(Cycles, memory);
	Address += __register;
	Cycles--;
	WriteByte(main_register, Address, Cycles, memory);
}
void CPU::ST_IndirectAddressX(int32_t& Cycles, Mem& memory)
{
	uint8_t IndAddress = FetchByte(Cycles, memory);
	IndAddress += X;
	Cycles--;
	uint16_t IndAddressAddress = ReadWord(IndAddress, Cycles, memory);
	WriteByte(A, IndAddressAddress, Cycles, memory);
}
void CPU::ST_IndirectAddressY(int32_t& Cycles, Mem& memory)
{
	uint8_t IndAddress = FetchByte(Cycles, memory);
	uint16_t IndAddressAddress = ReadWord(IndAddress, Cycles, memory);
	IndAddressAddress += Y;
	Cycles--;
	WriteByte(A, IndAddressAddress, Cycles,memory);
}
void m6502::CPU::RelativeModeClearIsJmp(const uint8_t FlagRegister, int32_t& Cycles, Mem& memory)
{
	uint8_t RelativeOffset = FetchByte(Cycles, memory);
	if ((FlagRegister & ps) == 0)
	{
		auto copyPC = PC;
		PC += RelativeOffset;

		if (copyPC / 0xff != PC / 0xff)
			Cycles -= 2;
		else
			Cycles--;
	}
}
void m6502::CPU::RelativeModeSetIsJmp(const uint8_t FlagRegister, int32_t& Cycles, Mem& memory)
{
	uint8_t RelativeOffset = FetchByte(Cycles, memory);
	if ((FlagRegister & ps) != 0)
	{
		auto copyPC = PC;
		PC += RelativeOffset;

		if (copyPC / 0xff != PC / 0xff)
			Cycles -= 2;
		else
			Cycles--;
	}
}