// Copyright [2020-2022] <tiansongyu>

#include "Nes2A03.h"

uint8_t Nes2A03::length_table[] = {
    10, 254, 20, 2,  40, 4,  80, 6,  160, 8,  60, 10, 14, 12, 26, 14,
    12, 16,  24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30};

Nes2A03::Nes2A03() { noise_seq.sequence = 0xDBDB; }

Nes2A03::~Nes2A03() {}

void Nes2A03::cpuWrite(uint16_t addr, uint8_t data) {
  switch (addr) {
    case 0x4000:
      switch ((data & 0xC0) >> 6) {
        case 0x00:
          pulse1_seq.new_sequence = 0b01000000;
          pulse1_osc.dutycycle = 0.125;  // 占空比体积
          break;
        case 0x01:
          pulse1_seq.new_sequence = 0b01100000;
          pulse1_osc.dutycycle = 0.250;
          break;
        case 0x02:
          pulse1_seq.new_sequence = 0b01111000;
          pulse1_osc.dutycycle = 0.500;
          break;
        case 0x03:
          pulse1_seq.new_sequence = 0b10011111;
          pulse1_osc.dutycycle = 0.750;
          break;
      }
      pulse1_seq.sequence = pulse1_seq.new_sequence;  // 更新占空比体积
      pulse1_halt = (data & 0x20);        // envelope loop / length counter halt
      pulse1_env.volume = (data & 0x0F);  // 音量
      pulse1_env.disable = (data & 0x10);  // constant volume
      break;

    case 0x4001:
      pulse1_sweep.enabled = data & 0x80;  // Enabled flag
      pulse1_sweep.period =
          (data & 0x70) >> 4;  // The divider's period is P + 1 half-frames
      pulse1_sweep.down = data & 0x08;  // 启用标志 Enabled flag
      pulse1_sweep.shift =
          data & 0x07;  // 移位计数器 Shift count (number of bits)
      pulse1_sweep.reload = true;  // ?
      break;

    case 0x4002:
      pulse1_seq.reload =
          (pulse1_seq.reload & 0xFF00) | data;  // Pulse 1 timer Low 8 bits
      break;

    case 0x4003:
      pulse1_seq.reload = (uint16_t)((data & 0x07)) << 8 |
                          (pulse1_seq.reload & 0x00FF);  // timer High 3 bits
      pulse1_seq.timer = pulse1_seq.reload;              // seq被赋值
      pulse1_seq.sequence = pulse1_seq.new_sequence;  // 更新占空比体积
      pulse1_lc.counter =
          length_table[(data & 0xF8) >> 3];  // Pulse 2 length counter load
      pulse1_env.start = true;               // ?
      break;

    case 0x4004:
      switch ((data & 0xC0) >> 6) {
        case 0x00:
          pulse2_seq.new_sequence = 0b01000000;
          pulse2_osc.dutycycle = 0.125;
          break;
        case 0x01:
          pulse2_seq.new_sequence = 0b01100000;
          pulse2_osc.dutycycle = 0.250;
          break;
        case 0x02:
          pulse2_seq.new_sequence = 0b01111000;
          pulse2_osc.dutycycle = 0.500;
          break;
        case 0x03:
          pulse2_seq.new_sequence = 0b10011111;
          pulse2_osc.dutycycle = 0.750;
          break;
      }
      pulse2_seq.sequence = pulse2_seq.new_sequence;
      pulse2_halt = (data & 0x20);
      pulse2_env.volume = (data & 0x0F);
      pulse2_env.disable = (data & 0x10);
      break;

    case 0x4005:
      pulse2_sweep.enabled = data & 0x80;
      pulse2_sweep.period = (data & 0x70) >> 4;
      pulse2_sweep.down = data & 0x08;
      pulse2_sweep.shift = data & 0x07;
      pulse2_sweep.reload = true;
      break;

    case 0x4006:
      pulse2_seq.reload = (pulse2_seq.reload & 0xFF00) | data;
      break;

    case 0x4007:
      pulse2_seq.reload =
          (uint16_t)((data & 0x07)) << 8 | (pulse2_seq.reload & 0x00FF);
      pulse2_seq.timer = pulse2_seq.reload;
      pulse2_seq.sequence = pulse2_seq.new_sequence;
      pulse2_lc.counter = length_table[(data & 0xF8) >> 3];
      pulse2_env.start = true;

      break;

    case 0x4008:
      break;

    case 0x400C:
      noise_env.volume = (data & 0x0F);   // , volume/envelope
      noise_env.disable = (data & 0x10);  // , constant volume
      noise_halt = (data & 0x20);         // Envelope loop / length counter halt
      break;

    case 0x400E:
      // Loop noise ???
      switch (data & 0x0F) {  //  noise period (P)
        case 0x00:
          noise_seq.reload = 0;
          break;
        case 0x01:
          noise_seq.reload = 4;
          break;
        case 0x02:
          noise_seq.reload = 8;
          break;
        case 0x03:
          noise_seq.reload = 16;
          break;
        case 0x04:
          noise_seq.reload = 32;
          break;
        case 0x05:
          noise_seq.reload = 64;
          break;
        case 0x06:
          noise_seq.reload = 96;
          break;
        case 0x07:
          noise_seq.reload = 128;
          break;
        case 0x08:
          noise_seq.reload = 160;
          break;
        case 0x09:
          noise_seq.reload = 202;
          break;
        case 0x0A:
          noise_seq.reload = 254;
          break;
        case 0x0B:
          noise_seq.reload = 380;
          break;
        case 0x0C:
          noise_seq.reload = 508;
          break;
        case 0x0D:
          noise_seq.reload = 1016;
          break;
        case 0x0E:
          noise_seq.reload = 2034;
          break;
        case 0x0F:
          noise_seq.reload = 4068;
          break;
      }
      break;

    case 0x4015:                    // APU STATUS
      pulse1_enable = data & 0x01;  // 脉冲通道1启用判断
      pulse2_enable = data & 0x02;  // 脉冲通道2启用判断
      noise_enable = data & 0x04;   // 噪声 启用判断
                                   // 还有DMC和三角启用判断，这里没有使用
      break;

    case 0x400F:
      pulse1_env.start = true;
      pulse2_env.start = true;
      noise_env.start = true;
      noise_lc.counter =
          length_table[(data & 0xF8) >>
                       3];  // 长度计数器负载 (L)   Length counter load
      break;
  }
}

