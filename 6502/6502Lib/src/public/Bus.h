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

#include <cstdint>
#include <iosfwd>
#include <memory>

#include "Cartridge.h"
#include "Nes2A03.h"
#include "Nes2C02.h"
#include "Nes6502.h"

class Bus {
 public:
  Bus();
  ~Bus();

 public:  // 总线
  // 6502 CPU
  Nes6502 cpu;
  // 2C02 PPU图形处理器。。。显卡。。。
  Nes2C02 ppu;
  // 2A03 APU 声卡。。。
  Nes2A03 apu;
  std::shared_ptr<Cartridge> cart;
  // CPU中2KB的内存RAM。上电清零，保证模拟器开机行为确定
  //（真实硬件是随机值，但确定性对调试和回归测试更有价值）
  uint8_t cpuRam[2048] = {};
  // 控制器数据暂存位置
  uint8_t controller[2] = {};

 public:
  // 设置声音发声频率
  // sample_rate是声音采样率，这个采样率决定一个声音数据的发生周期
  void SetSampleFrequency(uint32_t sample_rate);
  double dAudioSample = 0.0;

 private:
  double dAudioTime = 0.0;
  double dAudioGlobalTime = 0.0;
  double dAudioTimePerNESClock = 0.0;
  double dAudioTimePerSystemSample = 0.0f;

 public:
  // 总线中负责cpu的读写
  void cpuWrite(uint16_t addr, uint8_t data);
  uint8_t cpuRead(uint16_t addr, bool bReadOnly = false);

 private:
  // PPU 时钟与 CPU 时钟是 3:1。用显式的相位计数器而不是对一个
  // 不断增长的计数器取模——后者在 uint32 回绕时会让相位跳变
  //（2^32 % 3 != 0），导致 CPU 被多打或漏打一拍。
  uint8_t cpu_phase = 0;   // 0,1,2 循环；0 时 CPU 走一拍
  bool odd_cycle = false;  // 全局奇偶相位（DMA 对齐用）
  // 控制器移位寄存器与选通线
  uint8_t controller_state[2] = {};
  bool controller_strobe = false;

 private:
  // 游戏精灵的传送使用DMA机制，精灵一共有64个，每个精灵大小为4个字节
  // DMA传送开始时，CPU停止clock，改为DMA传送或者读取一次数据，每个周期还是只能进行读或者写一次操作
  // dma_page和dma_addr组成一个16位的CPU地址，
  // 通过DMA机制，将CPU中的数据，一个字节一个字节的送到PPU中
  // 由于开始时需要在CPU的偶数周期进行第一次读，所以需要513或者514个周期
  uint8_t dma_page = 0x00;
  uint8_t dma_addr = 0x00;
  uint8_t dma_data = 0x00;

  // DMA传输需要精确传送。原则上需要
  // 512个周期来读取和写入256字节的OAM内存，一个
  // 先读后写。但是，CPU需要处于“偶数”状态
  // 时钟周期，因此可能需要一个虚拟的空闲周期
  bool dma_dummy = true;

  // DMA传送发生标志
  bool dma_transfer = false;

 public:  // 系统接口
  // 将卡带智能指针对象连接到内部总线
  void insertCartridge(const std::shared_ptr<Cartridge> &cartridge);
  // 重置系统
  void reset();
  // 系统clock
  bool clock();

  // 显式版本化存档：魔数 + 版本号 + 各芯片逐字段状态。
  // 旧实现把整个 Bus 对象按 sizeof 裸 dump（含 vector/指针，跨进程
  // 未定义行为，且完全丢失 mapper 的 bank 切换状态）。
  void SaveState(std::ostream &os);
  bool LoadState(std::istream &is);
};
