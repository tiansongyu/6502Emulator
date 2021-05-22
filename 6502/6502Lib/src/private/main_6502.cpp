#include "main_6502.h"
#include "Bus.h"
int32_t CPU::Execute(int32_t Cycles)
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
		Flags.N = (((Register - Value) & NegativeFlagBit )> 0);
	};
	auto ASL = [&Cycles,this](uint8_t& Value)
	{
		Flags.C = (Value & NegativeFlagBit) > 0;
		Value <<= 1;
		SetStatus(Value);
		Cycles--;
		return Value;
	};
	auto LSR = [&Cycles, this](uint8_t& Value)
	{
		Flags.C = (Value & CarryFlag) > 0;
		Value >>= 1;
		SetStatus(Value);
		Cycles--;
		return Value;
	};
	auto ROL = [&Cycles, this](uint8_t& Value)
	{
		uint8_t NewBit = Flags.C ? CarryFlag : 0;
		Flags.C = (Value & NegativeFlagBit) > 0;
		Value <<= 1;
		Value |= NewBit;
		SetStatus(Value);
		Cycles--;
		return Value;
	};
	auto ROR = [&Cycles, this](uint8_t& Value)
	{
		bool OldBit = (Value & CarryFlag) > 0;
		Value >>= 1;
		Value |= Flags.C ? NegativeFlagBit : 0;
		Flags.C = OldBit;
		SetStatus(Value);
		Cycles--;
		return Value;
	};
	int32_t CyclesRequests = Cycles;
	while (Cycles > 0)
	{
		uint8_t Ins = FetchByte(Cycles);
		switch (Ins)
		{
		case INS_LDA_IM: //立即寻址 Immediate
		{
			A = FetchByte(Cycles);
			SetStatus(A);
		}break;
		case INS_AND_IMM: //立即寻址 AND Immediate
		{
			A &= FetchByte(Cycles);
			SetStatus(A);
		}break;
		case INS_EOR_IMM: //立即寻址 AND Immediate
		{
			A ^= FetchByte(Cycles);
			SetStatus(A);
		}break;
		case INS_ORA_IMM: //立即寻址 AND Immediate
		{
			A |= FetchByte(Cycles);
			SetStatus(A);
		}break;
		case INS_LDX_IM: //立即寻址 Immediate
		{
			X = FetchByte(Cycles);
			SetStatus(X);
		}break;
		case INS_LDY_IM: //立即寻址 Immediate
		{
			Y = FetchByte(Cycles);
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
			A = AddZeroPage(Cycles);
			SetStatus(A);
		}break;
		case INS_AND_ZP: //零页寻址 ..zero page
		{
			A &= AddZeroPage(Cycles);
			SetStatus(A);
		}break;
		case INS_EOR_ZP: //零页寻址 ..zero page
		{
			A ^= AddZeroPage(Cycles);
			SetStatus(A);
		}break;
		case INS_BIT_ZP:
		{
			uint8_t Value = AddZeroPage(Cycles);
			Flags.Z = !(Value & A);
			Flags.N = (Value & NegativeFlagBit) != 0;
			Flags.V = (Value & OverFlowFlagBit) != 0;
		}break;
		case INS_ORA_ZP: //零页寻址 ..zero page
		{
			A |= AddZeroPage(Cycles);
			SetStatus(A);
		}break;
		case INS_LDX_ZP: //零页寻址 ..zero page
		{
			X = AddZeroPage(Cycles);

			SetStatus(X);
		}break;
		case INS_LDY_ZP: //零页寻址 ..zero page
		{
			Y = AddZeroPage(Cycles);
			SetStatus(Y);
		}break;
		//INC START
		case INS_INC_ZP: //零页寻址 ..zero page
		{
			uint8_t Value = AddZeroPage(Cycles) + 1;
			uint8_t Address = bus->read(PC - 1, false);
			Cycles--;
			//Byte Address = ReadByte(memory[PC - 1], Cycles);
			WriteByte(Value, Address, Cycles);
			SetStatus(Value);
		}break;
		case INS_DEC_ZP: //零页寻址 ..zero page
		{
			uint8_t Value = AddZeroPage(Cycles) - 1;
			uint8_t Address = bus->read(PC - 1, false);
			Cycles--;
			//Byte Address = ReadByte(memory[PC - 1], Cycles);
			WriteByte(Value, Address, Cycles);
			SetStatus(Value);
		}break;
		// INC END
		case INS_LDA_ZPX: //零页寻址 ..zero page x
		{
			A = AddZeroPageAdd(Cycles, X);
			SetStatus(A);
		}break;
		case INS_INC_ZPX: //零页寻址 ..zero page x
		{
			uint8_t Value = AddZeroPageAdd(Cycles, X) + 1;
			uint8_t Address = bus->read(PC - 1, false) + X;
			Cycles--;
			WriteByte(Value, Address, Cycles);
			SetStatus(Value);
		}break;
		case INS_DEC_ZPX: //零页寻址 
		{
			uint8_t Value = AddZeroPageAdd(Cycles, X) - 1;
			uint8_t Address = bus->read(PC - 1, false)  + X;
			Cycles--;
			WriteByte(Value, Address, Cycles);
			SetStatus(Value);
		}break;
		case INS_AND_ZPX: //零页寻址 ..zero page x
		{
			A &= AddZeroPageAdd(Cycles, X);
			SetStatus(A);
		}break;
		case INS_EOR_ZPX: //零页寻址 ..zero page x
		{
			A ^= AddZeroPageAdd(Cycles, X);
			SetStatus(A);
		}break;
		case INS_ORA_ZPX: //零页寻址 ..zero page x
		{
			A |= AddZeroPageAdd(Cycles, X);
			SetStatus(A);
		}break;
		case INS_LDX_ZPY: //零页寻址 ..zero page x
		{
			X = AddZeroPageAdd(Cycles, Y);
			SetStatus(X);
		}break;
		case INS_LDY_ZPX: //零页寻址 ..zero page x
		{
			Y = AddZeroPageAdd(Cycles, X);
			SetStatus(Y);
		}break;
		case INS_LDA_ABS:
		{
			A = AbsoluteAddress(Cycles);
			SetStatus(A);
		}break;
		case INS_AND_ABS:
		{
			A &= AbsoluteAddress(Cycles);
			SetStatus(A);
		}break;
		case INS_EOR_ABS:
		{
			A ^= AbsoluteAddress(Cycles);
			SetStatus(A);
		}break;
		case INS_ORA_ABS:
		{
			A |= AbsoluteAddress(Cycles);
			SetStatus(A);
		}break;
		case INS_BIT_ABS:
		{
			uint8_t Value = AbsoluteAddress(Cycles);
			Flags.Z = !(Value & A);
			Flags.N = (Value & NegativeFlagBit) != 0;
			Flags.V = (Value & OverFlowFlagBit) != 0;
		}break;
		case INS_LDX_ABS:
		{
			X = AbsoluteAddress(Cycles);
			SetStatus(X);
		}break;
		case INS_LDY_ABS:
		{
			Y = AbsoluteAddress(Cycles);
			SetStatus(Y);
		}break;
		case INS_INC_ABS:
		{
			uint8_t Value = AbsoluteAddress(Cycles) + 1;
			uint16_t Address = ReadWord((uint16_t)(PC - 2), Cycles);
			//这里不知道为什么，貌似真实的CPU写入不需要消耗周期
			WriteByte(Value, Address, Cycles);
			Cycles++;
			//FIX ME
			SetStatus(Value);
		}break;
		case INS_DEC_ABS:
		{
			uint8_t Value = AbsoluteAddress(Cycles) - 1;
			uint16_t Address = ReadWord((uint16_t)(PC - 2), Cycles);
			//这里不知道为什么，貌似真实的CPU写入不需要消耗周期
			WriteByte(Value, Address, Cycles);
			Cycles++;
			//FIX ME
			SetStatus(Value);
		}break;
		case INS_LDA_ABS_X:
		{
			A = AbsoluteAddress_Register(Cycles, X);
			SetStatus(A);
		}break;
		case INS_AND_ABSX:
		{
			A &= AbsoluteAddress_Register(Cycles, X);
			SetStatus(A);
		}break;
		case INS_EOR_ABSX:
		{
			A ^= AbsoluteAddress_Register(Cycles, X);
			SetStatus(A);
		}break;
		case INS_ORA_ABSX:
		{
			A |= AbsoluteAddress_Register(Cycles, X);
			SetStatus(A);
		}break;
		case INS_INC_ABSX:
		{
			uint8_t Value = AbsoluteAddress_Register(Cycles, X) + 1;
			uint16_t Address = ReadWord(uint16_t(PC - 2), Cycles) + X;
			WriteByte(Value, Address, Cycles);
			SetStatus(Value);
		}break;
		case INS_DEC_ABSX:
		{
			uint8_t Value = AbsoluteAddress_Register(Cycles, X) - 1;
			uint16_t Address = ReadWord(uint16_t(PC - 2), Cycles) + X;
			WriteByte(Value, Address, Cycles);
			SetStatus(Value);
		}break;
		case INS_LDA_ABS_Y:
		{
			A = AbsoluteAddress_Register(Cycles, Y);
			SetStatus(A);
		}break;
		case INS_AND_ABSY:
		{
			A &= AbsoluteAddress_Register(Cycles, Y);
			SetStatus(A);
		}break;
		case INS_EOR_ABSY:
		{
			A ^= AbsoluteAddress_Register(Cycles, Y);
			SetStatus(A);
		}break;
		case INS_ORA_ABSY:
		{
			A |= AbsoluteAddress_Register(Cycles, Y);
			SetStatus(A);
		}break;
		case INS_LDX_ABS_Y:
		{
			X = AbsoluteAddress_Register(Cycles, Y);
			SetStatus(X);
		}break;
		case INS_LDY_ABS_X:
		{
			Y = AbsoluteAddress_Register(Cycles, X);
			SetStatus(Y);
		}break;
		case INS_LDA_IND_X:
		{
			A = IndirectAddressX(Cycles);
			SetStatus(A);
		}break;
		case INS_AND_INDX:
		{
			A &= IndirectAddressX(Cycles);
			SetStatus(A);
		}break;
		case INS_EOR_INDX:
		{
			A ^= IndirectAddressX(Cycles);
			SetStatus(A);
		}break;
		case INS_ORA_INDX:
		{
			A |= IndirectAddressX(Cycles);
			SetStatus(A);
		}break;
		case INS_LDA_IND_Y:
		{
			A = IndirectAddressY(Cycles);
			SetStatus(A);
		}break;
		case INS_AND_INDY:
		{
			A &= IndirectAddressY(Cycles);
			SetStatus(A);
		}break;
		case INS_EOR_INDY:
		{
			A ^= IndirectAddressY(Cycles);
			SetStatus(A);
		}break;
		case INS_ORA_INDY:
		{
			A |= IndirectAddressY(Cycles);
			SetStatus(A);
		}break;
		case INS_STA_ZP:
		{
			ZeroPageST(A, Cycles);
		}break;
		case INS_STA_ZPX:
		{
			ZeroPageSTRegister(A, X, Cycles);
		}break;
		case INS_STX_ZP:
		{
			ZeroPageST(X, Cycles);
		}break;
		case INS_STX_ZPY:
		{
			ZeroPageSTRegister(X, Y, Cycles);
		}break;
		case INS_STY_ZP:
		{
			ZeroPageST(Y, Cycles);
		}break;
		case INS_STY_ZPX:
		{
			ZeroPageSTRegister(Y, X, Cycles);
		}break;
		case INS_STA_ABS:
		{
			STAbsoluteAddress(A, Cycles);
		}break;
		case INS_STX_ABS:
		{
			STAbsoluteAddress(X, Cycles);
		}break;
		case INS_STY_ABS:
		{
			STAbsoluteAddress(Y, Cycles);
		}break;
		case INS_STA_ABS_X:
		{
			STAbsoluteAddress_Register(A, X, Cycles);
		}break;
		case INS_STA_ABS_Y:
		{
			STAbsoluteAddress_Register(A, Y, Cycles);
		}break;
		case INS_STA_IND_X:
		{
			STIndirectAddressX(Cycles);
		}break;
		case INS_STA_IND_Y:
		{
			STIndirectAddressY(Cycles);
		}break;
		case INS_JMP:
		{
			PC = FetchWord(Cycles);
		}break;
		case INS_JSR:
		{
			uint16_t SubAddress = FetchWord(Cycles); //两个周期
			PUSHWordTOAddress(PC-1, Cycles);
			PC = SubAddress;
			Cycles--;
		}break;
		case INS_RTS:
		{
			uint16_t ReturnAddress = PopWordFromAddress(Cycles);
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
			PushByteToAddress(A, Cycles);
		}break;
		case INS_PHP:
		{
			PushByteToAddress(ps, Cycles);

		}break;
		case INS_PLA:
		{
			uint8_t Data = PopByteFromAddress(Cycles);
			A = Data;
			Cycles--;
			SetStatus(A);
		}break;
		case INS_PLP:
		{
			uint8_t Data = PopByteFromAddress(Cycles);
			ps = Data;
			Cycles--;
		}break;
		case INS_BCC_REL:
		{
			RelativeModeClearIsJmp(CarryFlag, Cycles);
		}break;
		case INS_BCS_REL:
		{
			RelativeModeSetIsJmp(CarryFlag, Cycles);
		}break;
		case INS_BEQ_REL:
		{
			RelativeModeSetIsJmp(ZeroBit, Cycles);
		}break;
		case INS_BMI_REL:
		{
			RelativeModeSetIsJmp(NegativeFlagBit, Cycles);
		}break;
		case INS_BNE_REL:
		{
			RelativeModeClearIsJmp(ZeroBit, Cycles);
		}break;
		case INS_BPL_REL:
		{
			RelativeModeClearIsJmp(NegativeFlagBit, Cycles);
		}break;
		case INS_BVC_REL:
		{
			RelativeModeClearIsJmp(OverFlowFlagBit, Cycles);
		}break;
		case INS_BVS_REL:
		{
			RelativeModeSetIsJmp(OverFlowFlagBit, Cycles);
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
			uint32_t Value = FetchByte(Cycles);
			ADC(Value);
		}break;
		case INS_ADC_ZP:
		{
			uint32_t Value = AddZeroPage(Cycles);
			ADC(Value);
		}break;
		case INS_ADC_ZPX:
		{
			uint32_t Value = AddZeroPageAdd(Cycles,X);
			ADC(Value);
		}break;
		case INS_ADC_ABS:
		{
			uint32_t Value = AbsoluteAddress(Cycles);
			ADC(Value);
		}break;
		case INS_ADC_ABS_X:
		{
			uint32_t Value = AbsoluteAddress_Register(Cycles,X);
			ADC(Value);
		}break;
		case INS_ADC_ABS_Y:
		{
			uint32_t Value = AbsoluteAddress_Register(Cycles, Y);
			ADC(Value);
		}break;
		case INS_ADC_IND_X:
		{
			uint32_t Value = IndirectAddressX(Cycles);
			ADC(Value);
		}break;
		case INS_ADC_IND_Y:
		{
			uint32_t Value = IndirectAddressY(Cycles);
			ADC(Value);
		}break;
//////////////
		case INS_SBC_IM:
		{
			uint32_t Value = FetchByte(Cycles);
			SBC(Value);
		}break;
		case INS_SBC_ZP:
		{
			uint32_t Value = AddZeroPage(Cycles);
			SBC(Value);
		}break;
		case INS_SBC_ZPX:
		{
			uint32_t Value = AddZeroPageAdd(Cycles, X);
			SBC(Value);
		}break;
		case INS_SBC_ABS:
		{
			uint32_t Value = AbsoluteAddress(Cycles);
			SBC(Value);
		}break;
		case INS_SBC_ABS_X:
		{
			uint32_t Value = AbsoluteAddress_Register(Cycles, X);
			SBC(Value);
		}break;
		case INS_SBC_ABS_Y:
		{
			uint32_t Value = AbsoluteAddress_Register(Cycles, Y);
			SBC(Value);
		}break;
		case INS_SBC_IND_X:
		{
			uint32_t Value = IndirectAddressX(Cycles);
			SBC(Value);
		}break;
		case INS_SBC_IND_Y:
		{
			uint32_t Value = IndirectAddressY(Cycles);
			SBC(Value);
		}break;
		//////////////
		case INS_CMP_IM:
		{
			uint32_t Value = FetchByte(Cycles);
			CompareRegister(Value,A);
		}break;
		case INS_CMP_ZP:
		{
			uint32_t Value = AddZeroPage(Cycles);
			CompareRegister(Value, A);
		}break;
		case INS_CMP_ZPX:
		{
			uint32_t Value = AddZeroPageAdd(Cycles, X);
			CompareRegister(Value, A);
		}break;
		case INS_CMP_ABS:
		{
			uint32_t Value = AbsoluteAddress(Cycles);
			CompareRegister(Value, A);
		}break;
		case INS_CMP_ABS_X:
		{
			uint32_t Value = AbsoluteAddress_Register(Cycles, X);
			CompareRegister(Value, A);
		}break;
		case INS_CMP_ABS_Y:
		{
			uint32_t Value = AbsoluteAddress_Register(Cycles, Y);
			CompareRegister(Value, A);
		}break;
		case INS_CMP_IND_X:
		{
			uint32_t Value = IndirectAddressX(Cycles);
			CompareRegister(Value, A);
		}break;
		case INS_CMP_IND_Y:
		{
			uint32_t Value = IndirectAddressY(Cycles);
			CompareRegister(Value, A);
		}break;
		case INS_CPX_IM:
		{
			uint32_t Value = FetchByte(Cycles);
			CompareRegister(Value, X);
		}break;
		case INS_CPX_ZP:
		{
			uint32_t Value = AddZeroPage(Cycles);
			CompareRegister(Value, X);
		}break;
		case INS_CPX_ABS:
		{
			uint32_t Value = AbsoluteAddress(Cycles);
			CompareRegister(Value, X);
		}break;
		case INS_CPY_IM:
		{
			uint32_t Value = FetchByte(Cycles);
			CompareRegister(Value, Y);
		}break;
		case INS_CPY_ZP:
		{
			uint32_t Value = AddZeroPage(Cycles);
			CompareRegister(Value, Y);
		}break;
		case INS_CPY_ABS:
		{
			uint32_t Value = AbsoluteAddress(Cycles);
			CompareRegister(Value, Y);
		}break;
		case INS_ASL_ACC:
		{
			A = ASL(A);
		}break;
		case INS_ASL_ZP:
		{
			uint8_t Value = AddZeroPage(Cycles);
			uint8_t result = ASL(Value);
			WriteByte(result, bus->read(PC - 1, false), Cycles);
		}break;
		case INS_ASL_ZPX:
		{
			uint8_t Value = AddZeroPageAdd(Cycles,X);
			uint8_t result = ASL(Value);
			WriteByte(result, bus->read(PC - 1, false), Cycles);
		}break;
		case INS_ASL_ABS:
		{
			uint8_t Value = AbsoluteAddress(Cycles);
			uint8_t result = ASL(Value); int tmp = 2;
			uint16_t Address = ReadWord((uint16_t)(PC - 2), tmp);
			WriteByte(result, Address, Cycles);
		}break;
		case INS_ASL_ABSX:
		{
			uint8_t Value = AbsoluteAddress_RegisterShifts(Cycles,X);
			uint8_t result = ASL(Value); int tmp = 2;
			uint16_t Address = ReadWord((uint16_t)(PC - 2), tmp);
			WriteByte(result, Address+X, Cycles);
		}break;
		case INS_LSR_ACC:
		{
			A = LSR(A);
		}break;
		case INS_LSR_ZP:
		{
			uint8_t Value = AddZeroPage(Cycles);
			uint8_t result = LSR(Value);
			WriteByte(result, bus->read(PC - 1, false), Cycles);
		}break;
		case INS_LSR_ZPX:
		{
			uint8_t Value = AddZeroPageAdd(Cycles, X);
			uint8_t result = LSR(Value);
			WriteByte(result, bus->read(PC - 1, false), Cycles);
		}break;
		case INS_LSR_ABS:
		{
			uint8_t Value = AbsoluteAddress(Cycles);
			uint8_t result = LSR(Value); int tmp = 2;
			uint16_t Address = ReadWord((uint16_t)(PC - 2), tmp);
			WriteByte(result, Address, Cycles);
		}break;
		case INS_LSR_ABSX:
		{
			uint8_t Value = AbsoluteAddress_RegisterShifts(Cycles, X);
			uint8_t result = LSR(Value); int tmp = 2;
			uint16_t Address = ReadWord((uint16_t)(PC - 2), tmp);
			WriteByte(result, Address+X, Cycles);
		}break;
		case INS_ROL_ACC:
		{
			A = ROL(A);
		}break;
		case INS_ROL_ZP:
		{
			uint8_t Value = AddZeroPage(Cycles);
			uint8_t result = ROL(Value);
			WriteByte(result, bus->read(PC - 1, false), Cycles);
		}break;
		case INS_ROL_ZPX:
		{
			uint8_t Value = AddZeroPageAdd(Cycles, X);
			uint8_t result = ROL(Value);
			WriteByte(result, bus->read(PC - 1, false), Cycles);
		}break;
		case INS_ROL_ABS:
		{
			uint8_t Value = AbsoluteAddress(Cycles);
			uint8_t result = ROL(Value); int tmp = 2;
			uint16_t Address = ReadWord((uint16_t)(PC - 2), tmp);
			WriteByte(result, Address, Cycles);
		}break;
		case INS_ROL_ABSX:
		{
			uint8_t Value = AbsoluteAddress_RegisterShifts(Cycles, X);
			uint8_t result = ROL(Value); int tmp = 2;
			uint16_t Address = ReadWord((uint16_t)(PC - 2), tmp);
			WriteByte(result, Address+ X, Cycles);
		}break;
		case INS_ROR_ACC:
		{
			A = ROR(A);
		}break;
		case INS_ROR_ZP:
		{
			uint8_t Value = AddZeroPage(Cycles);
			uint8_t result = ROR(Value);
			WriteByte(result, bus->read(PC - 1, false), Cycles);
		}break;
		case INS_ROR_ZPX:
		{
			uint8_t Value = AddZeroPageAdd(Cycles, X);
			uint8_t result = ROR(Value);
			WriteByte(result, bus->read(PC - 1, false), Cycles);
		}break;
		case INS_ROR_ABS:
		{
			uint8_t Value = AbsoluteAddress(Cycles);
			uint8_t result = ROR(Value); int tmp = 2;
			uint16_t Address = ReadWord((uint16_t)(PC - 2), tmp);
			WriteByte(result, Address, Cycles);
		}break;
		case INS_ROR_ABSX:
		{
			uint8_t Value = AbsoluteAddress_RegisterShifts(Cycles, X);
			uint8_t result = ROR(Value); int tmp = 2;
			uint16_t Address = ReadWord((uint16_t)(PC - 2), tmp);
			WriteByte(result, Address+X, Cycles);
		}break;
		case INS_BRK_IM:
		{
			PUSHWordTOAddress(PC + 1, Cycles); //PC+2压入栈，由于在取值时PC已经+1，所以这里只需要+1
			PushByteToAddress(ps, Cycles);
			PC = ReadWord((uint16_t)0xFFFE, Cycles);
			Flags.B = true;
		}break;
		case INS_RTI_IM:
		{
			ps = PopByteFromAddress(Cycles);
			PC = PopWordFromAddress(Cycles);
		}break;
		default:
		{
			printf("没有设置 0x%x 指令\n", Ins);
			throw - 1;
		}break;
		}
	}
	return CyclesRequests - Cycles;
}

