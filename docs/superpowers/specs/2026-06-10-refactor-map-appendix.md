# 6502Emulator Refactor Map (Linus-taste edition)

Synthesized from subsystem analyses of CPU (Nes6502), PPU (Nes2C02), APU (Nes2A03), Bus/Cartridge, Mappers, Frontend, and Build/Vendor. All paths relative to `/home/ubuntu/github/6502Emulator/`.

---

## 1. Cross-cutting themes

**T1 — The GUI engine is welded into the core through one include.**
`6502/6502Lib/src/public/Nes2C02.h:8` includes olcPixelGameEngine.h because the PPU's framebuffer/palette are `olc::Sprite`/`olc::Pixel` members. This single fact makes libM6502Lib.a unlinkable standalone (verified undefined `olc::` symbols), drags X11/GL/png into every consumer, and exports a 6212-line vendor header from the core's PUBLIC include dir. The PPU only uses `Pixel(r,g,b)` and `Sprite::SetPixel` — the fix is a dumber data structure (`uint32_t framebuffer[240*256]` + static 64-entry RGB table), not an abstraction layer. The APU proves the pattern works: it is already olc-free and crosses the boundary via plain `double dAudioSample`.

**T2 — Save-state is a raw `memcpy` of the Bus, and it poisons everything.**
`olcNes_PPU_Backgrounds.cpp:240/248/360` writes `sizeof(nes)` raw bytes: serializes the CPU's heap-backed `std::vector lookup`, `Bus*` back-pointer, PPU `olc::Sprite` vectors, `shared_ptr<Cartridge>` control blocks — UB across process runs — and *misses* all mapper bank registers and PRG/CHR-RAM (silently wrong restores on mappers 1/2/3/4/66). Worse, it makes every member-layout cleanup "break the save format", freezing the codebase. The cure is one explicit, versioned, field-wise `SaveState/LoadState` — done *last*, after layouts settle, so it's written once.

**T3 — Copy-paste families everywhere; the fix is always a table or an array, never a framework.**
- CPU: 8 byte-identical branch opcodes; 3 hand-copied interrupt sequences; 4 duplicated shift/rotate writeback tails; per-instance 256-entry opcode vector rebuilt per object.
- PPU: mirroring ladder duplicated in ppuRead/ppuWrite; palette mirror special-case duplicated; 6-branch sprite-pattern-address; 6 lambdas redefined every clock tick inside a 950-line function.
- APU: `pulse1_*`/`pulse2_*`/`noise_*` parallel variable triples; twin duty switches; 16-case noise-period switch.
- Mappers: 12 copy-pasted bank-dispatch if-blocks in MMC3; $6000-$7FFF SRAM block pasted 4x across MMC1/MMC3.
- Frontend: two ~100-line near-duplicate update functions; controller mapping pasted twice.
- Build: identical compiler-flag blocks in two CMakeLists.

