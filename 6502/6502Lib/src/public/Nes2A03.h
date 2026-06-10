/*
        olc::NES - APU
        "Thanks Dad for believing computers were gonna be a big deal..." -
   javidx9

        License (OLC-3)
        ~~~~~~~~~~~~~~~

        Copyright 2018-2019 OneLoneCoder.com

        Redistribution and use in source and binary forms, with or without
        modification, are permitted provided that the following conditions
        are met:

        1. Redistributions or derivations of source code must retain the above
        copyright notice, this list of conditions and the following disclaimer.

        2. Redistributions or derivative works in binary form must reproduce
        the above copyright notice. This list of conditions and the following
        disclaimer must be reproduced in the documentation and/or other
        materials provided with the distribution.

        3. Neither the name of the copyright holder nor the names of its
        contributors may be used to endorse or promote products derived
        from this software without specific prior written permission.

        THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
        "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
        LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
        A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
        HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
        SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
        LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
        DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
        THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
        (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
        OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

        Relevant Video: https://youtu.be/xdzOvpYPmGE

        Author
        ~~~~~~
        David Barr, aka javidx9, OneLoneCoder 2019
*/

// 实现状态：脉冲 1/2 与噪声通道可用；三角波与 DMC 尚未实现。
#pragma once

#include <cstdint>

class Nes2A03 {
 public:
  Nes2A03();
  ~Nes2A03();

 public:
  void cpuWrite(uint16_t addr, uint8_t data);
  uint8_t cpuRead(uint16_t addr);
  void clock();
  void reset();

  double GetOutputSample();

 private:
  uint32_t frame_clock_counter = 0;
  uint32_t clock_counter = 0;

 private:
  static const uint8_t length_table[];

 private:
  // Sequencer Module
  // ~~~~~~~~~~~~~~~~
  // 序列器以 reload 为周期倒数计时，归零时旋转内部模板并输出其
  // 最低位。脉冲通道的模板是占空比图样，噪声通道的模板是 LFSR。
  // 输出频率随 reload 增大而降低——这就是音高的来源。
  struct sequencer {
    uint32_t sequence = 0x00000000;
    uint32_t new_sequence = 0x00000000;
    uint16_t timer = 0x0000;
    uint16_t reload = 0x0000;
    uint8_t output = 0x00;

    // funcManip 由通道决定如何旋转/反馈模板（无捕获 lambda 即可转换）
    uint8_t clock(bool bEnable, void (*funcManip)(uint32_t &s)) {
      if (bEnable) {
        timer--;
        if (timer == 0xFFFF) {
          timer = reload;
          funcManip(sequence);
          output = sequence & 0x00000001;
        }
      }
      return output;
    }
  };

  // 长度计数器：到 0 时静音通道；halt 暂停倒数
  struct lengthcounter {
    uint8_t counter = 0x00;
    uint8_t clock(bool bEnable, bool bHalt) {
      if (!bEnable)
        counter = 0;
      else if (counter > 0 && !bHalt)
        counter--;
      return counter;
    }
  };

  // 包络发生器：disable 时输出恒定音量，否则按 divider 速度衰减，
  // loop 模式下衰减到 0 后重新从 15 开始
  struct envelope {
    void clock(bool bLoop) {
      if (!start) {
        if (divider_count == 0) {
          divider_count = volume;

          if (decay_count == 0) {
            if (bLoop) {
              decay_count = 15;
            }
          } else {
            decay_count--;
          }
        } else {
          divider_count--;
        }
      } else {
        start = false;
        decay_count = 15;
        divider_count = volume;
      }

      if (disable) {
        output = volume;
      } else {
        output = decay_count;
      }
    }

    bool start = false;
    bool disable = false;
    uint16_t divider_count = 0;
    uint16_t volume = 0;
    uint16_t output = 0;
    uint16_t decay_count = 0;
  };

  // 扫频单元：周期性地按 shift 改变脉冲通道的 timer 周期（滑音），
  // 目标周期越界时静音通道
  struct sweeper {
    bool enabled = false;
    bool down = false;
    bool reload = false;
    uint8_t shift = 0x00;
    uint8_t timer = 0x00;
    uint8_t period = 0x00;
    uint16_t change = 0;
    bool mute = false;

    void track(uint16_t target) {
      if (enabled) {
        change = target >> shift;
        mute = (target < 8) || (target > 0x7FF);
      }
    }

    bool clock(uint16_t &target, bool channel) {
      bool changed = false;
      if (timer == 0 && enabled && shift > 0 && !mute) {
        if (target >= 8 && change < 0x07FF) {
          if (down) {
            target -= change - channel;
          } else {
            target += change;
          }
          changed = true;
        }
      }

      if (timer == 0 || reload) {
        timer = period;
        reload = false;
      } else {
        timer--;
      }

      mute = (target < 8) || (target > 0x7FF);

      return changed;
    }
  };

  // 一个发声通道的全部状态。脉冲 1/2 与噪声共用同一结构，
  // 差异只在各自的序列器模板与门控规则里。
  // lp 是通道的一阶低通滤波状态（在 Bus::clock 降采样到 44.1kHz 之前
  // 完成抗混叠，截止频率取 NES 模拟输出级附近）。
  struct Channel {
    bool enable = false;
    bool halt = false;
    sequencer seq;
    envelope env;
    lengthcounter lc;
    double lp = 0.0;
  };

  Channel pulse[2];
  Channel noise;
  sweeper sweep[2];  // 扫频单元只属于两个脉冲通道，与 pulse[] 同下标

  // 混音后的一阶高通（DC blocker）：消除通道开关造成的直流台阶，
  // 它们正是扬声器里"砰"声的来源。
  double mixer_hp_in = 0.0;
  double mixer_hp_out = 0.0;

 public:
  // 调试可视化：当前各通道的周期值（2047 表示静音）
  uint16_t pulse1_visual = 0;
  uint16_t pulse2_visual = 0;
  uint16_t noise_visual = 0;
  uint16_t triangle_visual = 0;
};
