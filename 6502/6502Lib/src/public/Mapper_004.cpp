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
	if (addr >= 0x8000 && addr <= 0x9FFF)
	{
		mapped_addr = pPRGBank[0] + (addr & 0x1FFF);
		return true;
	}

	if (addr >= 0xA000 && addr <= 0xBFFF)
	{
		mapped_addr = pPRGBank[1] + (addr & 0x1FFF);
		return true;
	}

	if (addr >= 0xC000 && addr <= 0xDFFF)
	{
		mapped_addr = pPRGBank[2] + (addr & 0x1FFF);
		return true;
	}

	if (addr >= 0xE000 && addr <= 0xFFFF)
	{
		mapped_addr = pPRGBank[3] + (addr & 0x1FFF);
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
		// Bank Select
		// 判断为偶数时
		if (!(addr & 0x0001))
		{
			// nTargetRegister = data & 0x07;
			// bPRGBankMode = (data & 0x40);
			// bCHRInversion = (data & 0x80);
			nBankSelectRegister = data;
		}
		else
		{
			// Update target register
			pRegister[nBankSelectRegister & 0x07] = data;

			// Update Pointer Table
			// R0 and R1 ignore the bottom bit,
			// as the value written still counts banks in 1KB units but odd numbered banks can't be selected.

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
			// R6 and R7 will ignore the top two bits, as the MMC3 has only 6 PRG ROM address lines.
			// Some romhacks rely on an 8-bit extension of R6/7 for oversized PRG-ROM,
			// but this is deliberately not supported by many emulators. See iNES Mapper 004 below.
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

		return false;
	}

	if (addr >= 0xA000 && addr <= 0xBFFF)
	{
		if (!(addr & 0x0001))
		{
			// Mirroring
			// 这个寄存器有什么TM的用？？？？？？？
			// 这不是直接赋值了吗，有个屁用
			if (data & 0x01)
				mirrormode = MIRROR::HORIZONTAL;
			else
				mirrormode = MIRROR::VERTICAL;
		}
		else
		{
			// PRG Ram Protect
			// TODO:
		}
		return false;
	}

	if (addr >= 0xC000 && addr <= 0xDFFF)
	{
		if (!(addr & 0x0001))
		{
			nIrqLatch = data;
		}
		else
		{
			nIrqCounter = 0x0000;
		}
		return false;
	}

	if (addr >= 0xE000 && addr <= 0xFFFF)
	{
		if (!(addr & 0x0001))
		{
			bIRQEnable = false;
			bIRQActive = false;
		}
		else
		{
			bIRQEnable = true;
		}
		return false;
	}

	return false;
}

bool Mapper_004::ppuMapRead(uint16_t addr, uint32_t &mapped_addr)
{
	if (addr >= 0x0000 && addr <= 0x03FF)
	{
		mapped_addr = pCHRBank[0] + (addr & 0x03FF);
		return true;
	}

	if (addr >= 0x0400 && addr <= 0x07FF)
	{
		mapped_addr = pCHRBank[1] + (addr & 0x03FF);
		return true;
	}

	if (addr >= 0x0800 && addr <= 0x0BFF)
	{
		mapped_addr = pCHRBank[2] + (addr & 0x03FF);
		return true;
	}

	if (addr >= 0x0C00 && addr <= 0x0FFF)
	{
		mapped_addr = pCHRBank[3] + (addr & 0x03FF);
		return true;
	}

	if (addr >= 0x1000 && addr <= 0x13FF)
	{
		mapped_addr = pCHRBank[4] + (addr & 0x03FF);
		return true;
	}

	if (addr >= 0x1400 && addr <= 0x17FF)
	{
		mapped_addr = pCHRBank[5] + (addr & 0x03FF);
		return true;
	}

	if (addr >= 0x1800 && addr <= 0x1BFF)
	{
		mapped_addr = pCHRBank[6] + (addr & 0x03FF);
		return true;
	}

	if (addr >= 0x1C00 && addr <= 0x1FFF)
	{
		mapped_addr = pCHRBank[7] + (addr & 0x03FF);
		return true;
	}

	return false;
}

bool Mapper_004::ppuMapWrite(uint16_t addr, uint32_t &mapped_addr)
{
	return false;
}

void Mapper_004::reset()
{
	nBankSelectRegister = 0x00;
	nBankData = 0x00;
	nPrgRamProtect = 0x00;
	nIrqCounter = 0x00;
	nIrqLatch = 0x00;

	mirrormode = MIRROR::HORIZONTAL;

	for (int i = 0; i < 4; i++)
		pPRGBank[i] = 0;
	for (int i = 0; i < 8; i++)
	{
		pCHRBank[i] = 0;
		pRegister[i] = 0;
	}

	pPRGBank[0] = 0 * 0x2000;
	pPRGBank[1] = 1 * 0x2000;
	pPRGBank[2] = (nPRGBanks * 2 - 2) * 0x2000;
	pPRGBank[3] = (nPRGBanks * 2 - 1) * 0x2000;
}

MIRROR Mapper_004::mirror()
{
	return mirrormode;
}

bool Mapper_004::irqState()
{
	return bIRQActive;
}

void Mapper_004::irqClear()
{
	bIRQActive = false;
}

void Mapper_004::scanline()
{
	if (nIrqCounter == 0)
	{
		nIrqCounter = nIrqLatch;
	}
	else
		nIrqCounter--;

	if (nIrqCounter == 0 && bIRQEnable)
	{
		bIRQActive = true;
	}
}