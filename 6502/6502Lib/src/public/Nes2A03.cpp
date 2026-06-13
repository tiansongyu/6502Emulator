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

// 三角波的 32 步序列：15 阶下降再 15 阶上升，输出 0..15
static const uint8_t kTriangleSeq[32] = {
    15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
    0,  1,  2,  3,  4,  5,  6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
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
    case 0x4008:  // 三角波：线性计数器 control 位 + reload 值
      triangle.control = (data & 0x80);
      triangle.linear_reload = (data & 0x7F);
      break;

    case 0x400A:  // 三角波：周期低 8 位
      triangle.reload = (triangle.reload & 0xFF00) | data;
      break;

    case 0x400B:  // 三角波：周期高 3 位 + 长度计数器装载 + 置线性 reload 标志
      triangle.reload =
          (uint16_t)((data & 0x07) << 8) | (triangle.reload & 0x00FF);
      triangle.lc.counter = length_table[(data & 0xF8) >> 3];
      triangle.linear_reload_flag = true;
      break;

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
      // 位分配：bit0 脉冲1，bit1 脉冲2，bit2 三角波，bit3 噪声，bit4 DMC。
      // 关闭某个通道的同时，硬件立即把它的长度计数器清零——
      // 轮询 $4015 的音频驱动依赖这个即时反馈。
      pulse[0].enable = data & 0x01;
      pulse[1].enable = data & 0x02;
      triangle.enable = data & 0x04;
      noise.enable = data & 0x08;
      if (!pulse[0].enable) pulse[0].lc.counter = 0;
      if (!pulse[1].enable) pulse[1].lc.counter = 0;
      if (!triangle.enable) triangle.lc.counter = 0;
      if (!noise.enable) noise.lc.counter = 0;
      // DMC 未实现
      break;

    case 0x4017:  // 帧计数器：序列模式 + 帧 IRQ 抑制
      frame_mode_5 = (data & 0x80);
      irq_inhibit = (data & 0x40);
      if (irq_inhibit) frame_irq = false;  // 抑制位立即清除待决帧 IRQ
      // 写入复位帧序列分频器；5-step 模式额外立即产生一次 quarter+half
      //（真实硬件有 3-4 CPU 周期延迟，此处简化为立即）。
      frame_clock_counter = 0;
      if (frame_mode_5) {
        clockQuarterFrame();
        clockHalfFrame();
      }
      break;
  }
}

uint8_t Nes2A03::cpuRead(uint16_t addr) {
  // $4015：通道状态——各长度计数器是否仍在计数，外加帧中断标志（bit6）。
  //（DMC 状态位未实现。）读取的副作用是清除帧中断标志。
  uint8_t data = 0x00;
  if (addr == 0x4015) {
    data |= (pulse[0].lc.counter > 0) ? 0x01 : 0x00;
    data |= (pulse[1].lc.counter > 0) ? 0x02 : 0x00;
    data |= (triangle.lc.counter > 0) ? 0x04 : 0x00;
    data |= (noise.lc.counter > 0) ? 0x08 : 0x00;
    data |= frame_irq ? 0x40 : 0x00;
    frame_irq = false;
  }
  return data;
}

// 四分帧节拍：包络发生器 + 三角波线性计数器
void Nes2A03::clockQuarterFrame() {
  pulse[0].env.clock(pulse[0].halt);
  pulse[1].env.clock(pulse[1].halt);
  noise.env.clock(noise.halt);
  triangle.clockLinear();
}

// 半帧节拍：长度计数器 + 扫频单元
void Nes2A03::clockHalfFrame() {
  for (int i = 0; i < 2; i++) {
    pulse[i].lc.clock(pulse[i].enable, pulse[i].halt);
    sweep[i].clock(pulse[i].seq.reload, i);
  }
  noise.lc.clock(noise.enable, noise.halt);
  triangle.lc.clock(triangle.enable, triangle.control);
}

