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
//
// GUI 前端。模拟器核心（M6502Lib）不依赖此文件，可独立复用。
//
// 线程模型：开启音频时，模拟器由声卡线程驱动（合成回调里
// while(!nes.clock())），GUI 线程采集输入并读取帧缓冲。
// 一切对整机的修改（reset / 读档 / 存档）都通过原子请求标志
// 交给驱动线程执行。调试面板（CPU 寄存器/OAM/模式表）直接读取
// 运行中的机器，可能读到撕裂的中间值——它们只影响显示，不影响
// 模拟本身。
#include <atomic>
#include <fstream>
#include <sstream>
#include <string>

#include "Bus.h"

#include "olcPixelGameEngine.h"

#include "olcPGEX_Sound.h"

class Demo_olcNES : public olc::PixelGameEngine {
 public:
  explicit Demo_olcNES(std::string romPath) : sRomPath(std::move(romPath)) {
    sAppName = "olcNES";
  }

 private:
  // The NES
  Bus nes;
  std::shared_ptr<Cartridge> cart;
  std::string sRomPath;
  std::string sSavePath = "./save_0.dat";

  // 音频驱动模式：声卡线程拉动模拟器（实时运行）。
  // 音频不可用（如 macOS stub 后端）时退回 GUI 线程按 60fps 驱动，
  // 并获得暂停/单步调试键。
  bool bAudioDriven = false;
  bool bEmulationRun = true;
  float fResidualTime = 0.0f;

  uint8_t nSelectedPalette = 0x00;
  // 模式表调试视图的刷新节流计数器（详见 OnUserUpdate 末尾）
  int nPatternRefresh = 0;

  // 跨线程请求：GUI 线程置位，驱动模拟器的线程消费。
  // 存档/读档的文件 IO 留在 GUI 线程（实时音频线程上一次磁盘卡顿
  // 就是一次爆音），音频线程只做内存内的序列化/反序列化。
  std::atomic<bool> bResetRequested{false};
  std::atomic<bool> bSaveRequested{false};
  std::atomic<bool> bLoadRequested{false};
  std::atomic<bool> bSaveReady{false};
  std::string sStateBuffer;  // 请求标志保护下的传递缓冲

  // 核心库输出的是普通 32 位像素缓冲；这里持有引擎侧精灵，
  // 每帧把缓冲拷入再绘制。
  olc::Sprite sprScreen = olc::Sprite(256, 240);
  olc::Sprite sprPattern[2] = {olc::Sprite(128, 128), olc::Sprite(128, 128)};

  // 手柄按键映射：bit7..bit0 = A B Select Start 上 下 左 右
  static constexpr struct {
    olc::Key key;
    uint8_t bit;
  } kPad[8] = {
      {olc::Key::K, 0x80},  // A
      {olc::Key::J, 0x40},  // B
      {olc::Key::Y, 0x20},  // Select
      {olc::Key::T, 0x10},  // Start
      {olc::Key::W, 0x08},  // Up
      {olc::Key::S, 0x04},  // Down
      {olc::Key::A, 0x02},  // Left
      {olc::Key::D, 0x01},  // Right
  };

  // 把核心库的 32 位像素缓冲写入引擎精灵（逐像素写 n 成员，
  // 避免对带构造函数的 olc::Pixel 做 memcpy）
  static void Blit(const uint32_t *src, olc::Sprite &dst, int count) {
    olc::Pixel *px = dst.GetData();
    for (int i = 0; i < count; i++) px[i].n = src[i];
  }

  std::string hex(uint32_t n, uint8_t d) {
    std::string s(d, '0');
    for (int i = d - 1; i >= 0; i--, n >>= 4)
      s[i] = "0123456789ABCDEF"[n & 0xF];
    return s;
  }

