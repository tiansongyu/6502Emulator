#pragma once
#include <cstdint>
#include <string>
#include <fstream>
#include <vector>
#include <memory>

#include "Mapper_000.h"
//待添加不同种类的mapper
#include "Mapper_002.h"
#include "Mapper_003.h"
#include "Mapper_066.h"

class Cartridge
{
public:	
	Cartridge(const std::string& sFileName);
	~Cartridge();


public:
	bool ImageValid();

	enum MIRROR
	{
		HORIZONTAL,
		VERTICAL,
		ONESCREEN_LO,
		ONESCREEN_HI,
	} mirror = HORIZONTAL;

private:
	bool bImageValid = false;

	uint8_t nMapperID = 0;
	uint8_t nPRGBanks = 0;
	uint8_t nCHRBanks = 0;

	std::vector<uint8_t> vPRGMemory;
	std::vector<uint8_t> vCHRMemory;

	std::shared_ptr<Mapper> pMapper;

public:
	// 与总线通信
	bool cpuRead(uint16_t addr, uint8_t &data);
	bool cpuWrite(uint16_t addr, uint8_t data);

	// 与PPU通信总线
	bool ppuRead(uint16_t addr, uint8_t &data);
	bool ppuWrite(uint16_t addr, uint8_t data);

	// 重置卡带
	void reset();
};