**T4 — In-band sentinels and pointers-as-type-tags.**
Member-fn-pointer comparison `addrmode == &Nes6502::IMP` as the accumulator-mode tag (5 sites); magic `mapped_addr == 0xFFFFFFFF` as "mapper handled it" (the direct cause of two ROM-corruption bugs, see #7 below); `MIRROR::HARDWARE` sentinel (this one is fine — keep it). Replace the fn-ptr tag with an `AddrMode` enum in the opcode table; kill the 0xFFFFFFFF sentinel by hoisting PRG-RAM handling into Cartridge/Mapper base.

**T5 — Interrupt handling is wrong and split across three files.**
CPU pushes status *after* setting I (Nes6502.cpp irq/nmi/BRK), Bus auto-acknowledges the mapper IRQ line before the CPU's I-flag gate (Bus.cpp:192-194), and the PPU fires MMC3 `scanline()` at a fixed cycle-260 approximation (Nes2C02.cpp:1400-1402). The first two together drop IRQs entirely while I=1 — the most plausible root cause of the readme-acknowledged Metal Max MMC3 bug.

**T6 — Reset/init is incomplete in every chip.**
`Nes2A03::reset()` is empty and Bus never calls it; `Mapper_004::reset()` leaves IRQ-enable asserted; Mapper_001/004 constructors skip reset() entirely; PPU reset leaves sprite state and `sprite_count` uninitialized (potential OOB index); Bus never zeroes `cpuRam`/`controller_state`. As a library (no GUI calling `nes.reset()`), several mappers are unusable after construction.

**T7 — Threading: the audio thread owns the machine, the GUI thread mutates it anyway.**
`SoundOut` spins `nes.clock()` on the ALSA thread while the GUI thread writes `controller[0]` bit-by-bit, calls `reset()`, and (F2) overwrites the entire object byte-wise. No synchronization anywhere.

**T8 — Dead weight.** ~480KB of never-written `olc::Sprite` members in the PPU, dead `oscpulse`/`*_sample` APU state, dead `DrawCode`/`DrawAudio`/`mapAsm` in the frontend, dead `M6502_DEBUG` define, nonexistent `src/private` include dir, dead `dAudioGlobalTime`.

---

## 2. Dependency-ordered refactor sequence

Each step is independently buildable and committable. Rationale for the order: surgical bug fixes first (better baseline to regression against); then the CPU table change (it unlocks save-state); then PPU internal cleanup, then the framebuffer decouple (it unlocks the build split); APU/mapper/bus structural work next; save-state rewritten **once, last**, after every layout has settled; frontend merge at the end on top of the new core API.

**Step 0 — Surgical bug-fix wave (one commit per fix, no structural change).**
The one/few-liners from section 3 that don't depend on refactoring: PPU $2004 read (#3), sprite-zero gate / sprite-overflow / $2006 low-byte (#11), APU timer-vs-reload mute (#5), APU $400F envelope restart (#6), mapper return-true-without-mapped_addr trio (#7), Cartridge honest failure (#8), `Mapper_004::reset()` clearing `bIRQEnable/bIRQActive`. Each individually testable against a known game.

**Step 1 — CPU: static const opcode table with an `AddrMode` enum.**
Replace the per-instance `std::vector<INSTRUCTION>` (Nes6502.cpp:80-225) with a file-scope `static const Instruction kLookup[256]` (`const char* name`, fn-ptrs, cycles, `AddrMode mode`). Replace all five `== &Nes6502::IMP` compares with `mode == AddrMode::IMP`. Fix the unofficial-opcode rows (real addressing modes for multi-byte NOPs, 0xEB → SBC/IMM) here too. Makes `Nes6502` trivially copyable — the prerequisite for Step 8 — and the constructor empty. *Note: changes `sizeof(Bus)`; same-session F1/F2 still works, cross-session never did.*

**Step 2 — CPU dedup + interrupt correctness.**
- `uint8_t Branch(bool take)` replaces 8 identical bodies (~70 lines deleted).
- `void Interrupt(uint16_t vector, uint8_t n_cycles, bool brk)` replaces the 3 copied sequences and is the single place to fix the I-push-order bug (#1) and the BRK `pc` off-by-one (Nes6502.cpp:917-918).
- `StoreResult(uint8_t)` for the 4 shift/rotate tails; make `temp` a local.
- Data-drive `disassemble()` off the `AddrMode` enum (output strings byte-identical).

**Step 3 — PPU internal restructuring (behavior-preserving except named fixes).**
- Promote the 6 per-tick lambdas to private methods; split `clock()` into BackgroundFetch / EvaluateSprites / LoadSpriteShifters / ComposePixel phase functions (~60-line dispatcher).
- `NametableByte(addr)` helper with the 4x4 mirror LUT — fixes ONESCREEN_LO/HI (#4) and deletes both if-ladders. Same for `PaletteByte(addr)` (`if ((addr & 0x13) == 0x10) addr &= ~0x10`).
- Collapse the 6-branch sprite pattern address into row/cell arithmetic (~85 → ~8 lines).
- Simplify pixel composition to the single `fg_wins` condition.
- Delete dead members (`sprNameTable`, `sprSpriteTitle`, `GetNameTable`), init `sprite_count`, complete `reset()`.

**Step 4 — PPU framebuffer decouple (the core goes olc-free).**
Replace `olc::Sprite sprScreen` / `olc::Pixel palScreen[64]` with `uint32_t framebuffer[240*256]` + `static constexpr uint32_t kPalette[64]` (same RGB values, byte-for-byte). Explicit visibility guard replaces the rely-on-SetPixel-bounds-check hack. `GetPatternTable`/`GetColourFromPaletteRam` fill plain buffers; frontend blits into one `olc::Sprite` per frame (`pColData` is layout-compatible 32-bit RGBA). Delete `#include "olcPixelGameEngine.h"` from Nes2C02.h.

**Step 5 — Build split (depends on Step 4).**
Move olcPixelGameEngine.h/olcPGEX_Sound.h to `third_party/olc/` behind an `olc_pge` INTERFACE target carrying X11/GL/PNG/Threads/ALSA via find_package; only 6502Emulator links it. Add a 6-line `olcImpl.cpp` as the sole OLC_PGE_APPLICATION/OLC_PGEX_SOUND TU. Root CMakeLists owns the C++17 + warnings policy once; delete dead `M6502_DEBUG`, dead `src/private`, redundant `add_dependencies`, `-std=c++17`-as-a-library; use `SYSTEM` includes for third_party instead of the in-header pragma patches (then delete those pragmas and triage what they were hiding). Post-build ROM copy via `add_custom_command`.

**Step 6 — APU: channel struct + array.**
`struct Channel { bool enable, halt; sequencer seq; envelope env; lengthcounter lc; sweeper sweep; double lp; uint16_t visual; }` with `pulse[2]` + `noise`. `cpuWrite` $4000-$4007 decodes once via `pulse[(addr >> 2) & 1]`; clock() becomes loops. Static `kDutySeq[4]` and `kNoisePeriod[16]` tables (fixing the off-by-one / missing-762 / octave issue in one place). Drop `std::function` from `sequencer::clock` (plain fn-ptr). Delete dead `oscpulse`/`*_sample`/`*_output`. Re-enable $4015 status read; decode $4017; implement `reset()` (and have Bus call it — Step 7). Triangle/DMC slot in *later* as data variations.

**Step 7 — Bus + mapper contract cleanup.**
- Explicit phase counters: `uint8_t cpuPhase` (0..2) and a DMA-parity bool replace `nSystemClockCounter % 3 / % 2` (kills the uint32 wrap bug, Bus.cpp:124/135/140).
- Split `DmaTick()` out of `clock()`.
- $4016 write strobes both controllers; $4017 write goes APU-only; honor `bReadOnly` in the controller read branch.
- IRQ level-trigger fix (#2): Bus stops calling `irqClear()`; `$E000` remains the only acknowledge. **Regression-gate this on MMC3 titles.**
- Hoist $6000-$7FFF PRG-RAM into Mapper base / Cartridge window, deleting the 0xFFFFFFFF sentinel and 4 pasted SRAM blocks; MMC3 bank dispatch becomes `pCHRBank[addr >> 10] + (addr & 0x3FF)` / `pPRGBank[(addr >> 13) & 3] + (addr & 0x1FFF)`; make every mapper ctor establish reset() state; `virtual ~Mapper()`; delete tautological range checks and the per-file pragma blocks; keep the plain mapper switch in Cartridge.cpp but move the concrete includes out of Cartridge.h.

**Step 8 — Explicit versioned save-state (depends on 1, 3, 4, 6, 7).**
`Bus::SaveState(std::ostream&)/LoadState(std::istream&)` with magic+version header; each chip writes its POD register block; Cartridge writes mapper registers + PRG/CHR-RAM. No serializer framework. This deliberately breaks `save_0.dat` (which never survived a restart anyway) and makes banked-mapper restores correct for the first time.

**Step 9 — Frontend consolidation.**
Merge the two update functions behind a runtime `bAudioDriven` flag (Apple `#ifdef` shrinks to choosing the default; Linux gains pause/step keys). Table-driven controller mapping with one atomic final store. Capturing lambda replaces static `SoundOut`/`pInstance`. `main(argc, argv)` takes the ROM path with a stderr message on failure. Cross-thread handoff: `std::atomic<bool>` reset/load requests consumed inside the synth callback between clocks. Delete or fully wire DrawCode/DrawAudio/mapAsm — one state, wired or gone.

---

## 3. Top correctness bugs (fix these, with file:line)

1. **IRQ/NMI/BRK push status AFTER setting I** — `Nes6502.cpp:306-309` (irq), `:332-335` (nmi), `:920-927` (BRK). Pushed P has I=1, so RTI leaves interrupts disabled. High.
2. **Bus auto-acknowledges mapper IRQ before the CPU I-gate** — `Bus.cpp:192-194`. IRQ asserted while I=1 is lost forever. (1)+(2) together are the likely Metal Max MMC3 root cause. High.
3. **$2004 OAMDATA read is dead code, always returns 0** — `Nes2C02.cpp:247-252` (`data = pOAM[oam_addr];` sits after the `break`). High.
4. **ONESCREEN_LO/HI mirroring unhandled** — `Nes2C02.cpp:381-393` and `:414-426`; MMC1 one-screen games read 0 / drop writes. High.
5. **APU mute gates test live countdown `seq.timer` instead of period `seq.reload`** — `Nes2A03.cpp:277` (pulse, forces ~26% duty notch on high notes), `:287` (noise — the rule doesn't even exist for noise; fast noise periods go fully silent). High.
6. **$400F write restarts both pulse envelopes** — `Nes2A03.cpp:183-184`. Every drum hit retriggers melody channels. High.
7. **Mapper writes corrupt in-memory ROM byte 0**: `Mapper_066.cpp:36-51` (cpuMapWrite returns true, `mapped_addr` unset → `vPRGMemory[0] = data`), `Mapper_001.cpp:248-258` (same for CHR ROM), `Mapper_000.cpp:57-66` (NROM maps CPU writes into PRG ROM). High.
8. **Unknown mapper still sets `bImageValid = true`, null `pMapper` deref crash** — `Cartridge.cpp:109-114`; iNES magic never checked. High.
9. **Raw save-state dump: UB across runs + mapper/cart RAM never saved** — `olcNes_PPU_Backgrounds.cpp:240, 248, 360`. High (fixed structurally in Step 8).
10. **GUI-thread / audio-thread data race on the whole machine** — `olcNes_PPU_Backgrounds.cpp:149-157` vs `:222-250` (controller torn writes, reset, F2 byte-stomp mid-instruction). High.
11. **PPU trio**: sprite-zero left-edge gate `~(a|b)` always truthy — `Nes2C02.cpp:1375`; `sprite_overflow` can never set — `:1040-1069`; $2006 first write zeroes tram_addr low byte — `:344` (breaks mid-frame split scrolling). Medium.
12. **$4017 write swallowed by APU branch; controller-2 strobe unreachable; $4016 doesn't strobe both pads** — `Bus.cpp:51-68`. Medium.
13. **`nSystemClockCounter` uint32 wrap double-clocks the CPU and flips DMA parity** — `Bus.cpp:124, 135, 140`. Medium.
14. **BRK pushes return address +3 instead of +2** — `Nes6502.cpp:917-918` (double `pc++` via IMM addrmode + BRK body). Medium.
15. **`Nes2A03::reset()` empty and never invoked by `Bus::reset()`** — `Nes2A03.cpp:346`, `Bus.cpp:100-110`; stale tones survive console reset. Medium.

---

## 4. Explicit non-goals

- **Do NOT rewrite or upgrade the vendored olc headers.** Only relocate them to `third_party/`, remove the local global warning pragmas (replacing with `SYSTEM` includes), and keep the one-implementation-TU invariant. Do NOT add `#pragma once` to them — the impl blocks living outside the include guards is load-bearing.
- **No mapper registry/factory.** The plain switch in Cartridge.cpp is exactly the right amount of abstraction for 6 mappers.
- **No serializer framework / no virtual `Serialize()` hierarchy.** Save-state is plain field-by-field POD writes per chip with a version header.
- **No cycle-accurate CPU rewrite.** Keep atomic instruction execution (`cycles==0` boundary), the `extra1 & extra2` cycle algebra, `complete()` semantics, and the deliberately-emulated JMP-indirect page-wrap bug.
- **No hardware-accurate per-cycle sprite evaluation.** The all-at-once cycle-257 evaluation and cycle-340 shifter load are intentional approximations; keep them.
- **MMC3 A12-edge clocking is deferred.** The cycle-260 approximation stays for this refactor; do the Bus acknowledge fix (#2) first and only revisit A12 if Metal Max remains broken (high risk, perturbs all working MMC3 games).
- **Triangle/DMC channels are out of scope** — a follow-up enabled by the Step 6 channel struct, not part of this refactor.
- **No backward compatibility for `save_0.dat`** — the old format never survived a process restart; breaking it is deliberate and documented.
- **No MMC1 double-write ignore / PRG-RAM protect implementation; no NES2.0 support** (explicitly reject NES2.0 headers instead of half-parsing them).
- **No performance cleverness.** The readme's goal is hardware-faithful, readable, deliberately unoptimized code; do not trade datasheet-mirroring structure for speed.
- **Do not change audio tuning constants or cadence**: `kLpAlpha = 0.0961`, `kHpR = 0.9985`, gain 2.5, gate-before-filter ordering, `InitialiseAudio(44100, 1, 8, 512)`, fractional-remainder decimation — these embody the just-fixed pop bug (`docs/audio_pop_root_cause.md`).

---

## 5. Behaviors that must be regression-tested

Test ROMs available in `/home/ubuntu/github/6502Emulator/rom/`: `nestest.nes`, `mmc1_a12.nes`, `smb.nes` (NROM), `contra.nes` (UxROM), `Mega Man 3/6` + `smb4.nes` + `Metal Max (Japan).nes` (MMC3), `Castlevania2.nes` (MMC1), `Doraemon (J).nes` (66-class), plus the 热血 series.

**Per CPU change (Steps 1-2):**
- `nestest.nes` full run, comparing the documented golden log — gates the table conversion, Branch/Interrupt/StoreResult dedup, and unofficial-opcode rows.
- After the I-flag push fix and BRK fix: any IRQ-using game still runs; this *changes* observable behavior by design.

**Per PPU change (Steps 3-4):**
- Pixel-identical frame capture before/after on smb.nes and contra.nes for the mechanical splits (clock() decomposition, composition simplify, framebuffer type swap — the 64 palette RGB values must survive byte-for-byte).
- 8x16-sprite game with vertical flips (Contra/SMB3) after the sprite-pattern-address collapse.
- MMC1 one-screen title renders after the mirror LUT (was: blank/garbage nametables); `mmc1_a12.nes`.
- Sprite-zero hit: SMB status-bar split still works after the `~(a|b)` gate fix and $2006 low-byte restore; mid-frame split-scroll games for the $2006 fix specifically.
- vblank/NMI timing and odd-frame skip: ideally blargg `ppu_vbl_nmi` / `sprite_hit` ROMs.

**MMC3/IRQ (Step 7, highest-risk change):**
- Mega Man 3, Mega Man 6, smb4 must keep working raster splits after removing Bus-side `irqClear()`; Metal Max expected to *improve* (this is the success criterion). Verify the level-triggered re-assert loop terminates ($E000 ack → RTI → no IRQ storm).

**APU (Steps 0/6):**
- SMB/Zelda-class audio: no pops (the LPF/HPF chain untouched), pulse notes without the periodic duty notch (timer→reload fix), drums no longer retrigger melody volume ($400F fix), noise audible at fast periods.
- `clock()`-returns-true cadence: exactly one `GetOutputSample()` per 44.1kHz sample (the GUI sound thread spins on it).
- Console reset (Backspace) now silences audio — deliberate change.

**Bus/timing:**
- 3:1 PPU:CPU ratio and OAM DMA (513/514-cycle stall, sprites intact) after the phase-counter swap; long-run soak (>15 min) to cover the old uint32-wrap territory.
- Controller input MSB-first protocol (A=0x80 … Right=0x01) after the $4016 strobe fix and the frontend table-driven rewrite; player 1 unaffected.

**Save-state (Step 8):**
- Until replaced: same-session F1→F2 round-trip keeps working at every step (layout changes are fine within one binary).
- After replacement: save → quit → relaunch → load works; load on an MMC1/MMC3 game mid-bank-switch restores correct banks (previously silently broken); load while audio thread runs doesn't crash (Step 9 handoff).

**Frontend (Step 9):**
- Step-instruction (C) drains `cpu.complete()` then residual cycles; step-frame (F) clears `ppu.frame_complete` (frontend-owned — keep clearing it or the loop never exits); pause (SPACE) now also on Linux; palette cycling (P) wraps over 8 palettes with instant pattern-table refresh; `disassemble()` debug output format `"$XXXX: MNE operand {MODE}"` unchanged.
- Init order preserved: cartridge inserted + `nes.reset()` + `SetSampleFrequency(44100)` before `InitialiseAudio`; `DestroyAudio()` joins the audio thread before `Bus` destruction.
