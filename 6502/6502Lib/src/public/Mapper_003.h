#pragma once
#include "Mapper.h"


class Mapper_003 : public Mapper
{
public:
	Mapper_003(uint8_t prgBanks, uint8_t chrBanks);
	~Mapper_003();

public:
	bool cpuMapRead(uint16_t addr, uint32_t &mapped_addr) override;
	bool cpuMapWrite(uint16_t addr, uint32_t &mapped_addr, uint8_t data = 0) override;
	bool ppuMapRead(uint16_t addr, uint32_t &mapped_addr) override;
	bool ppuMapWrite(uint16_t addr, uint32_t &mapped_addr) override;
	void reset() override;
private:
	uint8_t regHi = 0x00;
	uint8_t regLo = 0x00;
	// No local equipment required
};