void CPU::Reset()
{
	PC = 0xFFFC; //将PC寄存器初始化 LDX #$FF
	SP = 0x00FF; //将栈寄存器初始化
	Flags.C = Flags.Z = Flags.I = Flags.D = Flags.B = Flags.V = Flags.N = 0;		 //将标志寄存器初始化，
				 //实际上是某个指令实现的初始化，这里暂时这样做
				 //比如D=0可以写成CLD
	A = X = Y = 0;

}

void CPU::SetStatus(uint8_t _register)
{
	Flags.Z = (_register == 0);
	Flags.N = (_register & 0b10000000) > 0;
}
uint8_t CPU::FetchByte(int32_t& Cycles)
{
	uint8_t Data = bus->read(PC++, false);
	Cycles--;
	return Data;
}
uint8_t CPU::ReadByte(uint8_t address, int32_t& Cycles)
{
	uint8_t Data = bus->read(address, false);
	Cycles--;
	return Data;
}
uint8_t CPU::ReadByte(uint16_t address, int32_t& Cycles)
{
	uint8_t Data = bus->read(address, false);
	Cycles--;
	return Data;
}
uint16_t CPU::ReadWord(uint16_t address, int32_t& Cycles)
{
	uint16_t Data = bus->read(address, false);
	Data |= (bus->read(address+1, false) << 8);
	Cycles -= 2;
	return Data;
}
uint16_t CPU::ReadWord(uint8_t address, int32_t& Cycles)
{
	uint16_t Data = bus->read(address, false);
	Data |= (bus->read(address+1, false) << 8);
	Cycles -= 2;
	return Data;
}
uint16_t CPU::FetchWord(int32_t& Cycles)
{
	//小端存储数据  如果是大端口，需要调换数据顺序
	uint16_t Data = bus->read(PC++, false);
	Cycles--;
	Data |= (bus->read(PC++, false) << 8);
	Cycles--;
	return Data;
}
uint16_t CPU::SPTOAddress()
{
	return 0x100 | SP;
}
void CPU::ConnectBus(Bus* bus)
{
	this->bus = bus;
}
//写入两个字节
void CPU::WriteWord(const uint16_t Value, const uint32_t Address, int32_t& Cycles)
{
	bus->write(Address, Value & 0xFF);
	//memory.Data[Address] = ;
	bus->write(Address+1, (Value >> 8));
	//memory.Data[Address + 1] = ;
	Cycles -= 2;
}
void CPU::WriteByte(const uint8_t Value, const uint32_t Address, int32_t& Cycles)
{
	bus->write(Address, Value);
	//memory.Data[Address] = Value;
	Cycles--;
}

