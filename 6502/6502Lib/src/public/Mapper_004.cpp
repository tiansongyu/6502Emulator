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
#include "Mapper_004.h"

#include "StateIO.h"

Mapper_004::Mapper_004(uint8_t prgBanks, uint8_t chrBanks)
    : Mapper(prgBanks, chrBanks) {
  // MMC3 卡带带 8KB PRG RAM（$6000-$7FFF），由 Cartridge 统一处理
  prgRam.resize(8 * 1024);
  reset();
}

Mapper_004::~Mapper_004() {}

bool Mapper_004::cpuMapRead(uint16_t addr, uint32_t &mapped_addr) {
  // $8000-$FFFF 均分为 4 个 8KB 槽位，地址位 14-13 直接就是槽位号
  if (addr >= 0x8000) {
    mapped_addr = pPRGBank[(addr >> 13) & 0x03] + (addr & 0x1FFF);
    return true;
  }
  return false;
}

bool Mapper_004::cpuMapWrite(uint16_t addr, uint32_t & /*mapped_addr*/,
                             uint8_t data) {
  if (addr >= 0x8000 && addr <= 0x9FFF) {
    // Bank Select
    // 判断为偶数时
    if (!(addr & 0x0001)) {
      // nTargetRegister = data & 0x07;
      // bPRGBankMode = (data & 0x40);
      // bCHRInversion = (data & 0x80);
      nBankSelectRegister = data;
    } else {
      // Update target register
      pRegister[nBankSelectRegister & 0x07] = data;

      // Update Pointer Table
      // R0 and R1 ignore the bottom bit,
      // as the value written still counts banks in 1KB units but odd
      // numbered banks can't be selected.

      if (nBankSelectRegister & 0x80) {
        pCHRBank[0] = pRegister[2] * 0x0400;
        pCHRBank[1] = pRegister[3] * 0x0400;
        pCHRBank[2] = pRegister[4] * 0x0400;
        pCHRBank[3] = pRegister[5] * 0x0400;
        pCHRBank[4] = (pRegister[0] & 0xFE) * 0x0400;
        pCHRBank[5] = pRegister[0] * 0x0400 + 0x0400;
        pCHRBank[6] = (pRegister[1] & 0xFE) * 0x0400;
        pCHRBank[7] = pRegister[1] * 0x0400 + 0x0400;
      } else {
        pCHRBank[0] = (pRegister[0] & 0xFE) * 0x0400;
        pCHRBank[1] = pRegister[0] * 0x0400 + 0x0400;
        pCHRBank[2] = (pRegister[1] & 0xFE) * 0x0400;
        pCHRBank[3] = pRegister[1] * 0x0400 + 0x0400;
        pCHRBank[4] = pRegister[2] * 0x0400;
        pCHRBank[5] = pRegister[3] * 0x0400;
        pCHRBank[6] = pRegister[4] * 0x0400;
        pCHRBank[7] = pRegister[5] * 0x0400;
      }
      // R6 and R7 will ignore the top two bits, as the MMC3 has only 6
      // PRG ROM address lines. Some romhacks rely on an 8-bit extension
      // of R6/7 for oversized PRG-ROM, but this is deliberately not
      // supported by many emulators. See iNES Mapper 004 below.
      if (nBankSelectRegister & 0x40) {
        pPRGBank[2] = (pRegister[6] & 0x3F) * 0x2000;
        pPRGBank[0] = (nPRGBanks * 2 - 2) * 0x2000;
      } else {
        pPRGBank[0] = (pRegister[6] & 0x3F) * 0x2000;
        pPRGBank[2] = (nPRGBanks * 2 - 2) * 0x2000;
      }

      pPRGBank[1] = (pRegister[7] & 0x3F) * 0x2000;
      pPRGBank[3] = (nPRGBanks * 2 - 1) * 0x2000;
    }

    return false;
  }

  if (addr >= 0xA000 && addr <= 0xBFFF) {
    if (!(addr & 0x0001)) {
      // Mirroring
      // 这个寄存器有什么TM的用？？？？？？？
      // 这不是直接赋值了吗，有个屁用
      if (data & 0x01)
        mirrormode = MIRROR::HORIZONTAL;
      else
        mirrormode = MIRROR::VERTICAL;
    } else {
      // PRG Ram Protect
      // TODO(tiansongyu)
    }
    return false;
  }

  if (addr >= 0xC000 && addr <= 0xDFFF) {
    if (!(addr & 0x0001)) {
      nIrqLatch = data;
    } else {
      nIrqCounter = 0x0000;
    }
    return false;
  }

  if (addr >= 0xE000) {
    if (!(addr & 0x0001)) {
      bIRQEnable = false;
      bIRQActive = false;
    } else {
      bIRQEnable = true;
    }
    return false;
  }

  return false;
}

bool Mapper_004::ppuMapRead(uint16_t addr, uint32_t &mapped_addr) {
  // $0000-$1FFF 均分为 8 个 1KB 槽位，地址位 12-10 直接就是槽位号
  if (addr <= 0x1FFF) {
    mapped_addr = pCHRBank[addr >> 10] + (addr & 0x03FF);
    return true;
  }
  return false;
}

bool Mapper_004::ppuMapWrite(uint16_t /*addr*/, uint32_t & /*mapped_addr*/) {
  return false;
}

void Mapper_004::reset() {
  nBankSelectRegister = 0x00;
  nBankData = 0x00;
  nPrgRamProtect = 0x00;
  nIrqCounter = 0x00;
  nIrqLatch = 0x00;
  bIRQEnable = false;
  bIRQActive = false;

  mirrormode = MIRROR::HORIZONTAL;

  for (int i = 0; i < 4; i++) pPRGBank[i] = 0;
  for (int i = 0; i < 8; i++) {
    pCHRBank[i] = 0;
    pRegister[i] = 0;
  }

  pPRGBank[0] = 0 * 0x2000;
  pPRGBank[1] = 1 * 0x2000;
  pPRGBank[2] = (nPRGBanks * 2 - 2) * 0x2000;
  pPRGBank[3] = (nPRGBanks * 2 - 1) * 0x2000;
}

MIRROR Mapper_004::mirror() { return mirrormode; }

bool Mapper_004::irqState() { return bIRQActive; }

void Mapper_004::irqClear() { bIRQActive = false; }

void Mapper_004::scanline() {
  if (nIrqCounter == 0) {
    nIrqCounter = nIrqLatch;
  } else
    nIrqCounter--;
  if (nIrqCounter == 0 && bIRQEnable) {
    bIRQActive = true;
  }
}

void Mapper_004::SaveState(std::ostream &os) const {
  Mapper::SaveState(os);
  PutPod(os, nBankSelectRegister);
  PutPod(os, nBankData);
  PutPod(os, nPrgRamProtect);
  PutPod(os, nIrqCounter);
  PutPod(os, nIrqLatch);
  PutPod(os, pRegister);
  PutPod(os, pCHRBank);
  PutPod(os, pPRGBank);
  PutPod(os, bIRQEnable);
  PutPod(os, bIRQActive);
  PutPod(os, mirrormode);
}

void Mapper_004::LoadState(std::istream &is) {
  Mapper::LoadState(is);
  GetPod(is, nBankSelectRegister);
  GetPod(is, nBankData);
  GetPod(is, nPrgRamProtect);
  GetPod(is, nIrqCounter);
  GetPod(is, nIrqLatch);
  GetPod(is, pRegister);
  GetPod(is, pCHRBank);
  GetPod(is, pPRGBank);
  GetPod(is, bIRQEnable);
  GetPod(is, bIRQActive);
  GetPod(is, mirrormode);
}