  void DrawCpu(int x, int y) {
    DrawString(x, y, "STATUS:", olc::WHITE);
    DrawString(x + 64, y, "N",
               nes.cpu.status & Nes6502::N ? olc::GREEN : olc::RED);
    DrawString(x + 80, y, "V",
               nes.cpu.status & Nes6502::V ? olc::GREEN : olc::RED);
    DrawString(x + 96, y, "-",
               nes.cpu.status & Nes6502::U ? olc::GREEN : olc::RED);
    DrawString(x + 112, y, "B",
               nes.cpu.status & Nes6502::B ? olc::GREEN : olc::RED);
    DrawString(x + 128, y, "D",
               nes.cpu.status & Nes6502::D ? olc::GREEN : olc::RED);
    DrawString(x + 144, y, "I",
               nes.cpu.status & Nes6502::I ? olc::GREEN : olc::RED);
    DrawString(x + 160, y, "Z",
               nes.cpu.status & Nes6502::Z ? olc::GREEN : olc::RED);
    DrawString(x + 178, y, "C",
               nes.cpu.status & Nes6502::C ? olc::GREEN : olc::RED);
    DrawString(x, y + 10, "PC: $" + hex(nes.cpu.pc, 4));
    DrawString(
        x, y + 20,
        "A: $" + hex(nes.cpu.a, 2) + "  [" + std::to_string(nes.cpu.a) + "]");
    DrawString(
        x, y + 30,
        "X: $" + hex(nes.cpu.x, 2) + "  [" + std::to_string(nes.cpu.x) + "]");
    DrawString(
        x, y + 40,
        "Y: $" + hex(nes.cpu.y, 2) + "  [" + std::to_string(nes.cpu.y) + "]");
    DrawString(x, y + 50, "Stack P: $" + hex(nes.cpu.stkp, 4));
  }

  // 消费 GUI 线程发来的请求。必须在驱动模拟器的线程上调用，
  // 这样 reset / 读档不会跟 nes.clock() 竞争。
  void ServiceRequests() {
    if (bResetRequested.exchange(false)) nes.reset();
    if (bSaveRequested.exchange(false)) {
      std::ostringstream os(std::ios::binary);
      nes.SaveState(os);
      sStateBuffer = os.str();
      bSaveReady = true;  // 文件写出由 GUI 线程完成
    }
    if (bLoadRequested.exchange(false)) {
      std::istringstream is(sStateBuffer);
      // 头部校验不通过（比如存档属于另一个游戏）时机器保持原样
      nes.LoadState(is);
    }
  }

  bool OnUserCreate() override {
    cart = std::make_shared<Cartridge>(sRomPath);
    if (!cart->ImageValid()) {
      // Cartridge 已向 stderr 输出具体原因
      return false;
    }

    nes.insertCartridge(cart);
    nes.reset();

    // 初始化声音系统，给它一个按需返回样本的回调。
    // 回调运行在声卡线程上：先处理跨线程请求，再推动模拟器
    // 直到产生下一个 44.1kHz 样本——这就是模拟器的实时节拍器。
    nes.SetSampleFrequency(44100);
#if defined(__linux__) || defined(_WIN32) || defined(__EMSCRIPTEN__)
    try {
      bAudioDriven = olc::SOUND::InitialiseAudio(44100, 1, 8, 512);
    } catch (const std::exception &) {
      // ALSA 后端的失败路径会 join 一个从未启动的线程并抛出
      // system_error——捕获它并退回无声模式，而不是直接终止
      bAudioDriven = false;
    }
#else
    // 其他平台的 olcPGEX_Sound 后端是个返回 true 的空桩（线程不跑、
    // 回调永远不会被调用），不能信任其返回值
    bAudioDriven = false;
#endif
    if (bAudioDriven) {
      olc::SOUND::SetUserSynthFunction(
          [this](int nChannel, float /*fGlobalTime*/, float /*fTimeStep*/) {
            if (nChannel != 0) return 0.0f;
            ServiceRequests();
            while (!nes.clock()) {
            }
            return static_cast<float>(nes.dAudioSample);
          });
    }
    return true;
  }