void CPU::PushByteToAddress(uint8_t Data, int32_t& Cycles)
{
	WriteByte(Data, SPTOAddress(), Cycles); //两个周期
	SP--;
	Cycles--;
}
void CPU::PUSHWordTOAddress(uint16_t Data, int32_t& Cycles)
{
	WriteWord(Data, SPTOAddress() - 1, Cycles); //两个周期
	SP -= 2;
}

uint8_t CPU::PopByteFromAddress(int32_t& Cycles)
{
	uint8_t Value = ReadByte((uint16_t)(SPTOAddress() + 1), Cycles);
	SP++;
	Cycles--;
	return Value;
}
uint16_t CPU::PopWordFromAddress(int32_t& Cycles)
{
	uint16_t Value = ReadWord((uint16_t)(SPTOAddress() + 1), Cycles);
	SP += 2;
	Cycles--;
	return Value;
}
uint8_t CPU::AddZeroPage(int32_t& Cycles)
{
	uint8_t Address = FetchByte(Cycles);
	return ReadByte(Address, Cycles);
}
uint8_t CPU::AddZeroPageAdd(int32_t& Cycles, uint8_t _register)
{
	uint8_t ZeroPageAddress = FetchByte(Cycles);
	ZeroPageAddress += _register;
	Cycles--;
	return  ReadByte(ZeroPageAddress, Cycles);
}
uint8_t CPU::AbsoluteAddress(int32_t& Cycles)
{
	return ReadByte(FetchWord(Cycles), Cycles);
}
uint8_t CPU::AbsoluteAddress_Register(int32_t& Cycles, uint8_t _register)
{
	uint16_t AbsAddress = FetchWord(Cycles);
	auto Copy_AbsAddress = AbsAddress % 0x100;
	// 下面有更好的写法，这里暂时这样写 https://github.com/davepoo/6502Emulator/blob/bb278fbf80d6e4f0f3bb60826afc2551977d9ead/6502/6502Lib/src/private/m6502.cpp#L1068 
	AbsAddress += _register;
	if (Copy_AbsAddress + _register > 0xFF)
		Cycles--;
	return ReadByte(AbsAddress, Cycles);
}
uint8_t CPU::AbsoluteAddress_RegisterShifts(int32_t& Cycles, uint8_t _register)
{
	uint16_t AbsAddress = FetchWord(Cycles);
	auto Copy_AbsAddress = AbsAddress % 0x100;
	// 下面有更好的写法，这里暂时这样写 https://github.com/davepoo/6502Emulator/blob/bb278fbf80d6e4f0f3bb60826afc2551977d9ead/6502/6502Lib/src/private/m6502.cpp#L1068 
	AbsAddress += _register;
	Cycles--;
	return ReadByte(AbsAddress, Cycles);
}
uint8_t CPU::IndirectAddressX(int32_t& Cycles)
{
	uint8_t IndAddress = FetchByte(Cycles);
	IndAddress += X;
	Cycles--;
	uint16_t ind_address_ = ReadWord(
		IndAddress,
		Cycles);
	return  ReadByte(ind_address_, Cycles);
}
uint8_t CPU::IndirectAddressY(int32_t& Cycles)
{
	uint8_t IndAddress = FetchByte(Cycles);
	uint16_t ind_address_ = ReadWord(
		IndAddress,
		Cycles);
	auto Copy_ind_address_ = ind_address_ % 0x100;
	ind_address_ += Y;
	if (Copy_ind_address_ + Y > 0xFF)
		Cycles--;
	return  ReadByte(ind_address_, Cycles);
}
void CPU::ZeroPageST(uint8_t main_register, int32_t& Cycles)
{
	uint8_t Address = FetchByte(Cycles);
	WriteByte(main_register, Address, Cycles);
}
void CPU::ZeroPageSTRegister(uint8_t main_register, uint8_t __register, int32_t& Cycles)
{
	uint8_t Address = FetchByte(Cycles);
	Address += __register;
	Cycles--;
	WriteByte(main_register, Address, Cycles);
}
void CPU::STAbsoluteAddress(uint8_t main_register, int32_t& Cycles)
{
	uint16_t Address = FetchWord(Cycles);
	WriteByte(main_register, Address, Cycles);
}
void CPU::STAbsoluteAddress_Register(uint8_t main_register, uint8_t __register, int32_t& Cycles)
{
	uint16_t Address = FetchWord(Cycles);
	Address += __register;
	Cycles--;
	WriteByte(main_register, Address, Cycles);
}
void CPU::STIndirectAddressX(int32_t& Cycles)
{
	uint8_t IndAddress = FetchByte(Cycles);
	IndAddress += X;
	Cycles--;
	uint16_t IndAddressAddress = ReadWord(IndAddress, Cycles);
	WriteByte(A, IndAddressAddress, Cycles);
}
void CPU::STIndirectAddressY(int32_t& Cycles)
{
	uint8_t IndAddress = FetchByte(Cycles);
	uint16_t IndAddressAddress = ReadWord(IndAddress, Cycles);
	IndAddressAddress += Y;
	Cycles--;
	WriteByte(A, IndAddressAddress, Cycles);
}
void CPU::RelativeModeClearIsJmp(const uint8_t FlagRegister, int32_t& Cycles)
{
	uint8_t RelativeOffset = FetchByte(Cycles);
	if ((FlagRegister & ps) == 0)
	{
		auto copyPC = PC;
		PC += RelativeOffset;
		(copyPC / 0xFF != PC / 0xFF) ? Cycles -= 2 : Cycles--;
	}
}
void CPU::RelativeModeSetIsJmp(const uint8_t FlagRegister, int32_t& Cycles)
{
	uint8_t RelativeOffset = FetchByte(Cycles);
	if ((FlagRegister & ps) > 0)
	{
		auto copyPC = PC;
		PC += RelativeOffset;
		(copyPC / 0xFF != PC / 0xFF) ? Cycles -= 2 : Cycles--;	
	}
}
//just from olc
/*
std::map<uint16_t, std::string> CPU::disassemble(uint16_t nStart, uint16_t nStop)
{
	uint32_t addr = nStart;
	uint8_t value = 0x00, lo = 0x00, hi = 0x00;
	std::map<uint16_t, std::string> mapLines;
	uint16_t line_addr = 0;

	// A convenient utility to convert variables into
	// hex strings because "modern C++"'s method with 
	// streams is atrocious
	auto hex = [](uint32_t n, uint8_t d)
	{
		std::string s(d, '0');
		for (int i = d - 1; i >= 0; i--, n >>= 4)
			s[i] = "0123456789ABCDEF"[n & 0xF];
		return s;
	};

	// Starting at the specified address we read an instruction
	// byte, which in turn yields information from the lookup table
	// as to how many additional bytes we need to read and what the
	// addressing mode is. I need this info to assemble human readable
	// syntax, which is different depending upon the addressing mode

	// As the instruction is decoded, a std::string is assembled
	// with the readable output
	while (addr <= (uint32_t)nStop)
	{
		line_addr = addr;

		// Prefix line with instruction address
		std::string sInst = "$" + hex(addr, 4) + ": ";

		// Read instruction, and get its readable name
		uint8_t opcode = ; addr++;
		sInst += lookup[opcode].name + " ";

		// Get oprands from desired locations, and form the
		// instruction based upon its addressing mode. These
		// routines mimmick the actual fetch routine of the
		// 6502 in order to get accurate data as part of the
		// instruction
		if (lookup[opcode].addrmode == &olc6502::IMP)
		{
			sInst += " {IMP}";
		}
		else if (lookup[opcode].addrmode == &olc6502::IMM)
		{
			value = bus->read(addr, true); addr++;
			sInst += "#$" + hex(value, 2) + " {IMM}";
		}
		else if (lookup[opcode].addrmode == &olc6502::ZP0)
		{
			lo = bus->read(addr, true); addr++;
			hi = 0x00;
			sInst += "$" + hex(lo, 2) + " {ZP0}";
		}
		else if (lookup[opcode].addrmode == &olc6502::ZPX)
		{
			lo = bus->read(addr, true); addr++;
			hi = 0x00;
			sInst += "$" + hex(lo, 2) + ", X {ZPX}";
		}
		else if (lookup[opcode].addrmode == &olc6502::ZPY)
		{
			lo = bus->read(addr, true); addr++;
			hi = 0x00;
			sInst += "$" + hex(lo, 2) + ", Y {ZPY}";
		}
		else if (lookup[opcode].addrmode == &olc6502::IZX)
		{
			lo = bus->read(addr, true); addr++;
			hi = 0x00;
			sInst += "($" + hex(lo, 2) + ", X) {IZX}";
		}
		else if (lookup[opcode].addrmode == &olc6502::IZY)
		{
			lo = bus->read(addr, true); addr++;
			hi = 0x00;
			sInst += "($" + hex(lo, 2) + "), Y {IZY}";
		}
		else if (lookup[opcode].addrmode == &olc6502::ABS)
		{
			lo = bus->read(addr, true); addr++;
			hi = bus->read(addr, true); addr++;
			sInst += "$" + hex((uint16_t)(hi << 8) | lo, 4) + " {ABS}";
		}
		else if (lookup[opcode].addrmode == &olc6502::ABX)
		{
			lo = bus->read(addr, true); addr++;
			hi = bus->read(addr, true); addr++;
			sInst += "$" + hex((uint16_t)(hi << 8) | lo, 4) + ", X {ABX}";
		}
		else if (lookup[opcode].addrmode == &olc6502::ABY)
		{
			lo = bus->read(addr, true); addr++;
			hi = bus->read(addr, true); addr++;
			sInst += "$" + hex((uint16_t)(hi << 8) | lo, 4) + ", Y {ABY}";
		}
		else if (lookup[opcode].addrmode == &olc6502::IND)
		{
			lo = bus->read(addr, true); addr++;
			hi = bus->read(addr, true); addr++;
			sInst += "($" + hex((uint16_t)(hi << 8) | lo, 4) + ") {IND}";
		}
		else if (lookup[opcode].addrmode == &olc6502::REL)
		{
			value = bus->read(addr, true); addr++;
			sInst += "$" + hex(value, 2) + " [$" + hex(addr + value, 4) + "] {REL}";
		}

		// Add the formed string to a std::map, using the instruction's
		// address as the key. This makes it convenient to look for later
		// as the instructions are variable in length, so a straight up
		// incremental index is not sufficient.
		mapLines[line_addr] = sInst;
	}

	return mapLines;
}
*/