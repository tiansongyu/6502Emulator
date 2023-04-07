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
#include "Mapper.h"

class Mapper_003 : public Mapper {
 public:
  Mapper_003(uint8_t prgBanks, uint8_t chrBanks);
  ~Mapper_003();

 public:
  bool cpuMapRead(uint16_t addr, uint32_t &mapped_addr, uint8_t &data) override;
  bool cpuMapWrite(uint16_t addr, uint32_t &mapped_addr,
                   uint8_t data = 0) override;
  bool ppuMapRead(uint16_t addr, uint32_t &mapped_addr) override;
  bool ppuMapWrite(uint16_t addr, uint32_t &mapped_addr) override;
  void reset() override;

 private:
  uint8_t regHi = 0x00;
  uint8_t regLo = 0x00;
  // No local equipment required
};
