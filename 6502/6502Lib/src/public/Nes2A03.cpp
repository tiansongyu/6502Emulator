// Copyright [2020-2022] <tiansongyu>

#include "Nes2A03.h"

#include "StateIO.h"

const uint8_t Nes2A03::length_table[] = {
    10, 254, 20, 2,  40, 4,  80, 6,  160, 8,  60, 10, 14, 12, 26, 14,
    12, 16,  24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30};

// $4000/$4004 的 duty 位 (bit 7-6) 选择的占空比模板：
// 12.5%、25%、50%、75%
static const uint32_t kDutySeq[4] = {
    0b01000000,
    0b01100000,
    0b01111000,
    0b10011111,
};

// $400E 低 4 位选择的噪声周期
static const uint16_t kNoisePeriod[16] = {
    0, 4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 1016, 2034, 4068,
};

// 脉冲通道：8 位占空比模板循环右移
static void RotatePulse(uint32_t &s) {
  s = ((s & 0x0001) << 7) | ((s & 0x00FE) >> 1);
}

// 噪声通道：15 位线性反馈移位寄存器
static void ShiftNoise(uint32_t &s) {
  s = (((s & 0x0001) ^ ((s & 0x0002) >> 1)) << 14) | ((s & 0x7FFF) >> 1);
}

Nes2A03::Nes2A03() { reset(); }

Nes2A03::~Nes2A03() {}

void Nes2A03::cpuWrite(uint16_t addr, uint8_t data) {
  // $4000-$4007：两个脉冲通道各 4 个寄存器，布局完全相同，
  // (addr >> 2) & 1 选通道，addr & 3 选寄存器。
  if (addr >= 0x4000 && addr <= 0x4007) {
    Channel &p = pulse[(addr >> 2) & 1];
    sweeper &sw = sweep[(addr >> 2) & 1];

    switch (addr & 0x0003) {
      case 0:  // duty / 包络
        p.seq.new_sequence = kDutySeq[(data & 0xC0) >> 6];
        p.seq.sequence = p.seq.new_sequence;
        p.halt = (data & 0x20);        // envelope loop / length counter halt
        p.env.volume = (data & 0x0F);  // 音量
        p.env.disable = (data & 0x10);  // constant volume
        break;

      case 1:  // 扫频
        sw.enabled = data & 0x80;
        sw.period = (data & 0x70) >> 4;  // divider 周期为 P+1 个半帧
        sw.down = data & 0x08;
        sw.shift = data & 0x07;
        sw.reload = true;
        break;

      case 2:  // 周期低 8 位
        p.seq.reload = (p.seq.reload & 0xFF00) | data;
        break;

      case 3:  // 周期高 3 位 + 长度计数器装载
        p.seq.reload =
            (uint16_t)((data & 0x07)) << 8 | (p.seq.reload & 0x00FF);
        p.seq.timer = p.seq.reload;
        p.seq.sequence = p.seq.new_sequence;
        p.lc.counter = length_table[(data & 0xF8) >> 3];
        p.env.start = true;
        break;
    }
    return;
  }

  switch (addr) {
    case 0x400C:  // 噪声：包络
      noise.env.volume = (data & 0x0F);
      noise.env.disable = (data & 0x10);
      noise.halt = (data & 0x20);
      break;

    case 0x400E:  // 噪声：周期
      noise.seq.reload = kNoisePeriod[data & 0x0F];
      break;

    case 0x400F:  // 噪声：长度计数器装载 + 重启噪声包络
      noise.env.start = true;
      noise.lc.counter = length_table[(data & 0xF8) >> 3];
      break;

    case 0x4015:  // 通道开关
      pulse[0].enable = data & 0x01;
      pulse[1].enable = data & 0x02;
      noise.enable = data & 0x04;
      // 三角波与 DMC 未实现
      break;
  }
}

uint8_t Nes2A03::cpuRead(uint16_t addr) {
  // $4015：通道状态——各长度计数器是否仍在计数。
  // （帧中断 / DMC 状态位未实现。）
  uint8_t data = 0x00;
  if (addr == 0x4015) {
    data |= (pulse[0].lc.counter > 0) ? 0x01 : 0x00;
    data |= (pulse[1].lc.counter > 0) ? 0x02 : 0x00;
    data |= (noise.lc.counter > 0) ? 0x04 : 0x00;
  }
  return data;
}

