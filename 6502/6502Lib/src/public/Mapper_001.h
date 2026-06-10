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

class Mapper_001 : public Mapper {
 public:
  Mapper_001(uint8_t prgBanks, uint8_t chrBanks);
  ~Mapper_001();

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

 private:
  uint8_t nCHRBankSelect4Lo = 0x00;
  uint8_t nCHRBankSelect4Hi = 0x00;
  uint8_t nCHRBankSelect8 = 0x00;

  uint8_t nPRGBankSelect16Lo = 0x00;
  uint8_t nPRGBankSelect16Hi = 0x00;
  uint8_t nPRGBankSelect32 = 0x00;

  uint8_t nLoadRegister = 0x00;
  uint8_t nLoadRegisterCount = 0x00;
  uint8_t nControlRegister = 0x00;

  // No local equipment required

  MIRROR mirrormode = MIRROR::HORIZONTAL;

  // 存档字段的唯一清单：SaveState/LoadState 共用
  template <typename F>
  void VisitState(F f) {
    f(nCHRBankSelect4Lo);
    f(nCHRBankSelect4Hi);
    f(nCHRBankSelect8);
    f(nPRGBankSelect16Lo);
    f(nPRGBankSelect16Hi);
    f(nPRGBankSelect32);
    f(nLoadRegister);
    f(nLoadRegisterCount);
    f(nControlRegister);
    f(mirrormode);
  }
};
