// Copyright [2020-2022] <tiansongyu>

#pragma once
#include <cstdint>
#include <iosfwd>
#include <vector>

enum MIRROR {
  HARDWARE,
  HORIZONTAL,
  VERTICAL,
  ONESCREEN_LO,
  ONESCREEN_HI,
};

class Mapper {
 public:
  Mapper(uint8_t prgBanks, uint8_t chrBanks);
  virtual ~Mapper();

 public:
  // 卡带上的 PRG RAM（$6000-$7FFF 窗口）。没有 RAM 的 mapper 保持
  // 为空，Cartridge 据此决定是否响应该地址段——数据本身就是
  // "有没有 RAM" 的答案，不再需要带内哨兵地址协议。
  std::vector<uint8_t> prgRam;

  // Transform CPU bus address into PRG ROM offset
  virtual bool cpuMapRead(uint16_t addr, uint32_t &mapped_addr) = 0;
  virtual bool cpuMapWrite(uint16_t addr, uint32_t &mapped_addr,
                           uint8_t data = 0) = 0;
  // Transform PPU bus address into CHR ROM offset
  virtual bool ppuMapRead(uint16_t addr, uint32_t &mapped_addr) = 0;
  virtual bool ppuMapWrite(uint16_t addr, uint32_t &mapped_addr) = 0;

  virtual void reset() = 0;

  // Get Mirror mode if mapper is in control
  virtual MIRROR mirror();

  // IRQ 线电平。确认/释放由具体 mapper 的寄存器写入完成
  //（如 MMC3 写 $E000），总线只看电平——不存在"替游戏清中断"。
  virtual bool irqState();

  // Scanline Counting
  virtual void scanline();

  // 存档：基类写 PRG RAM，各 mapper 追加自己的 bank/IRQ 寄存器
  virtual void SaveState(std::ostream &os);
  virtual void LoadState(std::istream &is);

 protected:
  // These are stored locally as many of the mappers require this information
  uint8_t nPRGBanks = 0;
  uint8_t nCHRBanks = 0;
};
