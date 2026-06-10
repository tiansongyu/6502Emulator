// Headless regression harness for the NES emulator core.
//
// Proves the core (Bus/CPU/PPU/APU/Cartridge) runs without any GUI, and
// produces deterministic artifacts for before/after refactor comparison:
//
//   trace <rom> <n>            nestest-style CPU trace, n instructions,
//                              starting at PC=0xC000; prints $02/$03 result
//                              codes at the end (0x00 0x00 = pass).
//   frames <rom> <n> [start]   run n video frames; print a framebuffer hash
//                              every 30 frames. Optional: hold Start button
//                              on frames [start, start+10) to enter gameplay.
//   audio <rom> <seconds>      run with 44.1kHz sampling; print click/DC
//                              statistics of the mixed audio output.
#include <cinttypes>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <memory>
#include <sstream>
#include <string>

#include "Bus.h"

static uint64_t fnv1a(const uint8_t *p, size_t n, uint64_t h = 0xcbf29ce484222325ULL) {
  for (size_t i = 0; i < n; i++) {
    h ^= p[i];
    h *= 0x100000001b3ULL;
  }
  return h;
}

// Bus never zeroes cpuRam, so power-on RAM is whatever the process stack
// held - nestest's tail end executes zero-page bytes it never wrote, making
// runs non-deterministic. Zero it explicitly for reproducible baselines.
static void zeroRam(Bus &nes) { memset(nes.cpuRam, 0, sizeof(nes.cpuRam)); }

// startAt > 0：在 [startAt, startAt+10) 按住 Start（进入游戏）。
// startAt < 0：每 |startAt| 帧按住 Start 10 帧（盲穿多级菜单）。
static uint8_t startButton(int startAt, int frame) {
  if (startAt > 0)
    return (frame >= startAt && frame < startAt + 10) ? 0x10 : 0x00;
  if (startAt < 0) return (frame % -startAt) < 10 ? 0x10 : 0x00;
  return 0x00;
}

static std::shared_ptr<Cartridge> loadCart(const char *path) {
  auto cart = std::make_shared<Cartridge>(path);
  if (!cart->ImageValid()) {
    fprintf(stderr, "failed to load rom: %s\n", path);
    exit(1);
  }
  return cart;
}

// 所有模式共用的上电流程：装卡带、设采样率、复位
static void powerOn(Bus &nes, const char *rom) {
  zeroRam(nes);
  nes.insertCartridge(loadCart(rom));
  nes.SetSampleFrequency(44100);
  nes.reset();
}

static void stepInstruction(Bus &nes) {
  nes.cpu.clock();
  while (!nes.cpu.complete()) nes.cpu.clock();
}

static int runTrace(const char *rom, int count) {
  Bus nes;
  zeroRam(nes);
  nes.insertCartridge(loadCart(rom));
  nes.reset();
  nes.cpu.pc = 0xC000;  // nestest automation entry point
  while (!nes.cpu.complete()) nes.cpu.clock();
  nes.cpu.pc = 0xC000;
  for (int i = 0; i < count; i++) {
    printf("%04X A:%02X X:%02X Y:%02X P:%02X SP:%02X\n", nes.cpu.pc, nes.cpu.a,
           nes.cpu.x, nes.cpu.y, nes.cpu.status, nes.cpu.stkp);
    stepInstruction(nes);
  }
  printf("RESULT $02=%02X $03=%02X\n", nes.cpuRam[0x0002], nes.cpuRam[0x0003]);
  return 0;
}

static void runFrame(Bus &nes) {
  do {
    nes.clock();
  } while (!nes.ppu.frame_complete);
  nes.ppu.frame_complete = false;
}

// 步进 n 帧并按 startAt 规则注入 Start 键
static void stepFrames(Bus &nes, int n, int startAt, int base = 0) {
  for (int f = 1; f <= n; f++) {
    nes.controller[0] = startButton(startAt, base + f);
    runFrame(nes);
  }
}

// 当前帧缓冲的 FNV 哈希
static uint64_t hashScreen(const Bus &nes, uint64_t h = 0xcbf29ce484222325ULL) {
  return fnv1a(reinterpret_cast<const uint8_t *>(nes.ppu.GetScreen()),
               256 * 240 * 4, h);
}

static int runFrames(const char *rom, int frames, int startAt) {
  Bus nes;
  powerOn(nes, rom);
  uint64_t cumulative = 0xcbf29ce484222325ULL;
  for (int f = 1; f <= frames; f++) {
    nes.controller[0] = startButton(startAt, f);
    runFrame(nes);
    uint64_t h = hashScreen(nes);
    cumulative = fnv1a(reinterpret_cast<const uint8_t *>(&h), 8, cumulative);
    if (f % 30 == 0) printf("frame %4d hash %016" PRIx64 "\n", f, h);
  }
  printf("CUMULATIVE %016" PRIx64 "\n", cumulative);
  return 0;
}

