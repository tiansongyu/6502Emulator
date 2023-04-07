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
#ifdef __GNUC__
// 关闭 警告：由于数据类型范围限制，比较结果永远为真
// 关闭 警告：unused parameter
#pragma GCC diagnostic ignored "-Wtype-limits"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include "Mapper_000.h"

Mapper_000::Mapper_000(uint8_t prgBanks, uint8_t chrBanks)
    : Mapper(prgBanks, chrBanks) {
  reset();
}

Mapper_000::~Mapper_000() {}

void Mapper_000::reset() {}

bool Mapper_000::cpuMapRead(uint16_t addr, uint32_t &mapped_addr,
                            uint8_t &data) {
  // if PRGROM is 16KB
  //     CPU Address Bus          PRG ROM
  //     0x8000 -> 0xBFFF: Map    0x0000 -> 0x3FFF
  //     0xC000 -> 0xFFFF: Mirror 0x0000 -> 0x3FFF
  // if PRGROM is 32KB
  //     CPU Address Bus          PRG ROM
  //     0x8000 -> 0xFFFF: Map    0x0000 -> 0x7FFF

  // PRGROM分为16KB和32KB两种情况
  // 如果16KB，则其实只有0x3FFF使用到，从0x4000 -> 0x7FFF等同于0x0000 -> 0x3FFF
  if (addr >= 0x8000 && addr <= 0xFFFF) {
    mapped_addr = addr & (nPRGBanks > 1 ? 0x7FFF : 0x3FFF);
    return true;
  }

  return false;
}

bool Mapper_000::cpuMapWrite(uint16_t addr, uint32_t &mapped_addr,
                             uint8_t data) {
  // data没有使用,map中没有寄存器，cpu不会向map写入数据
  if (addr >= 0x8000 && addr <= 0xFFFF) {
    mapped_addr = addr & (nPRGBanks > 1 ? 0x7FFF : 0x3FFF);
    return true;
  }

  return false;
}

bool Mapper_000::ppuMapRead(uint16_t addr, uint32_t &mapped_addr) {
  // There is no mapping required for PPU
  // PPU Address Bus          CHR ROM
  // 0x0000 -> 0x1FFF: Map    0x0000 -> 0x1FFF
  if (addr >= 0x0000 && addr <= 0x1FFF) {
    mapped_addr = addr;
    return true;
  }

  return false;
}

bool Mapper_000::ppuMapWrite(uint16_t addr, uint32_t &mapped_addr) {
  if (addr >= 0x0000 && addr <= 0x1FFF) {
    if (nCHRBanks == 0) {
      // Treat as RAM
      mapped_addr = addr;
      return true;
    }
  }

  return false;
}
