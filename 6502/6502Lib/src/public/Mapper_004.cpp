#include "Mapper_004.h"

Mapper_004::Mapper_004(uint8_t prgBanks, uint8_t chrBanks) : Mapper(prgBanks, chrBanks)
{
	vRAMStatic.resize(32 * 1024);
}

Mapper_004::~Mapper_004()
{
}

bool Mapper_004::cpuMapRead(uint16_t addr, uint32_t &mapped_addr, uint8_t &data)
{
	if (addr >= 0x6000 && addr <= 0x7FFF)
	{
		// 0x6000 -> 0x7FFF 是mapper中的ram
		// Read is from static ram on cartridge
		mapped_addr = 0xFFFFFFFF;

		// Read data from RAM
		data = vRAMStatic[addr & 0x1FFF];

		// Signal mapper has handled request
		return true;
	}
	else
	{
		// return true;
		if (nBankSelectRegister & 0b0100000)
		{
			if (data >= 0x8000 && data <= 0x9FFF) // -2
			{
				mapped_addr = (nPRGBanks - 2) * 0x2000 + (addr & 0x1FFF);
			}
			else if (data >= 0xA000 && data <= 0xBFFF) // R7
			{
				uint8_t R7 = (nBankData & 0b11111100) >> 2;
				mapped_addr = R7 * 0x2000 + (addr & 0x1FFF);
			}
			else if (data >= 0xC000 && data <= 0xDFFF) // R6
			{
				uint8_t R6 = (nBankData & 0b11111100) >> 2;
				mapped_addr = R6 * 0x2000 + (addr & 0x1FFF);
			}
			else if (data >= 0xE000 && data <= 0xFFFF) // -1
			{
				mapped_addr = (nPRGBanks - 1) * 0x2000 + (addr & 0x1FFF);
			}
		}
		else
		{
			if (data >= 0x8000 && data <= 0x9FFF) // R6
			{
				uint8_t R6 = (nBankData & 0b11111100) >> 2;
				mapped_addr = R6 * 0x2000 + (addr & 0x1FFF);
			}
			else if (data >= 0xA000 && data <= 0xBFFF) // R7
			{
				uint8_t R7 = (nBankData & 0b11111100) >> 2;
				mapped_addr = R7 * 0x2000 + (addr & 0x1FFF);
			}
			else if (data >= 0xC000 && data <= 0xDFFF) // -2
			{
				mapped_addr = (nPRGBanks - 2) * 0x2000 + (addr & 0x1FFF);
			}
			else if (data >= 0xE000 && data <= 0xFFFF) // -1
			{
				mapped_addr = (nPRGBanks - 1) * 0x2000 + (addr & 0x1FFF);
			}
		}
		return true;
	}
	return false;
}

bool Mapper_004::cpuMapWrite(uint16_t addr, uint32_t &mapped_addr, uint8_t data)
{
	if (addr >= 0x6000 && addr <= 0x7FFF)
	{
		// Write is to static ram on cartridge
		mapped_addr = 0xFFFFFFFF;

		// Write data to RAM
		vRAMStatic[addr & 0x1FFF] = data;

		// Signal mapper has handled request
		return true;
	}

	if (addr >= 0x8000 && addr <= 0x9FFF)
	{
		if (addr % 2 == 0) // nBankSelectRegister
		{
			nBankSelectRegister = data;
		}
		else // nBankData
		{
			// Update target register
			pRegister[nBankSelectRegister & 0x07 ] = data;

			// Update Pointer Table
			if (nBankSelectRegister & 0x80)
			{
				pCHRBank[0] = pRegister[2] * 0x0400;
				pCHRBank[1] = pRegister[3] * 0x0400;
				pCHRBank[2] = pRegister[4] * 0x0400;
				pCHRBank[3] = pRegister[5] * 0x0400;
				pCHRBank[4] = (pRegister[0] & 0xFE) * 0x0400;
				pCHRBank[5] = pRegister[0] * 0x0400 + 0x0400;
				pCHRBank[6] = (pRegister[1] & 0xFE) * 0x0400;
				pCHRBank[7] = pRegister[1] * 0x0400 + 0x0400;
			}
			else
			{
				pCHRBank[0] = (pRegister[0] & 0xFE) * 0x0400;
				pCHRBank[1] = pRegister[0] * 0x0400 + 0x0400;
				pCHRBank[2] = (pRegister[1] & 0xFE) * 0x0400;
				pCHRBank[3] = pRegister[1] * 0x0400 + 0x0400;
				pCHRBank[4] = pRegister[2] * 0x0400;
				pCHRBank[5] = pRegister[3] * 0x0400;
				pCHRBank[6] = pRegister[4] * 0x0400;
				pCHRBank[7] = pRegister[5] * 0x0400;
			}

			if (nBankSelectRegister & 0x40)
			{
				pPRGBank[2] = (pRegister[6] & 0x3F) * 0x2000;
				pPRGBank[0] = (nPRGBanks * 2 - 2) * 0x2000;
			}
			else
			{
				pPRGBank[0] = (pRegister[6] & 0x3F) * 0x2000;
				pPRGBank[2] = (nPRGBanks * 2 - 2) * 0x2000;
			}

			pPRGBank[1] = (pRegister[7] & 0x3F) * 0x2000;
			pPRGBank[3] = (nPRGBanks * 2 - 1) * 0x2000;
		}
		return true;
	}
	else if (addr >= 0xA000 && addr <= 0xBFFF)
	{
		if (addr % 2 == 0) // nImageRegister
		{
			nImage = data & 0b01;
		}
		else // nPrgRamProtect
		{
			nPrgRamProtect = data & 0b11110000;
		}
		return true;
	}
	else if (addr >= 0xC000 && addr <= 0xDFFF)
	{
		if (addr % 2 == 0) // nIrqLock
		{
			nIrqLock = data;
		}
		else // nIrqReload
		{
			nIrqReload = data;
		}
		return true;
	}
	else if (addr >= 0xE000 && addr <= 0xFFFF)
	{
		if (addr % 2 == 0) // nIrqBan
		{
			nIrqBan = data;
		}
		else // nIrqUse
		{
			nIrqUse = data;
		}
		return true;
	}

	return false;
}

