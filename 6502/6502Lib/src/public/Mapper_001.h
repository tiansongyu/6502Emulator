#pragma once
#include "Mapper.h"


class Mapper_001 : public Mapper
{
public:
	Mapper_001(uint8_t prgBanks, uint8_t chrBanks);
	~Mapper_001();

public:
	bool cpuMapRead(uint16_t addr, uint32_t &mapped_addr) override;
	bool cpuMapWrite(uint16_t addr, uint32_t &mapped_addr, uint8_t data = 0) override;
	bool ppuMapRead(uint16_t addr, uint32_t &mapped_addr) override;
	bool ppuMapWrite(uint16_t addr, uint32_t &mapped_addr) override;
	void reset() override;
private :
	uint8_t regLoad = 0x00;
	uint8_t regControl = 0x00;
	uint8_t regCHRbank_0 = 0x00;
	uint8_t regCHRbank_1 = 0x00;
	uint8_t regPRGbank = 0x00;

	// No local equipment required
};

