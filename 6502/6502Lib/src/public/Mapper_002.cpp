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
#include "Mapper_002.h"

Mapper_002::Mapper_002(uint8_t prgBanks, uint8_t chrBanks) : Mapper(prgBanks, chrBanks)
{
    reset();
}

Mapper_002::~Mapper_002()
{
}

void Mapper_002::reset()
{
    regLo = 0x00;
    regHi = nPRGBanks - 1;
}

bool Mapper_002::cpuMapRead(uint16_t addr, uint32_t &mapped_addr, uint8_t &data)
{
    if (addr >= 0x8000 && addr <= 0xBFFF)
    {
        mapped_addr = regLo * 0x4000 + (addr & 0x3FFF);
        return true;
    }
    else if (addr >= 0xC000 && addr <= 0xFFFF)
    {
        mapped_addr = regHi * 0x4000 + (addr & 0x3FFF);
        return true;
    }

    return false;
}

bool Mapper_002::cpuMapWrite(uint16_t addr, uint32_t &mapped_addr, uint8_t data)
{
    if (addr >= 0x8000 && addr <= 0xFFFF)
    {
        regLo = data & 0x0F;
    }
    return false;
}

bool Mapper_002::ppuMapRead(uint16_t addr, uint32_t &mapped_addr)
{
    // There is no mapping required for PPU
    // PPU Address Bus          CHR ROM
    // 0x0000 -> 0x1FFF: Map    0x0000 -> 0x1FFF
    if (addr >= 0x0000 && addr <= 0x1FFF)
    {
        mapped_addr = addr;
        return true;
    }

    return false;
}

bool Mapper_002::ppuMapWrite(uint16_t addr, uint32_t &mapped_addr)
{
    if (addr >= 0x0000 && addr <= 0x1FFF)
    {
        if (nCHRBanks == 0)
        {
            // Treat as RAM
            mapped_addr = addr;
            return true;
        }
    }

    return false;
}
