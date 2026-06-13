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
// SDL2 前端。模拟器核心（M6502Lib）不依赖此文件，可独立复用。
//
// 设计：单线程 + 音频队列起搏。主循环里跑一帧模拟、顺手收集这一帧产生
// 的音频样本、推入 SDL 音频队列，再把 PPU 帧缓冲拷进纹理显示。节奏由
// 音频队列水位控制——队列里攒够约 3 帧就停一拍，队列被声卡抽干就再跑
// 一帧，这样模拟器自然跑在实时速率上，音画同步。单线程也彻底避免了
// 手柄/帧缓冲的跨线程竞争。
#include <SDL.h>

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "Bus.h"

namespace {

constexpr int kNesW = 256;
constexpr int kNesH = 240;
constexpr int kScale = 3;  // 窗口 = 256x240 的 3 倍
constexpr int kSampleRate = 44100;

// 手柄按键映射：bit7..bit0 = A B Select Start 上 下 左 右（与 readme 一致）
struct PadKey {
  SDL_Scancode sc;
  uint8_t bit;
};
constexpr PadKey kPad[8] = {
    {SDL_SCANCODE_K, 0x80},  // A
    {SDL_SCANCODE_J, 0x40},  // B
    {SDL_SCANCODE_Y, 0x20},  // Select
    {SDL_SCANCODE_T, 0x10},  // Start
    {SDL_SCANCODE_W, 0x08},  // Up
    {SDL_SCANCODE_S, 0x04},  // Down
    {SDL_SCANCODE_A, 0x02},  // Left
    {SDL_SCANCODE_D, 0x01},  // Right
};

}  // namespace

int main(int argc, char** argv) {
  const char* romPath = argc > 1 ? argv[1] : "./rom/smb.nes";
  const std::string savePath = "./save_0.dat";

  // ---- 装载卡带、复位整机 ----
  Bus nes;
  auto cart = std::make_shared<Cartridge>(romPath);
  if (!cart->ImageValid()) {
    // Cartridge 已向 stderr 输出具体原因
    std::fprintf(stderr, "failed to load rom: %s\n", romPath);
    return 1;
  }
  nes.insertCartridge(cart);
  nes.SetSampleFrequency(kSampleRate);
  nes.reset();

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
    std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Window* win = SDL_CreateWindow(
      "olcNES (SDL2)", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      kNesW * kScale, kNesH * kScale, SDL_WINDOW_SHOWN);
  SDL_Renderer* ren =
      SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
  // 核心 PPU 帧缓冲是每像素 0xAARRGGBB 打包（字节序 r,g,b,a），与
  // SDL_PIXELFORMAT_ABGR8888 一致，可整块拷贝、无需逐像素转换。
  SDL_Texture* tex =
      SDL_CreateTexture(ren, SDL_PIXELFORMAT_ABGR8888,
                        SDL_TEXTUREACCESS_STREAMING, kNesW, kNesH);

  // ---- 音频：浮点单声道队列输出 ----
  SDL_AudioSpec want{}, have{};
  want.freq = kSampleRate;
  want.format = AUDIO_F32SYS;
  want.channels = 1;
  want.samples = 512;
  want.callback = nullptr;  // 用 SDL_QueueAudio，不用回调线程
  SDL_AudioDeviceID dev = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0);
  if (dev) SDL_PauseAudioDevice(dev, 0);

  // 队列水位目标：约 3 帧的音频（每帧 ~735 样本）。攒够就停一拍，
  // 被抽干就跑下一帧——这就是实时节拍器。
  const Uint32 kTargetBytes =
      static_cast<Uint32>(kSampleRate * sizeof(float) * 3 / 60);

  // 无音频设备时（无声模式）退回定时器起搏 ~60fps
  const double kFrameSec = 1.0 / 60.0;
  Uint64 perfFreq = SDL_GetPerformanceFrequency();
  Uint64 lastTick = SDL_GetPerformanceCounter();
  double frameAccum = 0.0;

  std::vector<float> samples;
  samples.reserve(kSampleRate / 50);

  auto render = [&]() {
    SDL_UpdateTexture(tex, nullptr, nes.ppu.GetScreen(),
                      kNesW * static_cast<int>(sizeof(uint32_t)));
    SDL_RenderClear(ren);
    SDL_RenderCopy(ren, tex, nullptr, nullptr);
    SDL_RenderPresent(ren);
  };

  bool running = true;
  bool paused = false;
  while (running) {
    // ---- 事件与热键 ----
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        running = false;
      } else if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch (e.key.keysym.scancode) {
          case SDL_SCANCODE_ESCAPE:
            running = false;
            break;
          case SDL_SCANCODE_BACKSPACE:  // 复位
            nes.reset();
            break;
          case SDL_SCANCODE_SPACE:  // 暂停/继续
            paused = !paused;
            if (dev) SDL_PauseAudioDevice(dev, paused ? 1 : 0);
            break;
          case SDL_SCANCODE_F1: {  // 存档
            std::ostringstream os(std::ios::binary);
            nes.SaveState(os);
            std::ofstream ofs(savePath, std::ofstream::binary);
            if (ofs) ofs << os.str();
            break;
          }
          case SDL_SCANCODE_F2: {  // 读档（头部校验失败则保持原样）
            std::ifstream ifs(savePath, std::ifstream::binary);
            if (ifs) {
              std::stringstream ss;
              ss << ifs.rdbuf();
              nes.LoadState(ss);
            }
            break;
          }
          default:
            break;
        }
      }
    }

    // ---- 手柄：组装当前按键字节 ----
    const Uint8* ks = SDL_GetKeyboardState(nullptr);
    uint8_t pad = 0x00;
    for (const auto& m : kPad)
      if (ks[m.sc]) pad |= m.bit;
    nes.controller[0] = pad;

    // ---- 暂停：保持窗口与画面，不推进模拟、不出声 ----
    if (paused) {
      lastTick = SDL_GetPerformanceCounter();  // 暂停期间不累积定时器
      render();
      SDL_Delay(16);
      continue;
    }

    // ---- 是否该再跑一帧（音频水位 / 定时器起搏）----
    bool runFrame;
    if (dev) {
      runFrame = SDL_GetQueuedAudioSize(dev) < kTargetBytes;
    } else {
      Uint64 now = SDL_GetPerformanceCounter();
      frameAccum += static_cast<double>(now - lastTick) / perfFreq;
      lastTick = now;
      runFrame = frameAccum >= kFrameSec;
      if (runFrame) frameAccum -= kFrameSec;
    }

    if (!runFrame) {
      SDL_Delay(1);  // 攒够缓冲，让出 CPU
      continue;
    }

    // ---- 跑一帧，顺手收集音频样本 ----
    samples.clear();
    do {
      if (nes.clock()) samples.push_back(static_cast<float>(nes.dAudioSample));
    } while (!nes.ppu.frame_complete);
    nes.ppu.frame_complete = false;

    if (dev && !samples.empty())
      SDL_QueueAudio(dev, samples.data(),
                     static_cast<Uint32>(samples.size() * sizeof(float)));

    render();
  }

  if (dev) SDL_CloseAudioDevice(dev);
  SDL_DestroyTexture(tex);
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  SDL_Quit();
  return 0;
}
