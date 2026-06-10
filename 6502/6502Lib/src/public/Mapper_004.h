// Copyright [2020-2022] <tiansongyu>

#pragma once
#include "Mapper.h"

class Mapper_004 : public Mapper {
 public:
  Mapper_004(uint8_t prgBanks, uint8_t chrBanks);
  ~Mapper_004();

 public:
  bool cpuMapRead(uint16_t addr, uint32_t &mapped_addr) override;
  bool cpuMapWrite(uint16_t addr, uint32_t &mapped_addr,
                   uint8_t data = 0) override;
  bool ppuMapRead(uint16_t addr, uint32_t &mapped_addr) override;
  bool ppuMapWrite(uint16_t addr, uint32_t &mapped_addr) override;
  void reset() override;

  void SaveState(std::ostream &os) override;
  void LoadState(std::istream &is) override;

  MIRROR mirror();

  // IRQ Interface
  bool irqState();

  // Scanline Counting
  void scanline();

 private:
  uint8_t nBankSelectRegister = 0x00;
  uint8_t nBankData = 0x00;
  uint8_t nPrgRamProtect = 0x00;
  uint8_t nIrqCounter = 0x00;
  // nIrqLatch指的是发生中断时，刷新界面的行数
  uint8_t nIrqLatch = 0x00;
  uint32_t pRegister[8];
  uint32_t pCHRBank[8];
  uint32_t pPRGBank[4];

  bool bIRQEnable = false;
  bool bIRQActive = false;

  // No local equipment required

  MIRROR mirrormode = MIRROR::HORIZONTAL;

  // 存档字段的唯一清单：SaveState/LoadState 共用
  template <typename F>
  void VisitState(F f) {
    f(nBankSelectRegister);
    f(nBankData);
    f(nPrgRamProtect);
    f(nIrqCounter);
    f(nIrqLatch);
    f(pRegister);
    f(pCHRBank);
    f(pPRGBank);
    f(bIRQEnable);
    f(bIRQActive);
    f(mirrormode);
  }
};
