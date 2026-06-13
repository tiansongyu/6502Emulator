# 重构回归基线（2026-06-10）

由 `tests/nes_headless` 在提交 APU 爆音修复后的代码上生成。
所有"结构重构（A 段）"提交必须与下列值逐字节一致；"行为修正（B 段）"
提交允许偏离，但必须在提交信息里说明偏离的 ROM 与原因。

## CPU：nestest 自动模式

```
/tmp/nes_headless trace rom/nestest.nes 8991
最后一行: RESULT $02=00 $03=00   （nestest 官方+非官方指令全过）
完整 trace 保存于 /tmp/baseline_trace.txt（8993 行）
```

## PPU/整机：帧哈希（300 帧，CUMULATIVE 为逐帧哈希链）

| ROM | mapper | start帧 | CUMULATIVE |
| --- | --- | --- | --- |
| smb.nes | 0 | 60 | 347fa787444eaea5 |
| Castlevania2.nes | 1 | 120 | 404fe22aedd947d0 |
| contra.nes | 2 | 120 | b91409b7b5dad7d4 |
| SolomonsKey.nes | 3 | 120 | 188733b7d5e43f14 |
| Mega Man 3 (USA).nes | 4 | 120 | 3d97698de547d0f1 |
| Doraemon (J).nes | 66 | 120 | 4a84ed0272ef93a5 |

（PPU 重构补全 reset() 初始化后重新生成；自此累计哈希跨二进制确定，
恢复为主要判据。）

> **B 段行为修正（2026-06-13，APU 精度补全）**：Doraemon 由
> `7fcc11a73649f0b5` 变为 `4a84ed0272ef93a5`。原因：实现了 APU $4017
> 帧计数器的帧 IRQ。真实硬件上电即 4-step / 未抑制，帧 IRQ 本就周期性
> 触发；Doraemon（mapper 66，无 mapper IRQ）会受理它，于是标题画面的
> 计时相位改变。已渲染第 250 帧人工确认画面正常（标题画面无异常）。
> 其余 5 个 ROM 视频基线不变（它们在启动时写 $4017=$40 抑制帧 IRQ，
> 或 I 标志长期为 1）。

## APU：音频指标

| 场景 | 版本 | mean(DC) | rms | 最大跳变 | 点击(>0.05) |
| --- | --- | --- | --- | --- | --- |
| smb 游戏内 6s (start=60) | HEAD 原版 | −0.230 | 0.238 | 0.231 | 10298 |
| smb 游戏内 6s (start=60) | 爆音修复后 | +0.0003 | 0.033 | 0.245 | 7342 |
| smb 游戏内 6s (start=60) | 三角波+精度补全后 | +0.00071 | 0.0539 | 0.334 | 11744 |
| contra 标题 5s | HEAD 原版 | −0.260 | 0.260 | 0.094 | 298 |
| contra 标题 5s | 爆音修复后 | +0.00005 | 0.011 | 0.158 | 61 |
| contra 标题 5s | 三角波+精度补全后 | +0.00034 | 0.0363 | 0.217 | 144 |

说明：方波本身含合法陡沿，"点击计数"不会为 0；爆音的本质指标是
mean(DC) ≈ 0（直流台阶消失）。smb 标题画面本来就静音（实测确认，
游戏内按 Start 后音乐立即启动），不是 bug。

> **B 段行为修正（2026-06-13，APU 精度补全）**：rms 整体上升是预期的，
> 三角波通道（低音/旋律声部）现已发声；mean(DC) 仍 ≈ 0，无爆音回归。
> 同时修正了 $4015 的通道位分配（三角波=bit2、噪声=bit3，旧代码把噪声
> 错接在 bit2），并实现了 $4017 帧计数器（4/5-step 模式 + 帧 IRQ）。
> 这些改动会改变噪声/包络/长度的相位，故音频基线一并更新。DMC 仍未实现。

## 验证策略修订（CPU 阶段发现）

1. `cpuRam` 在库内从不清零，nestest 尾段会执行到未写过的零页字节，
   导致跨进程不确定。harness 现在显式清零 RAM；trace 由此变为确定性。
2. PPU 内部状态（OAM 等）同样未初始化，开机头几帧的垃圾内容随二进制
   布局变化。因此 **CUMULATIVE 哈希只在同一二进制布局内有效**；跨重构
   阶段的判据是 30..300 帧的逐 30 帧采样哈希必须逐一相同。PPU 阶段补全
   reset() 初始化后重新生成 CUMULATIVE 基线，此后恢复以累计哈希为准。

## 重新生成方法

```bash
g++ -std=c++17 -O2 tests/nes_headless.cpp tests/olc_impl.cpp \
  -I 6502/6502Lib/src/public build/6502/6502Lib/libM6502Lib.a \
  -lX11 -lGL -lpthread -lpng -lasound -lstdc++fs -o /tmp/nes_headless
cd rom
/tmp/nes_headless trace nestest.nes 8991 | tail -1
/tmp/nes_headless frames smb.nes 300 60 | tail -1
/tmp/nes_headless audio smb.nes 6 60
```
