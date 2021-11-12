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
| Reset                 | R           |

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

### windows

安装vs2019(安装)


## 编译
``` bash 
git clone git@github.com:tiansongyu/6502Emulator.git
cd 6502Emulator
mkdir build 
cd build 
cmake  -DCMAKE_BUILD_TYPE=Release ..
cd 6502/6502Emulator
make -j12 && ./6502/6502Emulator/6502Emulator
```

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

[6502 指令实现情况](https://trello.com/b/ll6HPTJ0/6502emulator)
[NES模拟器功能实现情况](https://trello.com/b/hB8YJmU6/nes%E6%A8%A1%E6%8B%9F%E5%99%A8)