uint8_t Nes2A03::cpuRead(uint16_t addr) {
  uint8_t data = 0x00;

  if (addr == 0x4015) {
    //  data |= (pulse1_lc.counter > 0) ? 0x01 : 0x00;
    //  data |= (pulse2_lc.counter > 0) ? 0x02 : 0x00;
    //  data |= (noise_lc.counter > 0) ? 0x04 : 0x00;
  }

  return data;
}

void Nes2A03::clock() {
  // Depending on the frame count, we set a flag to tell
  // us where we are in the sequence. Essentially, changes
  // to notes only occur at these intervals, meaning, in a
  // way, this is responsible for ensuring musical time is
  // maintained.
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

    // Update functional units

    // Quater frame "beats" adjust the volume envelope
    if (bQuarterFrameClock) {
      pulse1_env.clock(pulse1_halt);
      pulse2_env.clock(pulse2_halt);
      noise_env.clock(noise_halt);
    }

    // Half frame "beats" adjust the note length and
    // frequency sweepers
    if (bHalfFrameClock) {
      pulse1_lc.clock(pulse1_enable, pulse1_halt);
      pulse2_lc.clock(pulse2_enable, pulse2_halt);
      noise_lc.clock(noise_enable, noise_halt);
      pulse1_sweep.clock(pulse1_seq.reload, 0);
      pulse2_sweep.clock(pulse2_seq.reload, 1);
    }

    // Advance each channel's sequencer at the APU rate. The raw sequencer
    // output (LSB of the rotating duty pattern) is the actual NES hardware
    // signal; we then gate it by enable / length / sweep / timer and feed it
    // into a one-pole low-pass to suppress the sub-sample step transitions
    // that would otherwise alias when Bus::clock decimates to 44.1 kHz.

    pulse1_seq.clock(pulse1_enable, [](uint32_t &s) {
      s = ((s & 0x0001) << 7) | ((s & 0x00FE) >> 1);
    });
    pulse2_seq.clock(pulse2_enable, [](uint32_t &s) {
      s = ((s & 0x0001) << 7) | ((s & 0x00FE) >> 1);
    });
    noise_seq.clock(noise_enable, [](uint32_t &s) {
      s = (((s & 0x0001) ^ ((s & 0x0002) >> 1)) << 14) | ((s & 0x7FFF) >> 1);
    });

    auto gate_pulse = [](bool enable, const sequencer &seq,
                         const lengthcounter &lc, const sweeper &sw,
                         const envelope &env) -> double {
      if (!enable) return 0.0;
      if (lc.counter == 0) return 0.0;
      if (seq.timer < 8) return 0.0;
      if (sw.mute) return 0.0;
      return static_cast<double>(seq.output) * static_cast<double>(env.output);
    };

    double p1_raw = gate_pulse(pulse1_enable, pulse1_seq, pulse1_lc,
                               pulse1_sweep, pulse1_env);
    double p2_raw = gate_pulse(pulse2_enable, pulse2_seq, pulse2_lc,
                               pulse2_sweep, pulse2_env);
    double n_raw = 0.0;
    if (noise_enable && noise_lc.counter > 0 && noise_seq.timer >= 8) {
      n_raw =
          static_cast<double>(noise_seq.output) * static_cast<double>(noise_env.output);
    }

    // One-pole LPF: y += a * (x - y)
    // Sample rate at this point is APU rate ~894.886 kHz (NES master / 6).
    // a = 1 - exp(-2*pi*fc/fs); fc=14 kHz => a ~= 0.0961.
    constexpr double kLpAlpha = 0.0961;
    pulse1_lp += kLpAlpha * (p1_raw - pulse1_lp);
    pulse2_lp += kLpAlpha * (p2_raw - pulse2_lp);
    noise_lp += kLpAlpha * (n_raw - noise_lp);

    pulse1_sample = pulse1_lp;
    pulse2_sample = pulse2_lp;
    noise_sample = noise_lp;
    pulse1_output = pulse1_lp;
    pulse2_output = pulse2_lp;
    noise_output = noise_lp;
  }

  // Frequency sweepers change at high frequency
  pulse1_sweep.track(pulse1_seq.reload);
  pulse2_sweep.track(pulse2_seq.reload);

  pulse1_visual = (pulse1_enable && pulse1_env.output > 1 && !pulse1_sweep.mute)
                      ? pulse1_seq.reload
                      : 2047;
  pulse2_visual = (pulse2_enable && pulse2_env.output > 1 && !pulse2_sweep.mute)
                      ? pulse2_seq.reload
                      : 2047;
  noise_visual =
      (noise_enable && noise_env.output > 1) ? noise_seq.reload : 2047;

  clock_counter++;
}

double Nes2A03::GetOutputSample() {
  // NES non-linear mixer, linear approximation (nesdev wiki).
  //   pulse_out = 0.00752 * (pulse1 + pulse2)
  //   tnd_out   = 0.00851*triangle + 0.00494*noise + 0.00335*dmc
  // Pulse / noise volumes are 0..15. Triangle and DMC are not implemented.
  double pulse_out = 0.00752 * (pulse1_output + pulse2_output);
  double tnd_out = 0.00494 * noise_output;
  double mixed = pulse_out + tnd_out;

  // DC blocker (one-pole high-pass) on the mixed signal. The previous code
  // hard-zeroed channels on mute and offset the output by -0.16, producing a
  // step every time a channel toggled. Removing DC eliminates that click.
  //   y[n] = x[n] - x[n-1] + R * y[n-1], R close to 1.
  constexpr double kHpR = 0.9985;
  double y = mixed - mixer_hp_in + kHpR * mixer_hp_out;
  mixer_hp_in = mixed;
  mixer_hp_out = y;

  // Mixer formula peaks around 0.3; lift to roughly the previous output level.
  return y * 2.5;
}

void Nes2A03::reset() {}