void Nes2A03::clock() {
  // 帧计数器：每 6 个 PPU 时钟（= 2 个 CPU 时钟）是一个 APU 时钟。
  // 四分帧节拍驱动包络，半帧节拍驱动长度计数器与扫频——
  // 这就是"音乐时间"的来源。
  bool bQuarterFrameClock = false;
  bool bHalfFrameClock = false;

  if (clock_counter % 6 == 0) {
    frame_clock_counter++;

    // 4-Step Sequence Mode
    if (frame_clock_counter == 3729) {
      bQuarterFrameClock = true;
    }

    if (frame_clock_counter == 7457) {
      bQuarterFrameClock = true;
      bHalfFrameClock = true;
    }

    if (frame_clock_counter == 11186) {
      bQuarterFrameClock = true;
    }

    if (frame_clock_counter == 14916) {
      bQuarterFrameClock = true;
      bHalfFrameClock = true;
      frame_clock_counter = 0;
    }

    if (bQuarterFrameClock) {
      pulse[0].env.clock(pulse[0].halt);
      pulse[1].env.clock(pulse[1].halt);
      noise.env.clock(noise.halt);
    }

    if (bHalfFrameClock) {
      for (int i = 0; i < 2; i++) {
        pulse[i].lc.clock(pulse[i].enable, pulse[i].halt);
        sweep[i].clock(pulse[i].seq.reload, i);
      }
      noise.lc.clock(noise.enable, noise.halt);
    }

    // 推进序列器。序列器的原始输出（占空比模板的最低位）就是真实
    // 2A03 的硬件信号；按 enable/length/sweep/timer 门控后送入每通道
    // 的一阶低通，抑制 Bus::clock 降采样到 44.1kHz 时会混叠的阶跃。
    pulse[0].seq.clock(pulse[0].enable, RotatePulse);
    pulse[1].seq.clock(pulse[1].enable, RotatePulse);
    noise.seq.clock(noise.enable, ShiftNoise);

    // 硬件静音规则基于周期值（reload < 8 时超声波频段，强制静音），
    // 而不是正在倒数的 timer——后者每个周期都会扫过 0..7，会在波形上
    // 凿出周期性的缺口。
    auto gate_pulse = [](const Channel &p, const sweeper &sw) -> double {
      if (!p.enable) return 0.0;
      if (p.lc.counter == 0) return 0.0;
      if (p.seq.reload < 8) return 0.0;
      if (sw.mute) return 0.0;
      return static_cast<double>(p.seq.output) *
             static_cast<double>(p.env.output);
    };

    double raw[3] = {
        gate_pulse(pulse[0], sweep[0]),
        gate_pulse(pulse[1], sweep[1]),
        0.0,
    };
    // 噪声通道没有 "周期 < 8 静音" 的规则——快速噪声是合法设置
    if (noise.enable && noise.lc.counter > 0) {
      raw[2] = static_cast<double>(noise.seq.output) *
               static_cast<double>(noise.env.output);
    }

    // 一阶低通: y += a * (x - y)
    // 此处采样率为 APU 时钟 ~894.886 kHz（NES 主时钟 / 6）。
    // a = 1 - exp(-2*pi*fc/fs); fc = 14 kHz => a ~= 0.0961。
    constexpr double kLpAlpha = 0.0961;
    pulse[0].lp += kLpAlpha * (raw[0] - pulse[0].lp);
    pulse[1].lp += kLpAlpha * (raw[1] - pulse[1].lp);
    noise.lp += kLpAlpha * (raw[2] - noise.lp);
  }

  // 扫频单元的目标周期跟踪以高频进行
  sweep[0].track(pulse[0].seq.reload);
  sweep[1].track(pulse[1].seq.reload);

  pulse1_visual =
      (pulse[0].enable && pulse[0].env.output > 1 && !sweep[0].mute)
          ? pulse[0].seq.reload
          : 2047;
  pulse2_visual =
      (pulse[1].enable && pulse[1].env.output > 1 && !sweep[1].mute)
          ? pulse[1].seq.reload
          : 2047;
  noise_visual =
      (noise.enable && noise.env.output > 1) ? noise.seq.reload : 2047;

  clock_counter++;
}

double Nes2A03::GetOutputSample() {
  // NES 非线性混音器的线性近似（nesdev wiki）：
  //   pulse_out = 0.00752 * (pulse1 + pulse2)
  //   tnd_out   = 0.00851*triangle + 0.00494*noise + 0.00335*dmc
  // 各通道音量为 0..15；三角波与 DMC 未实现。
  double pulse_out = 0.00752 * (pulse[0].lp + pulse[1].lp);
  double tnd_out = 0.00494 * noise.lp;
  double mixed = pulse_out + tnd_out;

  // 混音后的 DC blocker（一阶高通）：
  //   y[n] = x[n] - x[n-1] + R * y[n-1]，R 接近 1。
  // 通道开/关引起的直流台阶被它消除，不再变成扬声器里的咔哒声。
  constexpr double kHpR = 0.9985;
  double y = mixed - mixer_hp_in + kHpR * mixer_hp_out;
  mixer_hp_in = mixed;
  mixer_hp_out = y;

  // 混音公式峰值约 0.3，提升到与旧实现相当的输出电平
  return y * 2.5;
}

void Nes2A03::reset() {
  pulse[0] = Channel{};
  pulse[1] = Channel{};
  noise = Channel{};
  noise.seq.sequence = 0xDBDB;  // LFSR 种子
  sweep[0] = sweeper{};
  sweep[1] = sweeper{};
  frame_clock_counter = 0;
  clock_counter = 0;
  mixer_hp_in = 0.0;
  mixer_hp_out = 0.0;
  pulse1_visual = 2047;
  pulse2_visual = 2047;
  noise_visual = 2047;
}

void Nes2A03::SaveState(std::ostream &os) const {
  PutPod(os, pulse);
  PutPod(os, noise);
  PutPod(os, sweep);
  PutPod(os, frame_clock_counter);
  PutPod(os, clock_counter);
  PutPod(os, mixer_hp_in);
  PutPod(os, mixer_hp_out);
}

void Nes2A03::LoadState(std::istream &is) {
  GetPod(is, pulse);
  GetPod(is, noise);
  GetPod(is, sweep);
  GetPod(is, frame_clock_counter);
  GetPod(is, clock_counter);
  GetPod(is, mixer_hp_in);
  GetPod(is, mixer_hp_out);
}