void Nes2A03::clock() {
  // 三角波 timer 以 CPU 速率运行（脉冲的两倍）。CPU 周期边界落在
  // apu_phase 0 与 3（PPU:CPU = 3:1，APU 相位在 0..5 循环）。
  if (apu_phase == 0 || apu_phase == 3) {
    triangle.clockTimer(kTriangleSeq);
  }

  // 帧计数器：每 6 个 PPU 时钟（= 2 个 CPU 时钟）是一个 APU 时钟。
  // 四分帧节拍驱动包络与三角波线性计数器，半帧节拍驱动长度计数器与
  // 扫频——这就是"音乐时间"的来源。计数单位为 APU 时钟。
  if (apu_phase == 0) {
    frame_clock_counter++;

    // 4-step 模式在 14916 处回绕，未抑制时拉高帧 IRQ 线；5-step 模式
    // 多一个静默步、在 18641 处回绕，从不触发 IRQ。
    if (!frame_mode_5) {
      if (frame_clock_counter == 3729) {
        clockQuarterFrame();
      } else if (frame_clock_counter == 7457) {
        clockQuarterFrame();
        clockHalfFrame();
      } else if (frame_clock_counter == 11186) {
        clockQuarterFrame();
      } else if (frame_clock_counter == 14916) {
        clockQuarterFrame();
        clockHalfFrame();
        if (!irq_inhibit) frame_irq = true;
        frame_clock_counter = 0;
      }
    } else {
      if (frame_clock_counter == 3729) {
        clockQuarterFrame();
      } else if (frame_clock_counter == 7457) {
        clockQuarterFrame();
        clockHalfFrame();
      } else if (frame_clock_counter == 11186) {
        clockQuarterFrame();
      } else if (frame_clock_counter == 18641) {
        clockQuarterFrame();
        clockHalfFrame();
        frame_clock_counter = 0;
      }
    }

    // 推进序列器。序列器的原始输出（占空比模板的最低位）就是真实
    // 2A03 的硬件信号；按 enable/length/sweep/timer 门控后送入每通道
    // 的一阶低通，抑制 Bus::clock 降采样到 44.1kHz 时会混叠的阶跃。
    pulse[0].seq.clock(pulse[0].enable, RotatePulse);
    pulse[1].seq.clock(pulse[1].enable, RotatePulse);
    noise.seq.clock(noise.enable, ShiftNoise);

    // 扫频单元跟踪当前周期，刷新静音判定（gate 在下面用到）
    sweep[0].track(pulse[0].seq.reload);
    sweep[1].track(pulse[1].seq.reload);

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
    // 三角波 output 已含静音门控（冻结即恒定 DC，由混音器 DC blocker 消除）
    triangle.lp += kLpAlpha * (static_cast<double>(triangle.output) - triangle.lp);
  }

  if (++apu_phase == 6) apu_phase = 0;
}

double Nes2A03::GetOutputSample() {
  // NES 非线性混音器的线性近似（nesdev wiki）：
  //   pulse_out = 0.00752 * (pulse1 + pulse2)
  //   tnd_out   = 0.00851*triangle + 0.00494*noise + 0.00335*dmc
  // 各通道音量为 0..15；DMC 未实现。
  double pulse_out = 0.00752 * (pulse[0].lp + pulse[1].lp);
  double tnd_out = 0.00851 * triangle.lp + 0.00494 * noise.lp;
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
  triangle = triangle_channel{};
  sweep[0] = sweeper{};
  sweep[1] = sweeper{};
  frame_clock_counter = 0;
  apu_phase = 0;
  // 上电/复位等同 $4017 写 0：4-step 模式、不抑制、无待决帧 IRQ
  frame_mode_5 = false;
  irq_inhibit = false;
  frame_irq = false;
  mixer_hp_in = 0.0;
  mixer_hp_out = 0.0;
}

void Nes2A03::SaveState(std::ostream &os) {
  VisitState([&os](auto &v) { PutPod(os, v); });
}

void Nes2A03::LoadState(std::istream &is) {
  VisitState([&is](auto &v) { GetPod(is, v); });
}