static int runAudio(const char *rom, double seconds, int startAt) {
  Bus nes;
  powerOn(nes, rom);
  const long want = static_cast<long>(seconds * 44100.0);
  long got = 0, clicks = 0, frame = 0;
  double prev = 0.0, maxDelta = 0.0, sum = 0.0, sumSq = 0.0;
  double lo = 1e9, hi = -1e9;
  while (got < want) {
    bool sampleReady = nes.clock();
    if (nes.ppu.frame_complete) {
      nes.ppu.frame_complete = false;
      frame++;
      nes.controller[0] = startButton(startAt, frame);
    }
    if (!sampleReady) continue;
    double s = nes.dAudioSample;
    if (got > 0) {
      double d = fabs(s - prev);
      if (d > maxDelta) maxDelta = d;
      if (d > 0.05) clicks++;
    }
    prev = s;
    sum += s;
    sumSq += s * s;
    if (s < lo) lo = s;
    if (s > hi) hi = s;
    got++;
  }
  double mean = sum / got;
  double rms = sqrt(sumSq / got);
  printf("samples %ld  range [%+.4f, %+.4f]  mean %+.5f  rms %.4f\n", got, lo,
         hi, mean, rms);
  printf("max sample-to-sample delta %.4f  clicks(>0.05) %ld\n", maxDelta,
         clicks);
  return 0;
}

// 把第 n 帧的帧缓冲以原始 RGBA 形式写出（256x240x4 字节），
// 供外部工具转成图片人工查验画面。
static int runDump(const char *rom, int frame, int startAt, const char *out) {
  Bus nes;
  powerOn(nes, rom);
  stepFrames(nes, frame, startAt);
  FILE *fp = fopen(out, "wb");
  if (!fp) return 1;
  fwrite(nes.ppu.GetScreen(), 4, 256 * 240, fp);
  fclose(fp);
  printf("wrote %s (frame %d)\n", out, frame);
  return 0;
}

// 存档往返测试：跑 a 帧后存档并继续跑 b 帧记录哈希；回读存档再跑
// b 帧；两次哈希必须一致，否则存档遗漏了某些机器状态。
static int runSaveTest(const char *rom, int a, int b, int startAt) {
  Bus nes;
  powerOn(nes, rom);
  stepFrames(nes, a, startAt);

  std::stringstream state;
  nes.SaveState(state);

  auto runAndHash = [&](Bus &n) {
    uint64_t h = 0xcbf29ce484222325ULL;
    for (int f = 0; f < b; f++) {
      n.controller[0] = 0;
      runFrame(n);
      h = hashScreen(n, h);
    }
    return h;
  };

  uint64_t h1 = runAndHash(nes);

  state.clear();
  state.seekg(0);
  if (!nes.LoadState(state)) {
    printf("SAVETEST LOAD FAILED\n");
    return 1;
  }
  uint64_t h2 = runAndHash(nes);

  printf("SAVETEST %s: h1=%016" PRIx64 " h2=%016" PRIx64 " %s\n", rom, h1, h2,
         h1 == h2 ? "PASS" : "FAIL");
  return h1 == h2 ? 0 : 1;
}

int main(int argc, char **argv) {
  if (argc < 4) {
    fprintf(stderr,
            "usage: %s trace <rom> <count>\n"
            "       %s frames <rom> <n> [press-start-frame]\n"
            "       %s audio <rom> <seconds> [press-start-frame]\n"
            "       %s dump <rom> <frame> <press-start-frame|0> <out.rgba>\n"
            "       %s savetest <rom> <warmup-frames> <verify-frames> "
            "<press-start-frame|0>\n",
            argv[0], argv[0], argv[0], argv[0], argv[0]);
    return 2;
  }
  std::string mode = argv[1];
  if (mode == "trace") return runTrace(argv[2], atoi(argv[3]));
  if (mode == "frames")
    return runFrames(argv[2], atoi(argv[3]), argc > 4 ? atoi(argv[4]) : 0);
  if (mode == "audio")
    return runAudio(argv[2], atof(argv[3]), argc > 4 ? atoi(argv[4]) : 0);
  if (mode == "dump" && argc >= 6)
    return runDump(argv[2], atoi(argv[3]), atoi(argv[4]), argv[5]);
  if (mode == "savetest" && argc >= 6)
    return runSaveTest(argv[2], atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
  fprintf(stderr, "unknown mode %s\n", mode.c_str());
  return 2;
}
