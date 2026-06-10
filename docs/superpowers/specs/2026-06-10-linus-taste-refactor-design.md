# 6502Emulator Linus-taste 重构设计

- 日期：2026-06-10
- 目标分支：`refactor/linus-taste`（master 不直接改动）
- 输入：8 个并行子系统分析 + 综合重构地图（见 `docs/superpowers/specs/` 同目录附录与
  `docs/audio_pop_root_cause.md`）

## 1. 目标

1. **Linus taste 重构**：好的数据结构优先；消灭特殊分支与复制粘贴家族；不引入
   框架式过度抽象（不做 mapper 注册表、不做序列化框架、不做渲染抽象层）。
2. **功能完全正常**：每一步结构性改动都以"字节级一致"为验收标准
   （nestest CPU trace 逐行一致、6 个 mapper 代表 ROM 的帧缓冲哈希一致、
   音频指标一致）；行为修正与结构重构严格分离为独立提交。
3. **可复用性强**：核心库（CPU/PPU/APU/Bus/Cartridge/Mapper）完全摆脱
   olcPixelGameEngine 依赖，可独立编译、链接、headless 运行；GUI 仅是其
   一个消费者。`tests/nes_headless.cpp` 即是"核心可复用"的活证明。
4. **修复 APU 问题**：整合并验证工作区中已有的爆音修复（硬件序列器路径 +
   每通道一阶低通 + 混音后 DC blocker，根因见 `docs/audio_pop_root_cause.md`），
   并在通道结构化之后修复 APU 自身的正确性 bug（详见 §4）。

## 2. 核心原则：重构在前，修 bug 在后

每个子系统按两段提交：

- **A 段（结构重构，保持行为）**：包括保留现有 bug。验收 = 与基线
  逐字节一致（trace / 帧哈希 / 音频指标）。
- **B 段（行为修正）**：在去重后的"单一位置"上做一行级修复，每个修复独立
  提交、注明症状与验证方式。验收 = 针对性验证 + 全 ROM 集合冒烟。

这样接口、结构、行为三类风险互不污染，回归可以二分定位。

## 3. 验证基础设施（已建立）

`tests/nes_headless.cpp` + `tests/olc_impl.cpp`，三种模式：

| 模式 | 命令 | 验收物 |
| --- | --- | --- |
| CPU | `trace rom/nestest.nes 8991` | 逐指令 trace + `$02=00 $03=00`（nestest 全过） |
| PPU/整机 | `frames <rom> 300 [start帧]` | 每 30 帧 FNV 哈希 + 累计哈希 |
| APU | `audio <rom> <秒> [start帧]` | 样本范围/均值(DC)/RMS/最大跳变/点击计数 |

基线 ROM 集（覆盖全部 6 个 mapper）：smb(0)、Castlevania2(1)、contra(2)、
SolomonsKey(3)、Mega Man 3(4)、Doraemon(66)。基线值存于 `tests/baselines.md`。

已量化的 APU 修复效果（SMB 游戏内 6 秒）：HEAD 原版 mean=−0.230（直流偏置，
通道开关时变成扬声器"砰"声）→ 修复版 mean≈0.0003；contra 点击数 298→61。

## 4. 重构内容（按依赖序）

### 阶段 1：提交现有 APU 爆音修复 + 测试设施
工作区中未提交的 Nes2A03 修复原样入库（去除调试插桩），tests/ 入库。

### 阶段 2：CPU（Nes6502）
- A：`std::vector<INSTRUCTION>` 实例表 → 文件作用域 `static const` 256 项
  数组；新增 `AddrMode` 枚举替代 `addrmode == &Nes6502::IMP` 函数指针比较
  （5 处）；8 个分支指令合并为 `Branch(bool)`；3 份中断序列合并为
  `Interrupt(vector, cycles)`（保留现有压栈顺序 bug）；4 处移位/回写尾部
  合并；`disassemble()` 数据驱动。CPU 变为可平凡拷贝（save-state 前提）。
