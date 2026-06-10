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
#pragma once

#include <cstdint>
#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

#include "Mapper.h"

class Cartridge {
 public:
  explicit Cartridge(const std::string &sFileName);
  ~Cartridge();

 public:
  bool ImageValid();
  uint8_t MapperId() const { return nMapperID; }
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

  // 存档：mapper 寄存器 + PRG RAM + CHR RAM（ROM 内容不会变，不入档；
  // 记录 mapper 号用于加载时核对存档与卡带是否匹配）
  void SaveState(std::ostream &os) const;
  void LoadState(std::istream &is);

  MIRROR Mirror();
};
