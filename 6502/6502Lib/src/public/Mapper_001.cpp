#include "Mapper_001.h"

Mapper_001::Mapper_001(uint8_t prgBanks, uint8_t chrBanks) : Mapper(prgBanks, chrBanks)
{
	reset();
}


Mapper_001::~Mapper_001()
{
}

void Mapper_001::reset()
{

}

bool Mapper_001::cpuMapRead(uint16_t addr, uint32_t &mapped_addr)
{

	if (addr >= 0x6000 && addr <= 0x7FFF)
	{
		mapped_addr = (addr & 0x1FFF);
		return true;
	}
	else if (addr >= 0x8000 && addr <= 0xBFFF)
	{
		// 可切换或固定到第一个bank
		switch ((regControl & 0b01100) >> 2))
		{
		case /* constant-expression */:
			/* code */
			break;
		
		default:
			break;
		}
		if( ( == 0 || ((regControl & 0b01100) >> 2)  == 1))
		{
			mapped_addr = addr & (nPRGBanks > 1 ? 0x7FFF : 0x3FFF);
		}
		else if()
		return true;
	}
	else if (addr >= 0xC000 && addr <= 0xFFFF)
	{
		// 固定到最后一个 bank 或可切换
		mapped_addr = addr & (nPRGBanks > 1 ? 0x7FFF : 0x3FFF);
		return true;
	}

	return false;
}

bool Mapper_001::cpuMapWrite(uint16_t addr, uint32_t &mapped_addr, uint8_t data)
{
	// data没有使用,map中没有寄存器，cpu不会向map写入数据
	if (addr >= 0x8000 && addr <= 0xFFFF)
	{
		mapped_addr = addr & (nPRGBanks > 1 ? 0x7FFF : 0x3FFF);
		return true;
	}

	return false;
}

bool Mapper_001::ppuMapRead(uint16_t addr, uint32_t &mapped_addr)
{
	// There is no mapping required for PPU
	// PPU Address Bus          CHR ROM
	// 0x0000 -> 0x1FFF: Map    0x0000 -> 0x1FFF
	if (addr >= 0x0000 && addr <= 0x1FFF)
	{
		mapped_addr = addr;
		return true;
	}

	return false;
}

bool Mapper_001::ppuMapWrite(uint16_t addr, uint32_t &mapped_addr)
{
	if (addr >= 0x0000 && addr <= 0x1FFF)
	{
		if (nCHRBanks == 0)
		{
			// Treat as RAM
			mapped_addr = addr;
			return true;
		}
	}

	return false;
}

