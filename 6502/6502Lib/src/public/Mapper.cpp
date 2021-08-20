#include "Mapper.h"

Mapper::Mapper(uint8_t prgBanks, uint8_t chrBanks)
{
	nPRGBanks = prgBanks;
	nCHRBanks = chrBanks;

	//reset();
}

Mapper::~Mapper()
{
}

