// Copyright [2021] <tiansongyu>

#include "Mapper_003.h"

Mapper_003::Mapper_003(uint8_t prgBanks, uint8_t chrBanks)
    : Mapper(prgBanks, chrBanks) {
  reset();
}

Mapper_003::~Mapper_003() {}

void Mapper_003::reset() {
  regHi = 0x00;
  regLo = 0x00;
}

bool Mapper_003::cpuMapRead(uint16_t addr, uint32_t &mapped_addr,
                            uint8_t &data) {
  // 存在ROM为16KB和32KB两种情况
  if (addr >= 0x8000 && addr <= 0xFFFF) {
    if (nPRGBanks == 1)  // 16KB
      mapped_addr = addr & 0x3FFF;
    else if (nPRGBanks == 2)  // 32KB
      mapped_addr = addr & 0x7FFF;
    return true;
  }

  return false;
}

bool Mapper_003::cpuMapWrite(uint16_t addr, uint32_t &mapped_addr,
                             uint8_t data) {
  // 修改寄存器regLo的值
  if (addr >= 0x8000 && addr <= 0xFFFF) {
    regLo = data & 0b00000011;
  }
  return false;
}

bool Mapper_003::ppuMapRead(uint16_t addr, uint32_t &mapped_addr) {
  // There is no mapping required for PPU
  // PPU Address Bus          CHR ROM
  // 0x0000 -> 0x1FFF: Map    0x0000 -> 0x1FFF

  // mapper_3
  // PPU $0000-$1FFF: 8 KB switchable CHR ROM bank
  if (addr >= 0x0000 && addr <= 0x1FFF) {
    mapped_addr = regLo * 0x2000 + addr;
    return true;
  }

  return false;
}

bool Mapper_003::ppuMapWrite(uint16_t addr, uint32_t &mapped_addr) {
  return false;
}
