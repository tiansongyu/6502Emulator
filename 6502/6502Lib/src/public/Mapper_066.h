// Copyright [2020-2022] <tiansongyu>

#pragma once
#include "Mapper.h"

class Mapper_066 : public Mapper {
 public:
  Mapper_066(uint8_t prgBanks, uint8_t chrBanks);
  ~Mapper_066();

 public:
  bool cpuMapRead(uint16_t addr, uint32_t &mapped_addr) override;
  bool cpuMapWrite(uint16_t addr, uint32_t &mapped_addr,
                   uint8_t data = 0) override;
  bool ppuMapRead(uint16_t addr, uint32_t &mapped_addr) override;
  bool ppuMapWrite(uint16_t addr, uint32_t &mapped_addr) override;
  void reset() override;

  void SaveState(std::ostream &os) override;
  void LoadState(std::istream &is) override;

 private:
  uint8_t regHi = 0x00;
  uint8_t regLo = 0x00;
  // No local equipment required

  // 存档字段的唯一清单：SaveState/LoadState 共用
  template <typename F>
  void VisitState(F f) {
    f(regLo);
    f(regHi);
  }
};
