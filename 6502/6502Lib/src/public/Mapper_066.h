#pragma once
#include "Mapper.h"

class Mapper_066 : public Mapper {
 public:
  Mapper_066(uint8_t prgBanks, uint8_t chrBanks);
  ~Mapper_066();

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