- B：中断压栈顺序修正（先压 P 再置 I）；BRK 返回地址 off-by-one 修正。
  注：nestest 不覆盖 IRQ 行为，用 MMC3 游戏集验证。

### 阶段 3：PPU（Nes2C02）
- A：950 行 `clock()` 按阶段拆为私有方法（每 tick 重建的 6 个 lambda 提升为
  成员函数）；镜像 if 阶梯 → 单一 `NametableIndex(addr)` 查表（ppuRead/ppuWrite
  共用）；调色板镜像特例去重；6 分支精灵图样地址 → 行/单元算术；删除
  从未写入的 `sprNameTable`/`sprSpriteTitle` 等死成员（约 480KB）；补全
  `reset()`。验收：6 ROM 帧哈希逐字节一致。
- B：$2004 OAMDATA 读死代码修复；ONESCREEN_LO/HI 镜像实现；精灵 0 命中
  左缘门 `~(a|b)` 修复；$2006 首写不再清 tram 低字节。

### 阶段 4：核心去 GUI 化 + 构建拆分（可复用性的主体）
- PPU 帧缓冲 `olc::Sprite` → `uint32_t framebuffer[240*256]` + 静态 64 色
  RGBA 表（数值逐字节一致）；`GetPatternTable` 等同样改为纯缓冲。
- `Nes2C02.h` 删除 `#include "olcPixelGameEngine.h"`；核心库零 olc 依赖、
  零 X11/GL/png 链接需求。
- olc 两个头移到 `third_party/olc/`，以 `SYSTEM` include 挂在仅 GUI 链接的
  INTERFACE target 上；删除头内全局 warning pragma 补丁；新增 6 行
  `olcImpl.cpp` 作为唯一实现 TU（不得给 olc 头加 `#pragma once`，实现块在
  include guard 外是有意为之）。
- 根 CMakeLists 统一 C++17 与告警策略；删除死的 `M6502_DEBUG`、不存在的
  `src/private`、冗余 `add_dependencies`、链接行里的 `-std=c++17`。
- 前端每帧把 framebuffer blit 进一个 `olc::Sprite`。
- tests/ 编译进 CMake（核心库 standalone 链接的持续证明）。

### 阶段 5：APU（Nes2A03）
- A：`pulse1_*`/`pulse2_*`/`noise_*` 三族变量 → `struct Channel`（enable/
  halt/seq/env/lc/sweep/lp/visual）+ `pulse[2]` 数组；$4000-$4007 解码经
  `pulse[(addr>>2)&1]` 一次实现；twin duty switch → `kDutySeq[4]` 表；
  16-case 噪声周期 switch → `kNoisePeriod[16]` 表；`sequencer::clock` 的
  `std::function` → 普通函数指针；删除死的 `oscpulse` 等。验收：音频指标
  与阶段 1 后基线一致。
- B（每个独立提交）：
  1. 静音门 `seq.timer >= 8`（活倒数值）→ `seq.reload >= 8`（周期值）；
     噪声通道删除这个本不存在的规则。症状：高音符 26% 占空比缺口、快噪声全哑。
  2. $400F 写不再重启两个 pulse 包络（只 start 噪声包络）。症状：每次鼓点
     重触发旋律音量。
  3. $4015 读返回真实长度计数器状态（现在恒 0）。
  4. `reset()` 补全并由 `Bus::reset()` 调用（复位后不再残留音）。
- 不改任何音频调音常数（kLpAlpha=0.0961、kHpR=0.9985、×2.5 增益、
  44100/1/8/512）——它们承载爆音修复。
- 三角波/DMC 不在本次范围（通道结构化后是自然的后续，单独评估）。

