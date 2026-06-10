# NES 模拟器(Nintendo Entertainment System emulator)

适用于Windows、UNIX、MAC平台的开源NES模拟器，为NES模拟器学习者提供与原NES机器高度相似的运行逻辑展示，源码并没有对运行速度进行优化，这也是为了保证高度还原NES运行机制的妥协。

该项目旨在重现NES游戏机的运行机制，而不注重性能，如果想要学习性能强大的模拟器，可以参照以下模拟器源码,·[fceux](https://github.com/TASEmulators/fceux)、[jsnes(js)](https://github.com/bfirsh/jsnes)、[nes(go)](https://github.com/fogleman/nes)、[SimpleNES](https://github.com/amhndu/SimpleNES)等源码，这些模拟器皆具有较好的性能，但整体代码结构并不利于NES模拟器的学习，因为其中做了大量的性能优化，导致模拟器的运行机制与原机器大相径庭，所以本仓库源码更利于理解NES。

### 控制器按键

| Nintendo              | Emulator    |
| --------------------- | ----------- |
| Up, Down, Left, Right | W A S D     |
| Start                 | T           |
| Select                | Y           |
| A                     | K           |
| B                     | J           |

### 功能键

| function        | key       |
|-----------------|-----------|
| save game state | F1        |
| load game state | F2        |
| reset game      | Backspace |
| change palette  | P         |

音频后端不可用时（无声模式），额外提供调试键：Space 暂停/继续，
C 单步一条 CPU 指令，F 单步一帧。

## 实现的模块
- BUS
- Cartridge(卡带)
- Mapper(0\1\2\3\4\66)
- 2A03(apu)
- 2C02(ppu)
- [6502(cpu)](https://github.com/tiansongyu/6502Emulator/blob/master/readme6502.md)

## 编译环境配置

### linux环境

安装cmake
``` bash 
# ubuntu
# 安装opengl、alsa
sudo apt install cmake mesa-common-dev libgl1-mesa-dev \
libglu1-mesa-dev alsa-base alsa-utils alsa-source libasound2-dev libpng-dev -y

```
### windows

安装vs2019
## 编译

### linux环境
``` bash 
git clone git@github.com:tiansongyu/6502Emulator.git 
cd 6502Emulator 
mkdir build  
cd build 
cmake  -DCMAKE_BUILD_TYPE=Release .. 
make -j12 
cd 6502/6502Emulator/
./6502Emulator [rom路径]     # 默认 ./rom/smb.nes，可指定其他 .nes 文件
```

### 回归测试（headless，无需图形环境）

模拟器核心是独立的静态库（不依赖任何 GUI），`tests/nes_headless`
就是用它写的回归工具：

``` bash
cd build/tests
./nes_headless trace    <rom> 8991        # nestest CPU 逐指令跟踪
./nes_headless frames   <rom> 300 [start帧] # 帧哈希（图形回归）
./nes_headless audio    <rom> 5 [start帧]   # 音频指标（直流/点击）
./nes_headless savetest <rom> 200 100 60    # 存档往返一致性
```

基线与验证方法见 `tests/baselines.md`。
### Windows环境
``` bash 
git clone git@github.com:tiansongyu/6502Emulator.git 
cd 6502Emulator 
make_VS2019.bat
``` 

## 可游玩游戏

每个游戏对应一个Mapper，只要实现游戏对应的Mapper，就可以游玩
本模拟器实现的mapper有
- Mapper0 (NROM)
- Mapper1 (MMC1)
- Mapper2 (UxROM)
- Mapper3 ( CNROM )
- Mapper4 (MMC3) 最复杂的一种Mapper（IRQ 改为电平触发后，metal max 等游戏的中断问题已修复）
- Mapper66 (GxROM)  

查看如下游戏列表，查看对应的Mapper，可以用来判断是否可游玩
* [NES Mapper List](http://tuxnes.sourceforge.net/nesmapper.txt)

0,1,2,3,4为任天堂自己生产的mapper，已经可以游玩大部分游戏，如想实现其他Mapper
，可以查看[Mapper资料](https://wiki.nesdev.org/w/index.php?title=Mapper),并查看本仓库源码，可以找到实现其他mapper的方法。

## 参考文档资料

* [NES Reference Guide (Wiki)](https://wiki.nesdev.org/w/index.php?title=NES_reference_guide)
* [6502官网](http://www.6502.org/)
* [6502 CPU Reference](http://www.obelisk.me.uk/6502/reference.html)
* [6502 CPU Instruction](https://www.masswerk.at/6502/6502_instruction_set.html)
* [6502 在线指令实现](https://www.masswerk.at/6502/assembler.html)
* [NES Documentation (PDF)](http://nesdev.com/NESDoc.pdf)


### 其他开发工具
* [NES Mapper List](http://tuxnes.sourceforge.net/nesmapper.txt)
* [NES文件下载](https://wowroms.com/en/roms/nintendo-entertainment-system/)

### 未实现功能

- [6502 指令实现情况](https://trello.com/b/ll6HPTJ0/6502emulator)  
- [NES模拟器功能实现情况](https://trello.com/b/hB8YJmU6/nes%E6%A8%A1%E6%8B%9F%E5%99%A8)
