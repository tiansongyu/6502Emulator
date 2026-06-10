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

static std::shared_ptr<Cartridge> loadCart(const char *path) {
  auto cart = std::make_shared<Cartridge>(path);
  if (!cart->ImageValid()) {
    fprintf(stderr, "failed to load rom: %s\n", path);
    exit(1);
  }
  return cart;
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

static int runFrames(const char *rom, int frames, int startAt) {
  Bus nes;
  zeroRam(nes);
  nes.insertCartridge(loadCart(rom));
  nes.SetSampleFrequency(44100);
  nes.reset();
  uint64_t cumulative = 0xcbf29ce484222325ULL;
  for (int f = 1; f <= frames; f++) {
    nes.controller[0] = 0x00;
    if (startAt > 0 && f >= startAt && f < startAt + 10)
      nes.controller[0] = 0x10;  // Start button
    runFrame(nes);
    const uint8_t *px = reinterpret_cast<const uint8_t *>(nes.ppu.GetScreen());
    uint64_t h = fnv1a(px, 256 * 240 * 4);
    cumulative = fnv1a(reinterpret_cast<const uint8_t *>(&h), 8, cumulative);
    if (f % 30 == 0) printf("frame %4d hash %016" PRIx64 "\n", f, h);
  }
  printf("CUMULATIVE %016" PRIx64 "\n", cumulative);
  return 0;
}

static int runAudio(const char *rom, double seconds, int startAt) {
  Bus nes;
  zeroRam(nes);
  nes.insertCartridge(loadCart(rom));
  nes.SetSampleFrequency(44100);
  nes.reset();
  const long want = static_cast<long>(seconds * 44100.0);
  long got = 0, clicks = 0, frame = 0;
  double prev = 0.0, maxDelta = 0.0, sum = 0.0, sumSq = 0.0;
  double lo = 1e9, hi = -1e9;
  while (got < want) {
    bool sampleReady = nes.clock();
    if (nes.ppu.frame_complete) {
      nes.ppu.frame_complete = false;
      frame++;
      nes.controller[0] =
          (startAt > 0 && frame >= startAt && frame < startAt + 10) ? 0x10
                                                                    : 0x00;
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

int main(int argc, char **argv) {
  if (argc < 4) {
    fprintf(stderr,
            "usage: %s trace <rom> <count>\n"
            "       %s frames <rom> <n> [press-start-frame]\n"
            "       %s audio <rom> <seconds> [press-start-frame]\n",
            argv[0], argv[0], argv[0]);
    return 2;
  }
  std::string mode = argv[1];
  if (mode == "trace") return runTrace(argv[2], atoi(argv[3]));
  if (mode == "frames")
    return runFrames(argv[2], atoi(argv[3]), argc > 4 ? atoi(argv[4]) : 0);
  if (mode == "audio")
    return runAudio(argv[2], atof(argv[3]), argc > 4 ? atoi(argv[4]) : 0);
  fprintf(stderr, "unknown mode %s\n", mode.c_str());
  return 2;
}
