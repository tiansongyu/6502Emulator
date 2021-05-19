#include "main_6502.h"
using namespace m6502;
s32 CPU::Execute(s32 Cycles, Mem& memory)
{
	s32 CyclesRequests = Cycles;
	while (Cycles > 0)
	{
		Byte Ins = FetchByte(Cycles, memory);
		switch (Ins)
		{
		case INS_LDA_IM: //Á¢¼´Ñ°Ö· Immediate
		{
			A = FetchByte(Cycles, memory);
			SetStatus(A);
		}break;
		case INS_AND_IMM: //Á¢¼´Ñ°Ö· AND Immediate
		{
			A &= FetchByte(Cycles, memory);
			SetStatus(A);
		}break;
		case INS_EOR_IMM: //Á¢¼´Ñ°Ö· AND Immediate
		{
			A ^= FetchByte(Cycles, memory);
			SetStatus(A);
		}break;
		case INS_ORA_IMM: //Á¢¼´Ñ°Ö· AND Immediate
		{
			A |= FetchByte(Cycles, memory);
			SetStatus(A);
		}break;
		case INS_LDX_IM: //Á¢¼´Ñ°Ö· Immediate
		{
			X = FetchByte(Cycles, memory);
			SetStatus(X);
		}break;
		case INS_LDY_IM: //Á¢¼´Ñ°Ö· Immediate
		{
			Y = FetchByte(Cycles, memory);
			SetStatus(Y);
		}break;
		case INS_TAX_IM: //Á¢¼´Ñ°Ö· Immediate
		{
			X = A;
			Cycles--;
			SetStatus(X);
		}break;
		case INS_TAY_IM: //Á¢¼´Ñ°Ö· Immediate
		{
			Y = A;
			Cycles--;
			SetStatus(Y);
		}break;
		case INS_TXA_IM: //Á¢¼´Ñ°Ö· Immediate
		{
			A = X;
			Cycles--;
			SetStatus(A);
		}break;
		case INS_TYA_IM: //Á¢¼´Ñ°Ö· Immediate
		{
			A = Y;
			Cycles--;
			SetStatus(A);
		}break;
		case INS_LDA_ZP: //ÁãÒ³Ñ°Ö· ..zero page
		{
			A = AddZeroPage(Cycles, memory);
			SetStatus(A);
		}break;
		case INS_AND_ZP: //ÁãÒ³Ñ°Ö· ..zero page
		{
			A &= AddZeroPage(Cycles, memory);
			SetStatus(A);
		}break;
		case INS_EOR_ZP: //ÁãÒ³Ñ°Ö· ..zero page
		{
			A ^= AddZeroPage(Cycles, memory);
			SetStatus(A);
		}break;
		case INS_BIT_ZP:
		{
			Byte Value = AddZeroPage(Cycles, memory);
			Flags.Z = !(Value & A);
			Flags.N = (Value & NagativeFlagBit) != 0;
			Flags.V = (Value & OverFlowFlagBit) != 0;
		}break;
		case INS_ORA_ZP: //ÁãÒ³Ñ°Ö· ..zero page
		{
			A |= AddZeroPage(Cycles, memory);
			SetStatus(A);
		}break;
		case INS_LDX_ZP: //ÁãÒ³Ñ°Ö· ..zero page
		{
			X = AddZeroPage(Cycles, memory);

			SetStatus(X);
		}break;
		case INS_LDY_ZP: //ÁãÒ³Ñ°Ö· ..zero page
		{
			Y = AddZeroPage(Cycles, memory);
			SetStatus(Y);
		}break;
		//INC START
		case INS_INC_ZP: //ÁãÒ³Ñ°Ö· ..zero page
		{
			Byte Value = AddZeroPage(Cycles, memory) + 1;
			Byte Address = ReadByte(memory[PC - 1], Cycles, memory);
			memory.WriteByte(Value, Address, Cycles);
			SetStatus(Value);
		}break;
		// INC END
		case INS_LDA_ZPX: //ÁãÒ³Ñ°Ö· ..zero page x
		{
			A = AddZeroPageAdd(Cycles, memory, X);
			SetStatus(A);
		}break;
		case INS_INC_ZPX: //ÁãÒ³Ñ°Ö· ..zero page x
		{
			Byte Value = AddZeroPageAdd(Cycles, memory, X) + 1;
			Byte Address = ReadByte((Word)(memory[PC-1]+X), Cycles, memory);
			memory.WriteByte(Value, Address, Cycles);
			SetStatus(Value);
		}break;
		case INS_AND_ZPX: //ÁãÒ³Ñ°Ö· ..zero page x
		{
			A &= AddZeroPageAdd(Cycles, memory, X);
			SetStatus(A);
		}break;
		case INS_EOR_ZPX: //ÁãÒ³Ñ°Ö· ..zero page x
		{
			A ^= AddZeroPageAdd(Cycles, memory, X);
			SetStatus(A);
		}break;
		case INS_ORA_ZPX: //ÁãÒ³Ñ°Ö· ..zero page x
		{
			A |= AddZeroPageAdd(Cycles, memory, X);
			SetStatus(A);
		}break;
		case INS_LDX_ZPY: //ÁãÒ³Ñ°Ö· ..zero page x
		{
			X = AddZeroPageAdd(Cycles, memory, Y);
			SetStatus(X);
		}break;
		case INS_LDY_ZPX: //ÁãÒ³Ñ°Ö· ..zero page x
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
			Byte Value = AbsoluteAddress(Cycles, memory);
			Flags.Z = !(Value & A);
			Flags.N = (Value & NagativeFlagBit) != 0;
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
			Byte Value  = AbsoluteAddress(Cycles, memory)+1;
			Word Address = ReadWord(memory[PC - 2], Cycles, memory);
			memory.WriteByte(Value, Address, Cycles);
			SetStatus(Value);
		}break;
		case INS_LDA_ABS_X:
		{
			A = AbsoluteAddress_Register( Cycles,memory, X);
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
			Byte Value = AbsoluteAddress_Register(Cycles, memory, X)+1;
			Byte Address = ReadWord(memory[PC - 2], Cycles, memory);
			memory.WriteByte(Value, Address, Cycles);
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
			A = IndirectAddressX( Cycles, memory);
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
			ZeroPageST_Register(A,X, Cycles, memory);
		}break;
		case INS_STX_ZP:
		{
			ZeroPageST_(X, Cycles, memory);
		}break;
		case INS_STX_ZPY:
		{
			ZeroPageST_Register(X,Y, Cycles, memory);
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
			ST_AbsoluteAddress_Register(A,X, Cycles, memory);
		}break;
		case INS_STA_ABS_Y:
		{
			ST_AbsoluteAddress_Register(A,Y, Cycles, memory);
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
			Word SubAddress = FetchWord(Cycles, memory); //Á½¸öÖÜÆÚ
			PUSHPCTOAddress(memory, Cycles);
			PC = SubAddress ;
			Cycles--;
		}break;
		case INS_RTS :
		{
			Word ReturnAddress = POPPCFROMAddress(Cycles,memory );
			PC = ReturnAddress + 1;
			Cycles-=2;
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
			Byte Data = POPByteFROMAddress(Cycles, memory);
			A = Data;
			Cycles--;
			SetStatus(A);
		}break;
		case INS_PLP:
		{
			Byte Data = POPByteFROMAddress(Cycles, memory);
			ps = Data;
			Cycles--;
		}break;
		default:
		{
			printf("Ã»ÓÐÉèÖÃ 0x%x Ö¸Áî\n", Ins);
			Cycles++;//ÓÉÓÚÇ°ÃæµÄÊ¹ÓÃÁËÒ»´ÎÖÜÆÚ¶ÁÈ¡ÎÞÓÃÊý¾Ý£¬ËùÒÔÒª¼Ó»ØÀ´
			//goto finish;
			throw - 1;
		}break;
		}
	}
	return CyclesRequests - Cycles;
}

