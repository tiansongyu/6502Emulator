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
	uint8_t nPrgRamProtect = 0x00;
	uint8_t nIrqCounter = 0x00;
	// nIrqLatch指的是发生中断时，刷新界面的行数
	uint8_t nIrqLatch = 0x00;  
	uint32_t pRegister[8];
	uint32_t pCHRBank[8];
	uint32_t pPRGBank[4];


	bool bIRQEnable = false;
	bool bIRQActive = false;

	// No local equipment required

	MIRROR mirrormode = MIRROR::HORIZONTAL;

	std::vector<uint8_t> vRAMStatic;
};