  // We must play nicely now with the sound hardware, so unload
  // it when the application terminates
  bool OnUserDestroy() override {
    // DestroyAudio 会无条件 join 音频线程，未启动时不可调用
    if (bAudioDriven) olc::SOUND::DestroyAudio();
    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override {
    Clear(olc::DARK_BLUE);

    // 手柄：一次性组装完整字节再写入，避免驱动线程读到半成品
    uint8_t pad = 0x00;
    for (auto &m : kPad)
      if (GetKey(m.key).bHeld) pad |= m.bit;
    nes.controller[0] = pad;

    // 热键。文件 IO 在本线程完成：F1 请求驱动线程做内存快照，
    // 就绪后写盘；F2 先读入内存，再请求驱动线程应用。
    if (GetKey(olc::Key::BACK).bPressed) bResetRequested = true;
    if (GetKey(olc::Key::F1).bPressed) bSaveRequested = true;
    if (bSaveReady.exchange(false)) {
      std::ofstream ofs(sSavePath, std::ofstream::binary);
      if (ofs) ofs.write(sStateBuffer.data(), sStateBuffer.size());
    }
    if (GetKey(olc::Key::F2).bPressed && !bLoadRequested) {
      std::ifstream ifs(sSavePath, std::ifstream::binary);
      if (ifs) {
        std::ostringstream buf;
        buf << ifs.rdbuf();
        sStateBuffer = buf.str();
        bLoadRequested = true;
      }
    }
    bool bPaletteChanged = false;
    if (GetKey(olc::Key::P).bPressed) {
      (++nSelectedPalette) &= 0x07;
      bPaletteChanged = true;
    }

    if (!bAudioDriven) {
      // 无音频后备：GUI 线程自己驱动，附带暂停/单步调试键
      ServiceRequests();

      if (GetKey(olc::Key::SPACE).bPressed) bEmulationRun = !bEmulationRun;

      if (bEmulationRun) {
        if (fResidualTime > 0.0f) {
          fResidualTime -= fElapsedTime;
        } else {
          fResidualTime += (1.0f / 60.0f) - fElapsedTime;
          do {
            nes.clock();
          } while (!nes.ppu.frame_complete);
          nes.ppu.frame_complete = false;
        }
      } else {
        // 单步一条 CPU 指令
        if (GetKey(olc::Key::C).bPressed) {
          do {
            nes.clock();
          } while (!nes.cpu.complete());
          // CPU 时钟只占系统时钟的 1/3，把残余的系统周期排空
          do {
            nes.clock();
          } while (nes.cpu.complete());
        }

        // 单步一整帧
        if (GetKey(olc::Key::F).bPressed) {
          do {
            nes.clock();
          } while (!nes.ppu.frame_complete);
          do {
            nes.clock();
          } while (!nes.cpu.complete());
          nes.ppu.frame_complete = false;
        }
      }
    }

    DrawCpu(516, 2);

    // OAM 前 26 个精灵的调试列表
    for (int i = 0; i < 26; i++) {
      std::string s = hex(i, 2) + ": (" +
                      std::to_string(nes.ppu.pOAM[i * 4 + 3]) + ", " +
                      std::to_string(nes.ppu.pOAM[i * 4 + 0]) + ") " +
                      "ID: " + hex(nes.ppu.pOAM[i * 4 + 1], 2) +
                      +" AT: " + hex(nes.ppu.pOAM[i * 4 + 2], 2);
      DrawString(516, 72 + i * 10, s);
    }

    // 调色板色板 + 当前选择框
    const int nSwatchSize = 6;
    for (int p = 0; p < 8; p++)
      for (int s = 0; s < 4; s++)
        FillRect(516 + p * (nSwatchSize * 5) + s * nSwatchSize, 340,
                 nSwatchSize, nSwatchSize,
                 olc::Pixel(nes.ppu.GetColourFromPaletteRam(p, s)));
    DrawRect(516 + nSelectedPalette * (nSwatchSize * 5) - 1, 339,
             (nSwatchSize * 4), nSwatchSize, olc::WHITE);

    // 模式表（调试视图）：CHR 内容随 mapper bank 切换而变，但每帧
    // 重算两张表要 2×16384 次 ppuRead。节流到每 8 帧刷新一次（肉眼
    // 无差别），切换调色板时立即刷新；其余帧直接重绘已缓存的精灵。
    if (bPaletteChanged || nPatternRefresh == 0) {
      for (int i = 0; i < 2; i++)
        Blit(nes.ppu.GetPatternTable(i, nSelectedPalette), sprPattern[i],
             128 * 128);
    }
    if (++nPatternRefresh >= 8) nPatternRefresh = 0;
    DrawSprite(516, 348, &sprPattern[0]);
    DrawSprite(648, 348, &sprPattern[1]);

    // 游戏画面（2 倍缩放）
    Blit(nes.ppu.GetScreen(), sprScreen, 256 * 240);
    DrawSprite(0, 0, &sprScreen, 2);
    return true;
  }
};

constexpr decltype(Demo_olcNES::kPad) Demo_olcNES::kPad;

int main(int argc, char **argv) {
  const char *rom = argc > 1 ? argv[1] : "./rom/smb.nes";
  Demo_olcNES demo(rom);
  demo.Construct(780, 480, 2, 2);
  demo.Start();
  return 0;
}