Byte CPU::AddZeroPage(s32& Cycles, Mem& memory)
{
	return ReadByte( FetchByte(Cycles, memory) , Cycles, memory);
}
Byte CPU::AddZeroPageAdd(s32& Cycles, Mem& memory, Byte _register)
{
	Byte ZeroPageAddress = FetchByte(Cycles, memory);
	ZeroPageAddress += _register;
	Cycles--;
	return  ReadByte(ZeroPageAddress, Cycles, memory);
}
Byte CPU::AbsoluteAddress(s32& Cycles, Mem& memory)
{
	return ReadByte(FetchWord(Cycles, memory),Cycles,memory);
}
Byte CPU::AbsoluteAddress_Register(s32& Cycles, Mem& memory,Byte _register)
{
	Word AbsAddress = FetchWord(Cycles, memory);
	auto Copy_AbsAddress = AbsAddress % 0x100;
	// ÏÂÃæÓÐ¸üºÃµÄÐ´·¨£¬ÕâÀïÔÝÊ±ÕâÑùÐ´ https://github.com/davepoo/6502Emulator/blob/bb278fbf80d6e4f0f3bb60826afc2551977d9ead/6502/6502Lib/src/private/m6502.cpp#L1068 
	AbsAddress += _register;
	if (Copy_AbsAddress + _register > 0xff)
		Cycles--;
	return ReadByte(AbsAddress,Cycles,memory);
}

