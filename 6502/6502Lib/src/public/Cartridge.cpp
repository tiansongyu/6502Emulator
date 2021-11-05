#include "Cartridge.h"
#include <iostream>
Cartridge::Cartridge(const std::string &sFileName)
{
	// iNES 格式文件头
	// nes文件格式wiki:https://wiki.nesdev.com/w/index.php?title=INES#Name_of_file_format
	// iNES0.7 iNES NES2.0
	struct sHeader
	{
		char name[4];
		uint8_t prg_rom_chunks;
		uint8_t chr_rom_chunks;
		uint8_t mapper1;
		uint8_t mapper2;
		uint8_t prg_ram_size;
		uint8_t tv_system1;
		uint8_t tv_system2;
		char unused[5];
	} header;

	bImageValid = false;

	std::ifstream ifs;
	ifs.open(sFileName, std::ifstream::binary);
	if (ifs.is_open())
	{
		// 读iNES文件头
		ifs.read((char *)&header, sizeof(sHeader));

		// 如果有mapper1，则调到512字节开始读取文件内容
		if (header.mapper1 & 0x04)
			ifs.seekg(512, std::ios_base::cur);

		// 读取这个iNES文件所使用的mapperid号
		nMapperID = ((header.mapper2 >> 4) << 4) | (header.mapper1 >> 4);
		mirror = (header.mapper1 & 0x01) ? VERTICAL : HORIZONTAL;

		// 这里默认是iNES文件格式，后续再添加iNES0.7 iNES2.0 // TODO
		uint8_t nFileType = 1;

		if (nFileType == 0)
		{
			// TODO iNES0.7
		}

		if (nFileType == 1)
		{
			nPRGBanks = header.prg_rom_chunks;
			vPRGMemory.resize(nPRGBanks * 16384);
			ifs.read((char *)vPRGMemory.data(), vPRGMemory.size());

			nCHRBanks = header.chr_rom_chunks;
			if (nCHRBanks == 0)
			{
				// 根据nCHRBanks个数，分配character rom内存
				vCHRMemory.resize(8192);
			}
			else
			{
				// 分配character rom内存
				vCHRMemory.resize(nCHRBanks * 8192);
			}
			ifs.read((char *)vCHRMemory.data(), vCHRMemory.size());
		}

		if (nFileType == 2)
		{
			// TODO iNES2.0
		}

		// 选择对应的映射器
		switch (nMapperID)
		{
		case 0:
			pMapper = std::make_shared<Mapper_000>(nPRGBanks, nCHRBanks);
			break;
			case   2: pMapper = std::make_shared<Mapper_002>(nPRGBanks, nCHRBanks); break;
			//case   3: pMapper = std::make_shared<Mapper_003>(nPRGBanks, nCHRBanks); break;
			//case  66: pMapper = std::make_shared<Mapper_066>(nPRGBanks, nCHRBanks); break;
			default:
				std::cout << "can not find the correct mapper" << std::endl;
				break;
			}

		bImageValid = true;
		ifs.close();
	}
}

Cartridge::~Cartridge()
{
}

bool Cartridge::ImageValid()
{
	return bImageValid;
}

bool Cartridge::cpuRead(uint16_t addr, uint8_t &data)
{
	// cpu读取卡带中的内容
	uint32_t mapped_addr = 0;
	if (pMapper->cpuMapRead(addr, mapped_addr))
	{
		data = vPRGMemory[mapped_addr];
		return true;
	}
	else
		return false;
}

bool Cartridge::cpuWrite(uint16_t addr, uint8_t data)
{
	uint32_t mapped_addr = 0;
	if (pMapper->cpuMapWrite(addr, mapped_addr, data))
	{
		vPRGMemory[mapped_addr] = data;
		return true;
	}
	else
		return false;
}

bool Cartridge::ppuRead(uint16_t addr, uint8_t &data)
{
	uint32_t mapped_addr = 0;
	if (pMapper->ppuMapRead(addr, mapped_addr))
	{
		data = vCHRMemory[mapped_addr];
		return true;
	}
	else
		return false;
}

bool Cartridge::ppuWrite(uint16_t addr, uint8_t data)
{
	uint32_t mapped_addr = 0;
	if (pMapper->ppuMapWrite(addr, mapped_addr))
	{
		vCHRMemory[mapped_addr] = data;
		return true;
	}
	else
		return false;
}

void Cartridge::reset()
{
	// 卡带重置，会重置mapper
	if (pMapper != nullptr)
		pMapper->reset();
}