### 阶段 6：Bus / Cartridge / Mapper
- A：`nSystemClockCounter % 3 / % 2` → 显式 `cpuPhase` 计数器 + DMA 奇偶位
  （消灭 uint32 回绕翻倍时钟 bug 类）；DMA 拆出 `DmaTick()`；$6000-$7FFF
  PRG-RAM 上提为 Cartridge 统一窗口，删除 `0xFFFFFFFF` 带内哨兵的 4 处
  SRAM 粘贴；MMC3 12 块 if 分发 → `bank[addr>>10]` 算术；每个 mapper 构造
  即 reset；`virtual ~Mapper()`；Cartridge.cpp 保留朴素 switch（6 个 mapper
  不需要注册表）。
- B：mapper 写 ROM 越界守卫三连（Mapper 0/1/66 的 `vPRGMemory[0]` 破坏）；
  未知 mapper / 坏 iNES 头诚实失败（当前 `bImageValid=true` + 空指针解引用
  崩溃）；$4016 写选通两个手柄、$4017 写只走 APU；控制器读尊重 `bReadOnly`。

### 阶段 7：中断正确性（最高风险，独立验证）
- Bus 不再在 `cpu.irq()` 前自动 `irqClear()`（IRQ 变电平触发，$E000 是唯一
  确认点）。与阶段 2B 的压栈顺序修正联合，预期修复 README 承认的
  Metal Max MMC3 中断 bug。
- 验收：Mega Man 3/6、smb4 光栅分割不回归（帧哈希/截图），Metal Max 改善，
  无 IRQ 风暴（确认 RTI 后不再立刻重入）。
- MMC3 A12 边沿精确时钟**不做**（cycle-260 近似保留；只有 Metal Max 仍坏
  才另行评估）。

### 阶段 8：存档重写 + 前端归并
- 存档：raw `sizeof(Bus)` dump（跨进程 UB、丢 mapper 状态）→ 显式分字段
  `SaveState/LoadState(ostream/istream)` + magic/version 头；各芯片只写 POD
  寄存器块；Cartridge 写 mapper 寄存器 + PRG/CHR-RAM。不做序列化框架。
  旧 `save_0.dat` 有意作废（它本来就活不过一次重编译）。
- 前端：两份 ~100 行 update 函数 → 单函数 + 运行时 `bAudioDriven` 标志
  （Apple `#ifdef` 缩为默认值选择；Linux 因此获得暂停/单步键）；控制器
  映射表驱动 + 单次原子写入（消除音频线程半字节竞态）；static
  `SoundOut`/`pInstance` 单例 → 捕获 lambda；`main(argc, argv)` 接受 ROM
  路径（默认 `./rom/smb.nes`），加载失败打印原因；reset/load 经
  `std::atomic<bool>` 请求标志在音频线程内消费（消除跨线程整机改写）；
  死代码 DrawCode/DrawAudio/mapAsm 删除。

### 阶段 9：终审
多智能体 review workflow 对全部 diff 做正确性/回归/品味三维评审 + 逐条
对抗验证；全套基线复跑；真实 X display 实机启动 + 截图。

## 5. 非目标（明确不做）

- 不重写/升级 olc 第三方头（只搬家 + 去本地 pragma 补丁）。
- 不做 mapper 工厂/注册表、序列化框架、渲染抽象层。
- 不做逐周期 CPU / 逐周期精灵评估重写；保留 JMP 间接页回绕等故意模拟的
  硬件 bug；保留 cycle-260 MMC3 近似。
- 不实现三角波/DMC（结构就绪后的后续工作）、MMC1 双写忽略、NES2.0。
- 不做性能优化（README 明示本仓库以可读性/硬件还原为先）。
- 不改音频调音常数与 `InitialiseAudio(44100,1,8,512)` 节奏。
- 不兼容旧 save_0.dat。

## 6. 风险与回退

- 每阶段独立提交且可构建；任何阶段验收失败即 `git bisect` 范围只有一个
  提交。分支不动 master。
- 最高风险是阶段 7（IRQ 语义变化），故置于所有"哈希一致"阶段之后单独
  验证，失败可单独 revert 而不影响重构主体。
- 帧哈希对"行为修正"类提交会变化，属预期；每个 B 段提交记录预期变化的
  ROM 与原因，截图人工确认画面正确。
