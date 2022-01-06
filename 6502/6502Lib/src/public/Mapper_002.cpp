// Copyright [2021] <tiansongyu>

#include "Mapper_002.h"

Mapper_002::Mapper_002(uint8_t prgBanks, uint8_t chrBanks)
    : Mapper(prgBanks, chrBanks) {
  reset();
}

Mapper_002::~Mapper_002() {}

void Mapper_002::reset() {
  regLo = 0x00;
  regHi = nPRGBanks - 1;
}

bool Mapper_002::cpuMapRead(uint16_t addr, uint32_t &mapped_addr,
                            uint8_t &data) {
  if (addr >= 0x8000 && addr <= 0xBFFF) {
    mapped_addr = regLo * 0x4000 + (addr & 0x3FFF);
    return true;
  } else if (addr >= 0xC000 && addr <= 0xFFFF) {
    mapped_addr = regHi * 0x4000 + (addr & 0x3FFF);
    return true;
  }

  return false;
}

bool Mapper_002::cpuMapWrite(uint16_t addr, uint32_t &mapped_addr,
                             uint8_t data) {
  if (addr >= 0x8000 && addr <= 0xFFFF) {
    regLo = data & 0x0F;
  }
  return false;
}

bool Mapper_002::ppuMapRead(uint16_t addr, uint32_t &mapped_addr) {
  // There is no mapping required for PPU
  // PPU Address Bus          CHR ROM
  // 0x0000 -> 0x1FFF: Map    0x0000 -> 0x1FFF
  if (addr >= 0x0000 && addr <= 0x1FFF) {
    mapped_addr = addr;
    return true;
  }

  return false;
}

bool Mapper_002::ppuMapWrite(uint16_t addr, uint32_t &mapped_addr) {
  if (addr >= 0x0000 && addr <= 0x1FFF) {
    if (nCHRBanks == 0) {
      // Treat as RAM
      mapped_addr = addr;
      return true;
    }
  }

  return false;
}
