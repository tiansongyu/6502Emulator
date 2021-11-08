#pragma once
#include "Mapper.h"
#include <vector>

class Mapper_004 : public Mapper
{
public:
	Mapper_004(uint8_t prgBanks, uint8_t chrBanks);
	~Mapper_004();

public:
	bool cpuMapRead(uint16_t addr, uint32_t &mapped_addr, uint8_t &data) override;
	bool cpuMapWrite(uint16_t addr, uint32_t &mapped_addr, uint8_t data = 0) override;
	bool ppuMapRead(uint16_t addr, uint32_t &mapped_addr) override;
	bool ppuMapWrite(uint16_t addr, uint32_t &mapped_addr) override;
	void reset() override;

	MIRROR mirror();

	// IRQ Interface
	bool irqState();
	void irqClear();

	// Scanline Counting
	void scanline();

private:
	uint8_t nBankSelectRegister = 0x00;
	uint8_t nBankData = 0x00;
	uint8_t nImage = 0x00;
	uint8_t nPrgRamProtect = 0x00;
	uint8_t nIrqLock = 0x00;
	uint8_t nIrqReload = 0x00;
	uint8_t nIrqBan = 0x00;
	uint8_t nIrqUse = 0x00;

	uint32_t pRegister[8];

	// No local equipment required

	MIRROR mirrormode = MIRROR::HORIZONTAL;

	std::vector<uint8_t> vRAMStatic;
};