bool Mapper_004::ppuMapRead(uint16_t addr, uint32_t &mapped_addr)
{
	if (addr < 0x2000)
	{
		if (nCHRBanks == 0)
		{
			mapped_addr = addr;
			return true;
		}
		else
		{
			if (nBankSelectRegister & 0b10000000)
			{
				if (addr >= 0x0000 && addr <= 0x03FF) // r2
				{
					uint8_t R2 = nBankData;
					mapped_addr = R2 * 0x0400 + (addr & 0x03FF);
					return true;
				}
				else if (addr >= 0x0400 && addr <= 0x07FF) //r3
				{
					uint8_t R3 = nBankData;
					mapped_addr = R3 * 0x0400 + (addr & 0x03FF);
					return true;
				}
				else if (addr >= 0x0800 && addr <= 0x0BFF) // r4
				{
					uint8_t R4 = nBankData;
					mapped_addr = R4 * 0x0400 + (addr & 0x03FF);
					return true;
				}
				else if (addr >= 0x0C00 && addr <= 0x0FFF) //r5
				{
					uint8_t R5 = nBankData;
					mapped_addr = R5 * 0x0400 + (addr & 0x03FF);
					return true;
				}
				else if (addr >= 0x1000 && addr <= 0x17FF) //r0
				{
					uint8_t R0 = (nBankData & 0b01111110);
					// uint8_t R0 = (nBankData & 0b01111111);
					mapped_addr = R0 * 0x0400 + (addr & 0x07FF);
					return true;
				}
				else if (addr >= 0x1800 && addr <= 0x1FFF) //r1
				{
					uint8_t R1 = (nBankData & 0b01111110);
					// uint8_t R0 = (nBankData & 0b01111111);
					mapped_addr = R1 * 0x0400 + (addr & 0x07FF);
					return true;
				}
			}
			else
			{
				if (addr >= 0x0000 && addr <= 0x07FF) // r0
				{

					uint8_t R0 = (nBankData & 0b01111110);
					// uint8_t R0 = (nBankData & 0b01111111);
					mapped_addr = R0 * 0x0400 + (addr & 0x07FF);
					return true;
				}
				else if (addr >= 0x0800 && addr <= 0x0FFF) //r1
				{
					uint8_t R1 = (nBankData & 0b01111110);
					// uint8_t R0 = (nBankData & 0b01111111);
					mapped_addr = R1 * 0x0400 + (addr & 0x07FF);
					return true;
				}
				else if (addr >= 0x1000 && addr <= 0x13FF) // r2
				{
					uint8_t R2 = nBankData;
					mapped_addr = R2 * 0x0400 + (addr & 0x03FF);
					return true;
				}
				else if (addr >= 0x1400 && addr <= 0x17FF) //r3
				{
					uint8_t R3 = nBankData;
					mapped_addr = R3 * 0x0400 + (addr & 0x03FF);
					return true;
				}
				else if (addr >= 0x1800 && addr <= 0x1BFF) //r4
				{
					uint8_t R4 = nBankData;
					mapped_addr = R4 * 0x0400 + (addr & 0x03FF);
					return true;
				}
				else if (addr >= 0x1C00 && addr <= 0x1FFF) //r5
				{
					uint8_t R5 = nBankData;
					mapped_addr = R5 * 0x0400 + (addr & 0x03FF);
					return true;
				}
			}
		}
	}

	return false;
}

bool Mapper_004::ppuMapWrite(uint16_t addr, uint32_t &mapped_addr)
{
	if (addr < 0x2000)
	{
		if (nCHRBanks == 0)
		{
			mapped_addr = addr;
			return true;
		}

		return true;
	}
	else
		return false;
}

void Mapper_004::reset()
{
	nBankSelectRegister = 0x00;
	nBankData = 0x00;
	nImage = 0x00;
	nPrgRamProtect = 0x00;
	nIrqLock = 0x00;
	nIrqReload = 0x00;
	nIrqBan = 0x00;
	nIrqUse = 0x00;
}

MIRROR Mapper_004::mirror()
{
	return mirrormode;
}

bool Mapper_004::irqState()
{
}
void Mapper_004::irqClear()
{
}

// Scanline Counting
void Mapper_004::scanline()
{
}