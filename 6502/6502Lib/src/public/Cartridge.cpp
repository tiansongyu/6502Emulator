// Copyright (C) 2020-2022 tiansongyu
//
// This file is part of 6502Emulator.
//
// 6502Emulator is free GameEngine: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// 6502Emulator is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with 6502Emulator.  If not, see <http://www.gnu.org/licenses/>.
//
// 6502Emulator is actively maintained and developed!

#include "Cartridge.h"

#include <fstream>
#include <iostream>

#include "Mapper_000.h"
#include "Mapper_001.h"
#include "Mapper_002.h"
#include "Mapper_003.h"
#include "Mapper_004.h"
#include "Mapper_066.h"
Cartridge::Cartridge(const std::string &sFileName) {
  // iNES 格式文件头
  // nes文件格式wiki:https://wiki.nesdev.com/w/index.php?title=INES#Name_of_file_format
  // iNES0.7 iNES NES2.0
  struct sHeader {
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
  if (ifs.is_open()) {
    // 读iNES文件头
    ifs.read(reinterpret_cast<char *>(&header), sizeof(sHeader));
    // 如果有mapper1，则调到512字节开始读取文件内容
    if (header.mapper1 & 0x04) ifs.seekg(512, std::ios_base::cur);

    // 读取这个iNES文件所使用的mapperid号
    nMapperID = ((header.mapper2 >> 4) << 4) | (header.mapper1 >> 4);
    hw_mirror = (header.mapper1 & 0x01) ? VERTICAL : HORIZONTAL;

    // 这里默认是iNES文件格式，后续再添加iNES0.7 iNES2.0 // TODO
    uint8_t nFileType = 1;
    if ((header.mapper2 & 0x0C) == 0x08) nFileType = 2;

    if (nFileType == 0) {
      // TODO(tiansongyu) iNES0.7
    }

    if (nFileType == 1) {
      nPRGBanks = header.prg_rom_chunks;
      vPRGMemory.resize(nPRGBanks * 16384);
      ifs.read(reinterpret_cast<char *>(vPRGMemory.data()), vPRGMemory.size());

      nCHRBanks = header.chr_rom_chunks;
      if (nCHRBanks == 0) {
        // 根据nCHRBanks个数，分配character rom内存
        vCHRMemory.resize(8192);
      } else {
        // 分配character rom内存
        vCHRMemory.resize(nCHRBanks * 8192);
      }
      ifs.read(reinterpret_cast<char *>(vCHRMemory.data()), vCHRMemory.size());
    }

    if (nFileType == 2) {
      nPRGBanks = ((header.prg_ram_size & 0x07) << 8) | header.prg_rom_chunks;
      vPRGMemory.resize(nPRGBanks * 16384);
      ifs.read(reinterpret_cast<char *>(vPRGMemory.data()), vPRGMemory.size());

      nCHRBanks = ((header.prg_ram_size & 0x38) << 8) | header.chr_rom_chunks;
      vCHRMemory.resize(nCHRBanks * 8192);
      ifs.read((char *)vCHRMemory.data(), vCHRMemory.size());
    }

    // 选择对应的映射器
    switch (nMapperID) {
      case 0:
        pMapper = std::make_shared<Mapper_000>(nPRGBanks, nCHRBanks);
        break;
      case 1:
        pMapper = std::make_shared<Mapper_001>(nPRGBanks, nCHRBanks);
        break;
      case 2:
        pMapper = std::make_shared<Mapper_002>(nPRGBanks, nCHRBanks);
        break;
      case 3:
        pMapper = std::make_shared<Mapper_003>(nPRGBanks, nCHRBanks);
        break;
      case 4:
        pMapper = std::make_shared<Mapper_004>(nPRGBanks, nCHRBanks);
        break;
      case 66:
        pMapper = std::make_shared<Mapper_066>(nPRGBanks, nCHRBanks);
        break;
      default:
        std::cout << "can not find the correct mapper" << std::endl;
        break;
    }
    printf("MapperID is %d \n", nMapperID);
    bImageValid = true;
    ifs.close();
  }
}

Cartridge::~Cartridge() {}

bool Cartridge::ImageValid() { return bImageValid; }
// $6000-$7FFF 是卡带 PRG RAM 窗口。带 RAM 的 mapper 把 prgRam 容器
// 设为非空——数据本身回答了"有没有 RAM"，不需要哨兵地址协议。
bool Cartridge::cpuRead(uint16_t addr, uint8_t &data) {
  if (!pMapper->prgRam.empty() && addr >= 0x6000 && addr <= 0x7FFF) {
    data = pMapper->prgRam[addr & 0x1FFF];
    return true;
  }

  uint32_t mapped_addr = 0;
  if (pMapper->cpuMapRead(addr, mapped_addr)) {
    data = vPRGMemory[mapped_addr];
    return true;
  }
  return false;
}

bool Cartridge::cpuWrite(uint16_t addr, uint8_t data) {
  if (!pMapper->prgRam.empty() && addr >= 0x6000 && addr <= 0x7FFF) {
    pMapper->prgRam[addr & 0x1FFF] = data;
    return true;
  }

  uint32_t mapped_addr = 0;
  if (pMapper->cpuMapWrite(addr, mapped_addr, data)) {
    vPRGMemory[mapped_addr] = data;
    return true;
  }
  return false;
}

bool Cartridge::ppuRead(uint16_t addr, uint8_t &data) {
  uint32_t mapped_addr = 0;
  if (pMapper->ppuMapRead(addr, mapped_addr)) {
    data = vCHRMemory[mapped_addr];
    return true;
  } else
    return false;
}

bool Cartridge::ppuWrite(uint16_t addr, uint8_t data) {
  uint32_t mapped_addr = 0;
  if (pMapper->ppuMapWrite(addr, mapped_addr)) {
    vCHRMemory[mapped_addr] = data;
    return true;
  } else
    return false;
}

void Cartridge::reset() {
  // 卡带重置，会重置mapper

  if (pMapper != nullptr) pMapper->reset();
}

MIRROR Cartridge::Mirror() {
  MIRROR m = pMapper->mirror();
  if (m == MIRROR::HARDWARE) {
    // Mirror configuration was defined
    // in hardware via soldering
    return hw_mirror;
  } else {
    // Mirror configuration can be
    // dynamically set via mapper
    return m;
  }
}

std::shared_ptr<Mapper> Cartridge::GetMapper() { return pMapper; }