Byte CPU::IndirectAddressX(s32& Cycles, Mem& memory)
{
	Byte IndAddress = FetchByte(Cycles, memory);
	IndAddress += X;
	Cycles--;
	Word ind_address_ = ReadWord(
		IndAddress,
		Cycles,
		memory);
	return  ReadByte(ind_address_,Cycles,memory);
}
Byte CPU::IndirectAddressY(s32& Cycles, Mem& memory)
{
	Byte IndAddress = FetchByte(Cycles, memory);
	Word ind_address_ = ReadWord(
		IndAddress,
		Cycles,
		memory);
	auto Copy_ind_address_ = ind_address_ % 0x100;
	ind_address_ += Y;
	if (Copy_ind_address_ + Y > 0xff)
		Cycles--;
	return  ReadByte(ind_address_, Cycles, memory);
}

void CPU::ZeroPageST_(Byte main_register, s32& Cycles, Mem& memory)
{
	Byte Address = FetchByte(Cycles, memory);
	memory.WriteByte(main_register, Address, Cycles);
}

void CPU::ZeroPageST_Register(Byte main_register, Byte __register, s32& Cycles, Mem& memory)
{
	Byte Address = FetchByte(Cycles, memory);
	Address += __register;
	Cycles--;
	memory.WriteByte(main_register, Address, Cycles);
}

void CPU::ST_AbsoluteAddress(Byte main_register, s32& Cycles, Mem& memory)
{
	Word Address = FetchWord(Cycles, memory);
	memory.WriteByte(main_register, Address, Cycles);
}

void CPU::ST_AbsoluteAddress_Register(Byte main_register, Byte __register,s32& Cycles, Mem& memory)
{
	Word Address = FetchWord(Cycles, memory);
	Address += __register;
	Cycles--;
	memory.WriteByte(main_register, Address, Cycles);
}
void CPU::ST_IndirectAddressX(s32& Cycles, Mem& memory)
{
	Byte IndAddress = FetchByte(Cycles, memory);
	IndAddress += X;
	Cycles--;
	Word IndAddressAddress = ReadWord(IndAddress, Cycles, memory);
	memory.WriteByte(A, IndAddressAddress, Cycles);
}
void CPU::ST_IndirectAddressY(s32& Cycles, Mem& memory)
{
	Byte IndAddress = FetchByte(Cycles, memory);
	Word IndAddressAddress = ReadWord(IndAddress, Cycles, memory);
	IndAddressAddress += Y;
	Cycles--;
	memory.WriteByte(A, IndAddressAddress, Cycles);
}