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
#ifndef _6502_6502LIB_SRC_PUBLIC_CARTRIDGE_H_
#define _6502_6502LIB_SRC_PUBLIC_CARTRIDGE_H_

#include <cstdint>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

#include "Mapper_000.h"
// 待添加不同种类的mapper
#include "Mapper_001.h"
#include "Mapper_002.h"
#include "Mapper_003.h"
#include "Mapper_004.h"
#include "Mapper_066.h"

class Cartridge {
 public:
  explicit Cartridge(const std::string &sFileName);
  ~Cartridge();

 public:
  bool ImageValid();
  std::shared_ptr<Mapper> GetMapper();

 private:
  bool bImageValid = false;
  MIRROR hw_mirror = HORIZONTAL;

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

  MIRROR Mirror();
};
#endif  // _6502_6502LIB_SRC_PUBLIC_CARTRIDGE_H_
