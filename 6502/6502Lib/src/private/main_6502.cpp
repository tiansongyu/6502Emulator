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
		case INS_LDA_IM: //立即寻址 Immediate
		{
			A =  FetchByte(Cycles, memory);
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
		case INS_LDA_ZP: //零页寻址 ..zero page
		{
			A = AddZeroPage(Cycles, memory);
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
		case INS_LDA_ZPX: //零页寻址 ..zero page x
		{
			A = AddZeroPageAdd(Cycles, memory, X);
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
		case INS_LDA_ABS_X:
		{
			A = AbsoluteAddress_Register( Cycles,memory, X);
			SetStatus(A);
		}break;
		case INS_LDA_ABS_Y:
		{
			A = AbsoluteAddress_Register(Cycles, memory, Y);
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
		case INS_LDA_IND_Y:
		{
			A = IndirectAddressY(Cycles, memory);
			SetStatus(A);
		}break;
		case INS_JSR:
		{
			Word SubAddress = FetchWord(Cycles, memory); //两个周期
			memory.WriteWord(PC - 1, SP, Cycles); //两个周期
			SP += 2;
			PC = SubAddress;
			Cycles--;
		}break;
		default:
		{
			printf("没有设置 0x%x 指令\n", Ins);
			Cycles++;//由于前面的使用了一次周期读取无用数据，所以要加回来
			goto finish;
		}break;
		}
	}
finish:
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
	auto Copy_AbsAddress = AbsAddress % 256;
	// 下面有更好的写法，这里暂时这样写 https://github.com/davepoo/6502Emulator/blob/bb278fbf80d6e4f0f3bb60826afc2551977d9ead/6502/6502Lib/src/private/m6502.cpp#L1068 
	AbsAddress += _register;
	if (Copy_AbsAddress + _register > 254)
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
	auto Copy_ind_address_ = ind_address_ % 256;
	ind_address_ += Y;
	if (Copy_ind_address_ + Y > 254)
		Cycles--;
	return  ReadByte(ind_address_, Cycles, memory);
}

