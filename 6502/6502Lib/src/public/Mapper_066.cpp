// Copyright [2020-2021] <tiansongyu>
#ifdef __GNUC__
// 关闭 警告：由于数据类型范围限制，比较结果永远为真
// 关闭 警告：unused parameter
#pragma GCC diagnostic ignored "-Wtype-limits"
#pragma GCC diagnostic ignored "-Wunused-parameter"

#endif
#include "Mapper_066.h"

Mapper_066::Mapper_066(uint8_t prgBanks, uint8_t chrBanks) : Mapper(prgBanks, chrBanks)
{
    reset();
}

Mapper_066::~Mapper_066()
{
}

void Mapper_066::reset()
{
    regHi = 0x00;
    regLo = 0x00;
}

bool Mapper_066::cpuMapRead(uint16_t addr, uint32_t &mapped_addr, uint8_t &data)
{
    // CPU $8000-$FFFF: 32 KB switchable PRG ROM bank
    // PPU $0000-$1FFF: 8 KB switchable CHR ROM bank

    if (addr >= 0x8000 && addr <= 0xFFFF)
    {
        mapped_addr = regHi * 0x8000 + (addr & 0x7FFF);
        return true;
    }

    return false;
}

bool Mapper_066::cpuMapWrite(uint16_t addr, uint32_t &mapped_addr, uint8_t data)
{
    // 7  bit  0
    // ---- ----
    // xxPP xxCC
    // ||   ||
    // ||   ++- Select 8 KB CHR ROM bank for PPU $0000-$1FFF
    // ++------ Select 32 KB PRG ROM bank for CPU $8000-$FFFF
    if (addr >= 0x8000 && addr <= 0xFFFF)
    {
        regLo = data & 0b00000011;
        regHi = (data & 0b00110000) >> 4;
        return true;
    }

    return false;
}

bool Mapper_066::ppuMapRead(uint16_t addr, uint32_t &mapped_addr)
{
    // There is no mapping required for PPU
    // PPU Address Bus          CHR ROM
    // 0x0000 -> 0x1FFF: Map    0x0000 -> 0x1FFF
    if (addr >= 0x0000 && addr <= 0x1FFF)
    {
        mapped_addr = regLo * 0x2000 + addr;
        return true;
    }

    return false;
}

bool Mapper_066::ppuMapWrite(uint16_t addr, uint32_t &mapped_addr)
{
    return false;
}
