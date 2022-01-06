// Copyright [2020-2021] <tiansongyu>
#ifdef __GNUC__
// 关闭 警告：由于数据类型范围限制，比较结果永远为真
#pragma GCC diagnostic ignored "-Wtype-limits"
#endif

#include "olc2C02.h"
olc2C02::olc2C02() {
  // 颜色
  palScreen[0x00] = olc::Pixel(84, 84, 84);
  palScreen[0x01] = olc::Pixel(0, 30, 116);
  palScreen[0x02] = olc::Pixel(8, 16, 144);
  palScreen[0x03] = olc::Pixel(48, 0, 136);
  palScreen[0x04] = olc::Pixel(68, 0, 100);
  palScreen[0x05] = olc::Pixel(92, 0, 48);
  palScreen[0x06] = olc::Pixel(84, 4, 0);
  palScreen[0x07] = olc::Pixel(60, 24, 0);
  palScreen[0x08] = olc::Pixel(32, 42, 0);
  palScreen[0x09] = olc::Pixel(8, 58, 0);
  palScreen[0x0A] = olc::Pixel(0, 64, 0);
  palScreen[0x0B] = olc::Pixel(0, 60, 0);
  palScreen[0x0C] = olc::Pixel(0, 50, 60);
  palScreen[0x0D] = olc::Pixel(0, 0, 0);
  palScreen[0x0E] = olc::Pixel(0, 0, 0);
  palScreen[0x0F] = olc::Pixel(0, 0, 0);

  palScreen[0x10] = olc::Pixel(152, 150, 152);
  palScreen[0x11] = olc::Pixel(8, 76, 196);
  palScreen[0x12] = olc::Pixel(48, 50, 236);
  palScreen[0x13] = olc::Pixel(92, 30, 228);
  palScreen[0x14] = olc::Pixel(136, 20, 176);
  palScreen[0x15] = olc::Pixel(160, 20, 100);
  palScreen[0x16] = olc::Pixel(152, 34, 32);
  palScreen[0x17] = olc::Pixel(120, 60, 0);
  palScreen[0x18] = olc::Pixel(84, 90, 0);
  palScreen[0x19] = olc::Pixel(40, 114, 0);
  palScreen[0x1A] = olc::Pixel(8, 124, 0);
  palScreen[0x1B] = olc::Pixel(0, 118, 40);
  palScreen[0x1C] = olc::Pixel(0, 102, 120);
  palScreen[0x1D] = olc::Pixel(0, 0, 0);
  palScreen[0x1E] = olc::Pixel(0, 0, 0);
  palScreen[0x1F] = olc::Pixel(0, 0, 0);

  palScreen[0x20] = olc::Pixel(236, 238, 236);
  palScreen[0x21] = olc::Pixel(76, 154, 236);
  palScreen[0x22] = olc::Pixel(120, 124, 236);
  palScreen[0x23] = olc::Pixel(176, 98, 236);
  palScreen[0x24] = olc::Pixel(228, 84, 236);
  palScreen[0x25] = olc::Pixel(236, 88, 180);
  palScreen[0x26] = olc::Pixel(236, 106, 100);
  palScreen[0x27] = olc::Pixel(212, 136, 32);
  palScreen[0x28] = olc::Pixel(160, 170, 0);
  palScreen[0x29] = olc::Pixel(116, 196, 0);
  palScreen[0x2A] = olc::Pixel(76, 208, 32);
  palScreen[0x2B] = olc::Pixel(56, 204, 108);
  palScreen[0x2C] = olc::Pixel(56, 180, 204);
  palScreen[0x2D] = olc::Pixel(60, 60, 60);
  palScreen[0x2E] = olc::Pixel(0, 0, 0);
  palScreen[0x2F] = olc::Pixel(0, 0, 0);

  palScreen[0x30] = olc::Pixel(236, 238, 236);
  palScreen[0x31] = olc::Pixel(168, 204, 236);
  palScreen[0x32] = olc::Pixel(188, 188, 236);
  palScreen[0x33] = olc::Pixel(212, 178, 236);
  palScreen[0x34] = olc::Pixel(236, 174, 236);
  palScreen[0x35] = olc::Pixel(236, 174, 212);
  palScreen[0x36] = olc::Pixel(236, 180, 176);
  palScreen[0x37] = olc::Pixel(228, 196, 144);
  palScreen[0x38] = olc::Pixel(204, 210, 120);
  palScreen[0x39] = olc::Pixel(180, 222, 120);
  palScreen[0x3A] = olc::Pixel(168, 226, 144);
  palScreen[0x3B] = olc::Pixel(152, 226, 180);
  palScreen[0x3C] = olc::Pixel(160, 214, 228);
  palScreen[0x3D] = olc::Pixel(160, 162, 160);
  palScreen[0x3E] = olc::Pixel(0, 0, 0);
  palScreen[0x3F] = olc::Pixel(0, 0, 0);

  for (int i = 0; i < 26; i++) {
    // OAM前26个title
    sprSpriteTitle[i] = olc::Sprite(8, 8);
  }
}

olc2C02::~olc2C02() {}

olc::Sprite &olc2C02::GetScreen() {
  // 返回当前需要绘制的屏幕中的像素 256 x 240
  return sprScreen;
}

// olc::Sprite &olc2C02::GetSpriteTitle(uint8_t x, uint8_t y, uint8_t title,
//                                      uint8_t attr, uint8_t palette, int i) {
//   // 此函数用来获取右方单块的title
//   x = x;
//   uint16_t bank = title & 0x1;
//   uint16_t nOffset = (uint16_t)title * 16;

//   // 每个title是8x8像素
//   for (uint16_t row = 0; row < 8; row++) {
//     // 每一个title由8个高字节和8个低字节部分组成
//     // 两个字节的和构成一个title
//     // 所以一个title一共占用16个字节

//     // 2-Bit Pixels       LSB Bit Plane     MSB Bit Plane
//     // 0 0 0 0 0 0 0 0    0 0 0 0 0 0 0 0   0 0 0 0 0 0 0 0
//     // 0 1 1 0 0 1 1 0    0 1 1 0 0 1 1 0   0 0 0 0 0 0 0 0
//     // 0 1 2 0 0 2 1 0    0 1 1 0 0 1 1 0   0 0 1 0 0 1 0 0
//     // 0 0 0 0 0 0 0 0 =  0 0 0 0 0 0 0 0 + 0 0 0 0 0 0 0 0
//     // 0 1 1 0 0 1 1 0    0 1 1 0 0 1 1 0   0 0 0 0 0 0 0 0
//     // 0 0 1 1 1 1 0 0    0 0 1 1 1 1 0 0   0 0 0 0 0 0 0 0
//     // 0 0 0 2 2 0 0 0    0 0 0 1 1 0 0 0   0 0 0 1 1 0 0 0
//     // 0 0 0 0 0 0 0 0    0 0 0 0 0 0 0 0   0 0 0 0 0 0 0 0

//     uint8_t tile_lsb = ppuRead(bank * 0x1000 + nOffset + row + 0x0000);
//     uint8_t tile_msb = ppuRead(bank * 0x1000 + nOffset + row + 0x0008);

//     for (uint16_t col = 0; col < 8; col++) {
//       // 这里是将8x8像素放入精灵内存中
//       uint8_t pixel = ((tile_lsb & 0x01) << 1) + (tile_msb & 0x01);

//       // 通过>>=1移动一个字节来便利整个title
//       tile_lsb >>= 1;
//       tile_msb >>= 1;

//       // 现在已经获取到了整个title的像素点信息
//       // 需要将该信息转换为调色板中的信息
//       sprSpriteTitle[i].SetPixel((7 - col),  // 读取title信息时，从右面开始读取
//                                              // 对应调色板中的信息也要调换位置
//                                  row, GetColourFromPaletteRam(palette, pixel));
//     }
//   }
//   return sprSpriteTitle[i];
// }

olc::Sprite &olc2C02::GetPatternTable(uint8_t i, uint8_t palette) {
  /*此函数使用指定的调色板将给定模式表的 CHR ROM 绘制到 olc::Sprite 中。模式表由
  16x16
  的“瓷砖或字符”组成。它独立于正在运行的仿真并且使用它不会改变系统状态，尽管它从实时系统中获取它需要的所有数据。因此，如果游戏尚未建立调色板或映射到相关的
  CHR ROM 库，则精灵可能看起来是空的。这种方法允许“实时”提取模式表，确切地说是
  NES，最终玩家会看到它。] 图块由 8x8 像素组成。在 NES 上，像素是 2
  位，它给出了特定调色板的 4 种不同颜色的索引。有 8
  种调色板可供选择。每个调色板中的颜色“0”实际上被认为是透明的，因为内存中的那些位置“反映”了正在使用的全局背景颜色。这个机制在
  ppuRead() & ppuWrite() 中有详细展示 NES 使用 2
  位像素存储字符。这些不是按顺序存储的，而是存储在单个位平面中。例如：
  */
  for (uint16_t nTileY = 0; nTileY < 16; nTileY++) {
    for (uint16_t nTileX = 0; nTileX < 16; nTileX++) {
      uint16_t nOffset = nTileY * 256 + nTileX * 16;

      // Now loop through 8 rows of 8 pixels
      for (uint16_t row = 0; row < 8; row++) {
        uint8_t tile_lsb = ppuRead(i * 0x1000 + nOffset + row + 0x0000);
        uint8_t tile_msb = ppuRead(i * 0x1000 + nOffset + row + 0x0008);

        for (uint16_t col = 0; col < 8; col++) {
          uint8_t pixel = ((tile_lsb & 0x01) << 1) + (tile_msb & 0x01);

          tile_lsb >>= 1;
          tile_msb >>= 1;

          sprPatternTable[i].SetPixel(nTileX * 8 + (7 - col), nTileY * 8 + row,
                                      GetColourFromPaletteRam(palette, pixel));
        }
      }
    }
  }

  return sprPatternTable[i];
}

olc::Pixel &olc2C02::GetColourFromPaletteRam(uint8_t palette, uint8_t pixel) {
  // 输入调色板中制定位置信息，返回颜色
  // "0x3F00"       - 内存中调色板的偏移
  // "palette << 2" - 每个调色板的大小为4字节
  // "pixel"        - 每个像素索引为0、1、2或3
  // "& 0x3F"       - 阻止读取超出调色板边界的内容
  return palScreen[ppuRead(0x3F00 + (palette << 2) + pixel) & 0x3F];

  // 这里使用总线访问PPU中的内容，然后获取系统的颜色，更符合真实计算机的逻辑
  // 当然，这里也可以直接读取系统中的颜色
}

olc::Sprite &olc2C02::GetNameTable(uint8_t i) {
  // 获取名称表
  return sprNameTable[i];
}

uint8_t olc2C02::cpuRead(uint16_t addr, bool rdonly) {
  uint8_t data = 0x00;

  if (rdonly) {
    // 仅读取ppu寄存器的内容(不改变值)
    switch (addr) {
      case 0x0000:  // Control
        data = control.reg;
        break;
      case 0x0001:  // Mask
        data = mask.reg;
        break;
      case 0x0002:  // Status
        data = status.reg;
        break;
      case 0x0003:  // OAM Address
        break;
      case 0x0004:  // OAM Data
        break;
      case 0x0005:  // Scroll
        break;
      case 0x0006:  // PPU Address
        break;
      case 0x0007:  // PPU Data
        break;
    }
  } else {
    // 读取寄存器中的值，也可能会改变一些状态值
    switch (addr) {
        // Control - Not readable
      case 0x0000:
        break;

        // Mask - Not Readable
      case 0x0001:
        break;

        // Status
      case 0x0002:
        // 只有后三位存有信息
        // https://wiki.nesdev.com/w/index.php?title=PPU_programmer_reference#Status_.28.242002.29_.3C_read
        // 0xE0 = 0b11100000   0x1F = 0b00011111
        // ppu_data_buffer 之前写入 PPU 寄存器的最低有效位
        data = (status.reg & 0xE0) | (ppu_data_buffer & 0x1F);

        // Clear the vertical blanking flag
        status.vertical_blank = 0;

        // Reset Loopy's Address latch flag
        address_latch = 0;
        break;

        // OAM Address
      case 0x0003:
        break;

        // OAM Data
      case 0x0004:
        break;
        // https://wiki.nesdev.com/w/index.php?title=PPU_programmer_reference#OAM_data_.28.242004.29_.3C.3E_read.2Fwrite
        // 这个读取几乎不用
        data = pOAM[oam_addr];

        // Scroll - Not Readable
      case 0x0005:
        break;

        // PPU Address - Not Readable
      case 0x0006:
        break;

        // PPU Data
      case 0x0007:
        // https://wiki.nesdev.com/w/index.php?title=PPU_programmer_reference#Data_.28.242007.29_.3C.3E_read.2Fwrite
        // CPU 读取并获取内部缓冲区的内容后
        // PPU 会立即使用当前 VRAM 地址处的字节更新内部缓冲区
        // ppu_data_buffer
        data = ppu_data_buffer;  //???????
        // 更新缓冲区
        ppu_data_buffer = ppuRead(vram_addr.reg);  //
        // 从 $3F00-$3FFF 读取调色板数据的工作方式不同。
        // 调色板数据立即放置在数据总线上，因此不需要启动读取
        if (vram_addr.reg >= 0x3F00) data = ppu_data_buffer;  //????????
        // 根据控制寄存器中设置的模式，
        // 所有从 PPU 数据读取的数据都会自动递增命名表地址。
        // 如果设置为垂直模式，则增量为 32，因此跳过一整行名称表；
        // 在水平模式下它只是增加 1，移动到下一列
        vram_addr.reg += (control.increment_mode ? 32 : 1);
        break;
    }
  }

  return data;
}

void olc2C02::cpuWrite(uint16_t addr, uint8_t data) {
  switch (addr) {
    case 0x0000:  // Control
      control.reg = data;
      // 临时存储要在不同时间“转移”到“指针”中的信息
      tram_addr.nametable_x = control.nametable_x;
      tram_addr.nametable_y = control.nametable_y;
      break;
    case 0x0001:  // Mask
      mask.reg = data;
      break;
    case 0x0002:  // Status
      break;
    case 0x0003:  // OAM Address
      oam_addr = data;
      break;
    case 0x0004:  // OAM Data
      pOAM[oam_addr] = data;
      break;
    case 0x0005:  // Scroll
      // 滚动功能实现寄存器
      // 通过address_latch 判断是水平滚动还是垂直滚动
      // 这个scroll执行两次，先执行x 后执行 y
      if (address_latch == 0) {
        // 首次写入滚动寄存器包含像素空间中的X偏移量
        // 我们将其分为粗x值和细x值
        // 细x值保存的是像素x偏移
        // 粗x值保存名称表的偏移
        fine_x = data & 0x07;
        tram_addr.coarse_x = data >> 3;
        address_latch = 1;
      } else {
        // 首次写入滚动寄存器包含像素空间中的Y偏移量
        // 我们将其分为粗y值和细y值
        // 细y值保存的是像素y偏移
        // 粗y值保存名称表的偏移
        tram_addr.fine_y = data & 0x07;
        tram_addr.coarse_y = data >> 3;
        address_latch = 0;
      }
      break;
    case 0x0006:  // PPU Address
                  // ppu地址是一个16字节地址，所以分两次传输
      // 第一次传输高8字节
      // 第二次传输底8字节
      // 两个周期
      if (address_latch == 0) {
        // PPU address bus can be accessed by CPU via the ADDR and DATA
        // registers. The fisrt write to this register latches the high byte
        // of the address, the second is the low byte. Note the writes
        // are stored in the tram register...
        // tram_addr.reg = (uint16_t)((data & 0x3F) << 8) | (tram_addr.reg &
        // 0x00FF);

        // CPU可通过ADDR和DATA访问PPU地址总线寄存器
        // 第一次写入该寄存器将锁存高字节
        // 在地址中，第二个是低字节。
        // tram_addr.reg=（uint16_t）（（data&0x3F）<<8）|（tram_addr.reg&0x00FF）；
        tram_addr.reg = (uint16_t)((data & 0x3F) << 8);  // fix something
        address_latch = 1;
      } else {
        // 写入整个地址后，内部vram地址
        // 缓冲区已更新。在渲染期间不能写入PPU
        // 因为PPU将自动维护vram地址，同时更新
        // 渲染扫描线位置。
        tram_addr.reg = (tram_addr.reg & 0xFF00) | data;
        vram_addr = tram_addr;
        address_latch = 0;
      }
      break;
    case 0x0007:  // PPU Data
      ppuWrite(vram_addr.reg, data);
      //  根据控制寄存器中设置的模式，所有从 PPU
      //  数据写入的数据都会自动递增命名表地址。
      // 如果设置为垂直模式，则增量为 32，
      // 因此跳过一整行名称表；
      // 在水平模式下它只是增加 1，移动到下一列
      vram_addr.reg += (control.increment_mode ? 32 : 1);
      break;
  }
}

uint8_t olc2C02::ppuRead(uint16_t addr, bool rdonly) {
  rdonly = rdonly;
  uint8_t data = 0x00;
  addr &= 0x3FFF;

  if (cart->ppuRead(addr, data)) {
  } else if (addr >= 0x0000 && addr <= 0x1FFF) {
    // 如果盒带无法映射地址，请使用
    // 这里有一个实际位置????
    data = tblPattern[(addr & 0x1000) >> 12][addr & 0x0FFF];
  } else if (addr >= 0x2000 && addr <= 0x3EFF) {
    addr &= 0x0FFF;

    if (cart->Mirror() == MIRROR::VERTICAL) {
      // Vertical
      if (addr >= 0x0000 && addr <= 0x03FF) data = tblName[0][addr & 0x03FF];
      if (addr >= 0x0400 && addr <= 0x07FF) data = tblName[1][addr & 0x03FF];
      if (addr >= 0x0800 && addr <= 0x0BFF) data = tblName[0][addr & 0x03FF];
      if (addr >= 0x0C00 && addr <= 0x0FFF) data = tblName[1][addr & 0x03FF];
    } else if (cart->Mirror() == MIRROR::HORIZONTAL) {
      // Horizontal
      if (addr >= 0x0000 && addr <= 0x03FF) data = tblName[0][addr & 0x03FF];
      if (addr >= 0x0400 && addr <= 0x07FF) data = tblName[0][addr & 0x03FF];
      if (addr >= 0x0800 && addr <= 0x0BFF) data = tblName[1][addr & 0x03FF];
      if (addr >= 0x0C00 && addr <= 0x0FFF) data = tblName[1][addr & 0x03FF];
    }
  } else if (addr >= 0x3F00 && addr <= 0x3FFF) {
    addr &= 0x001F;
    if (addr == 0x0010) addr = 0x0000;
    if (addr == 0x0014) addr = 0x0004;
    if (addr == 0x0018) addr = 0x0008;
    if (addr == 0x001C) addr = 0x000C;
    data = tblPalette[addr] & (mask.grayscale ? 0x30 : 0x3F);
  }

  return data;
}

void olc2C02::ppuWrite(uint16_t addr, uint8_t data) {
  addr &= 0x3FFF;

  if (cart->ppuWrite(addr, data)) {
  } else if (addr >= 0x0000 && addr <= 0x1FFF) {
    tblPattern[(addr & 0x1000) >> 12][addr & 0x0FFF] = data;
  } else if (addr >= 0x2000 && addr <= 0x3EFF) {
    addr &= 0x0FFF;
    if (cart->Mirror() == MIRROR::VERTICAL) {
      // Vertical
      if (addr >= 0x0000 && addr <= 0x03FF) tblName[0][addr & 0x03FF] = data;
      if (addr >= 0x0400 && addr <= 0x07FF) tblName[1][addr & 0x03FF] = data;
      if (addr >= 0x0800 && addr <= 0x0BFF) tblName[0][addr & 0x03FF] = data;
      if (addr >= 0x0C00 && addr <= 0x0FFF) tblName[1][addr & 0x03FF] = data;
    } else if (cart->Mirror() == MIRROR::HORIZONTAL) {
      // Horizontal
      if (addr >= 0x0000 && addr <= 0x03FF) tblName[0][addr & 0x03FF] = data;
      if (addr >= 0x0400 && addr <= 0x07FF) tblName[0][addr & 0x03FF] = data;
      if (addr >= 0x0800 && addr <= 0x0BFF) tblName[1][addr & 0x03FF] = data;
      if (addr >= 0x0C00 && addr <= 0x0FFF) tblName[1][addr & 0x03FF] = data;
    }
  } else if (addr >= 0x3F00 && addr <= 0x3FFF) {
    addr &= 0x001F;
    if (addr == 0x0010) addr = 0x0000;
    if (addr == 0x0014) addr = 0x0004;
    if (addr == 0x0018) addr = 0x0008;
    if (addr == 0x001C) addr = 0x000C;
    tblPalette[addr] = data;
  }
}

void olc2C02::ConnectCartridge(const std::shared_ptr<Cartridge> &cartridge) {
  this->cart = cartridge;
}

void olc2C02::reset() {
  fine_x = 0x00;
  address_latch = 0x00;
  ppu_data_buffer = 0x00;
  scanline = 0;
  cycle = 0;
  bg_next_tile_id = 0x00;
  bg_next_tile_attrib = 0x00;
  bg_next_tile_lsb = 0x00;
  bg_next_tile_msb = 0x00;
  bg_shifter_pattern_lo = 0x0000;
  bg_shifter_pattern_hi = 0x0000;
  bg_shifter_attrib_lo = 0x0000;
  bg_shifter_attrib_hi = 0x0000;
  status.reg = 0x00;
  mask.reg = 0x00;
  control.reg = 0x00;
  vram_addr.reg = 0x0000;
  tram_addr.reg = 0x0000;
  odd_frame = false;
}

void olc2C02::clock() {
  // 这里做了大部分内容
  // ppu相当于一个状态机
  // As we progress through scanlines and cycles, the PPU is effectively
  // a state machine going through the motions of fetching background
  // information and sprite information, compositing them into a pixel
  // to be output.

  // The lambda functions (functions inside functions) contain the various
  // actions to be performed depending upon the output of the state machine
  // for a given scanline/cycle combination

  // ==============================================================================
  // Increment the background tile "pointer" one tile/column horizontally

  // 当我们在扫描线和循环中前进时，PPU有效地
  // 通过获取背景的动作的状态机
  // 信息和精灵信息，将它们合成为像素
  // 被输出。
  // lambda函数（函数中的函数）包含各种
  // 根据状态机的输出执行的操作
  // 对于给定的扫描线/循环组合
  // ==============================================================================
  // 将背景平铺“指针”水平增加一个平铺/列
  auto IncrementScrollX = [&]() {
    // Note: pixel perfect scrolling horizontally is handled by the  data
    // shifters. Here we are operating in the spatial domain of   tiles, 8x8
    // pixel blocks. 注：像素完美水平滚动由数据移位器处理。
    // 在这里，我们在瓦片的空间域中操作，即8x8像素块。
    // 如果绘制
    if (mask.render_background || mask.render_sprites) {
      // A single name table is 32x30 tiles. As we increment horizontally
      // we may cross into a neighbouring nametable, or wrap around to
      // a neighbouring nametable
      // 单个名称表为32x30平铺。当我们水平增加时
      // 我们可能会跨进相邻的一张姓名表，或者绕到另一张姓名表上
      if (vram_addr.coarse_x == 31) {
        // Leaving nametable so wrap address round
        // 离开nametable，将地址环绕
        vram_addr.coarse_x = 0;
        // Flip target nametable bit
        // 翻转目标命名表位
        vram_addr.nametable_x = ~vram_addr.nametable_x;
      } else {
        // 依然在当前名称表中，所以只需增加coarse_x
        vram_addr.coarse_x++;
      }
    }
  };

  // ==============================================================================
  // Increment the background tile "pointer" one scanline vertically
  auto IncrementScrollY = [&]() {
    // Incrementing vertically is more complicated. The visible nametable
    // is 32x30 tiles, but in memory there is enough room for 32x32 tiles.
    // The bottom two rows of tiles are in fact not tiles at all, they
    // contain the "attribute" information for the entire table. This is
    // information that describes which palettes are used for different
    // regions of the nametable.

    // In addition, the NES doesnt scroll vertically in chunks of 8 pixels
    // i.e. the height of a tile, it can perform fine scrolling by using
    // the fine_y component of the register. This means an increment in Y
    // first adjusts the fine offset, but may need to adjust the whole
    // row offset, since fine_y is a value 0 to 7, and a row is 8 pixels high

    // 垂直递增更为复杂。可见名称表
    // 是32x30瓷砖，但内存中有足够的空间容纳32x32瓷砖。
    // 底部的两排瓷砖实际上根本不是瓷砖，它们
    // 包含整个表的“属性”信息。这是
    // 描述用于不同应用程序的选项板的信息
    // 名称表的区域。
    // 此外，网元不会以8像素的块垂直滚动
    // 即瓷砖的高度，它可以通过使用
    // 寄存器的精细部分。这意味着Y的增量
    // 首先调整精细偏移，但可能需要调整整个偏移
    // 行偏移，因为fine_y是一个0到7的值，一行高8像素
    if (mask.render_background || mask.render_sprites) {
      // If possible, just increment the fine y offset
      if (vram_addr.fine_y < 7) {
        vram_addr.fine_y++;
      } else {
        // If we have gone beyond the height of a row, we need to
        // increment the row, potentially wrapping into neighbouring
        // vertical nametables. Dont forget however, the bottom two rows
        // do not contain tile information. The coarse y offset is used
        // to identify which row of the nametable we want, and the fine
        // y offset is the specific "scanline"

        // 如果我们已经超过了一排的高度，我们需要
        // 增加行数，可能会换行到相邻行中
        // 垂直名称表。不过，别忘了下面两排
        // 不包含平铺信息。使用粗略的y偏移
        // 确定我们想要的名称表的哪一行，以及
        // y偏移是特定的“扫描线”

        // find_y是title内的像素y坐标扫描线
        vram_addr.fine_y = 0;

        // coarse_y记录的是名称表内的y扫描线
        if (vram_addr.coarse_y == 29) {
          // We do, so reset coarse y offset
          vram_addr.coarse_y = 0;
          // And flip the target nametable bit
          vram_addr.nametable_y = ~vram_addr.nametable_y;
        } else if (vram_addr.coarse_y == 31) {
          // 进入名称表的属性范围边界
          // In case the pointer is in the attribute memory, we
          // just wrap around the current nametable
          vram_addr.coarse_y = 0;
        } else {
          // None of the above boundary/wrapping conditions apply
          // so just increment the coarse y offset
          vram_addr.coarse_y++;
        }
      }
    }
  };

  // ==============================================================================
  // Transfer the temporarily stored horizontal nametable access information
  // into the "pointer". Note that fine x scrolling is not part of the "pointer"
  // addressing mechanism
  // 传输临时存储的水平nametable访问信息
  // 进入“指针”。请注意，精细x滚动不是“指针”的一部分
  // 寻址机制
  auto TransferAddressX = [&]() {
    // Ony if rendering is enabled
    if (mask.render_background || mask.render_sprites) {
      vram_addr.nametable_x = tram_addr.nametable_x;
      vram_addr.coarse_x = tram_addr.coarse_x;
    }
  };

  // ==============================================================================
  // Transfer the temporarily stored vertical nametable access information
  // into the "pointer". Note that fine y scrolling is part of the "pointer"
  // addressing mechanism
  // 传输临时存储的垂直名称表访问信息
  // 进入“指针”。请注意，精细的y形滚动是“指针”的一部分
  // 寻址机制
  auto TransferAddressY = [&]() {
    // Ony if rendering is enabled
    if (mask.render_background || mask.render_sprites) {
      vram_addr.fine_y = tram_addr.fine_y;
      vram_addr.nametable_y = tram_addr.nametable_y;
      vram_addr.coarse_y = tram_addr.coarse_y;
    }
  };

  // ==============================================================================
  // Prime the "in-effect" background tile shifters ready for outputting next
  // 8 pixels in scanline.

  // 为“有效”背景平铺移位器加上底漆，以便下一步输出
  // 扫描线为8像素。
  auto LoadBackgroundShifters = [&]() {
    // Each PPU update we calculate one pixel. These shifters shift 1 bit along
    // feeding the pixel compositor with the binary information it needs. Its
    // 16 bits wide, because the top 8 bits are the current 8 pixels being drawn
    // and the bottom 8 bits are the next 8 pixels to be drawn. Naturally this
    // means the required bit is always the MSB of the shifter. However, "fine
    // x" scrolling plays a part in this too, whcih is seen later, so in fact we
    // can choose any one of the top 8 bits.

    // 每次PPU更新我们计算一个像素。这些移位器沿方向移位1位
    // 向像素合成器提供所需的二进制信息。它的
    // 16位宽，因为前8位是当前正在绘制的8个像素
    // 底部的8位是接下来要绘制的8个像素。当然这意味着
    // 所需位始终是移位器的MSB。但是，“精细x”滚动
    // 这也起了一定作用，后面会看到，所以事实上我们可以选择
    // 前8位中的任意一位。

    bg_shifter_pattern_lo = (bg_shifter_pattern_lo & 0xFF00) | bg_next_tile_lsb;
    bg_shifter_pattern_hi = (bg_shifter_pattern_hi & 0xFF00) | bg_next_tile_msb;

    // Attribute bits do not change per pixel, rather they change every 8 pixels
    // but are synchronised with the pattern shifters for convenience, so here
    // we take the bottom 2 bits of the attribute word which represent which
    // palette is being used for the current 8 pixels and the next 8 pixels, and
    // "inflate" them to 8 bit words.
    bg_shifter_attrib_lo = (bg_shifter_attrib_lo & 0xFF00) |
                           ((bg_next_tile_attrib & 0b01) ? 0xFF : 0x00);
    bg_shifter_attrib_hi = (bg_shifter_attrib_hi & 0xFF00) |
                           ((bg_next_tile_attrib & 0b10) ? 0xFF : 0x00);
  };

  // ==============================================================================
  // Every cycle the shifters storing pattern and attribute information shift
  // their contents by 1 bit. This is because every cycle, the output progresses
  // by 1 pixel. This means relatively, the state of the shifter is in sync
  // with the pixels being drawn for that 8 pixel section of the scanline.

  // 每个周期，存储模式和属性信息的移位器移位
  // 它们的内容是1位的。这是因为每一个周期，输出都在进行
  // 1像素。这意味着换档杆的状态相对而言是同步的
  // 为扫描线的8像素部分绘制像素。

  auto UpdateShifters = [&]() {
    if (mask.render_background) {
      // Shifting background tile pattern row
      bg_shifter_pattern_lo <<= 1;
      bg_shifter_pattern_hi <<= 1;

      // Shifting palette attributes by 1
      bg_shifter_attrib_lo <<= 1;
      bg_shifter_attrib_hi <<= 1;
    }
    if (mask.render_sprites && cycle >= 1 && cycle < 258) {
      for (int i = 0; i < sprite_count; i++) {
        if (spriteScanline[i].x > 0) {
          spriteScanline[i].x--;
        } else {
          sprite_shifter_pattern_lo[i] <<= 1;
          sprite_shifter_pattern_hi[i] <<= 1;
        }
      }
    }
  };

  // All but 1 of the secanlines is visible to the user. The pre-render scanline
  // at -1, is used to configure the "shifters" for the first visible scanline,
  // 0. 初始化的scanline的值为 0 但是当帧数大于 1 时 scanline 从 -1 开始
  if (scanline >= -1 && scanline < 240) {
    // 这里的scanline 指的是行数 ，渲染的行数为 0 - 239
    // cycle 指的是 Pixel 列数
    if (scanline == 0 && cycle == 0 && odd_frame &&
        (mask.render_background || mask.render_sprites)) {
      // "Odd Frame" cycle skip
      // 第一cycle时， 直接跳过
      cycle = 1;
    }
    // 当 scanline == -1 && cycle == 1 说明是一个新的frame
    if (scanline == -1 && cycle == 1) {
      // 初始化数据
      // Effectively start of new frame, so clear vertical blank flag
      status.vertical_blank = 0;

      // Clear sprite overflow flag
      status.sprite_overflow = 0;

      // Clear the sprite zero hit flag
      status.sprite_zero_hit = 0;

      // Clear Shifters
      // Shifters 移位器
      // 一行最多有 8 个精灵
      // 每个精灵的 x 值最大 为 256
      // 每个精灵的 y 值最大 为 240
      for (int i = 0; i < 8; i++) {
        sprite_shifter_pattern_lo[i] = 0;
        sprite_shifter_pattern_hi[i] = 0;
      }
    }

    if ((cycle >= 2 && cycle < 258) || (cycle >= 321 && cycle < 338)) {
      UpdateShifters();

      // In these cycles we are collecting and working with visible data
      // The "shifters" have been preloaded by the end of the previous
      // scanline with the data for the start of this scanline. Once we
      // leave the visible region, we go dormant until the shifters are
      // preloaded for the next scanline.

      // Fortunately, for background rendering, we go through a fairly
      // repeatable sequence of events, every 2 clock cycles.

      // 在这些周期中，我们收集和处理可见数据
      // “换档杆”已在上一节结束时预加载
      // 带有此扫描线起点数据的扫描线。一旦我们
      // 离开可见区域，我们进入休眠状态，直到移位器被移除
      // 为下一条扫描线预加载。
      // 幸运的是，对于背景渲染，我们经历了一个相当复杂的过程
      // 可重复的事件序列，每2个时钟周期。

      switch ((cycle - 1) % 8) {
        case 0:
          // Load the current background tile pattern and attributes into the
          // "shifter" 这个是背景，所有的x坐标开始都是 8 的整数倍
          LoadBackgroundShifters();

          // Fetch the next background tile ID
          // "(vram_addr.reg & 0x0FFF)" : Mask to 12 bits that are relevant
          // "| 0x2000"                 : Offset into nametable space on PPU
          // address bus 获取下一个背景title的ID号(名称表中)
          bg_next_tile_id = ppuRead(0x2000 | (vram_addr.reg & 0x0FFF));

          // Explanation:
          // The bottom 12 bits of the loopy register provide an index into
          // the 4 nametables, regardless of nametable mirroring configuration.
          // nametable_y(1) nametable_x(1) coarse_y(5) coarse_x(5)
          //
          // Consider a single nametable is a 32x32 array, and we have four of
          // them

          // 说明：
          // 循环寄存器的底部12位提供到的索引
          // 这4个名称表，与名称表镜像配置无关。
          // 名称表_y（1）名称表_x（1）粗略_y（5）粗略_x（5）
          //
          // 考虑单个可指定的是32×32数组，并且我们有四个数组。

          //   0                1
          // 0 +----------------+----------------+
          //   |                |                |
          //   |                |                |
          //   |    (32x32)     |    (32x32)     |
          //   |                |                |
          //   |                |                |
          // 1 +----------------+----------------+
          //   |                |                |
          //   |                |                |
          //   |    (32x32)     |    (32x32)     |
          //   |                |                |
          //   |                |                |
          //   +----------------+----------------+
          //
          // This means there are 4096 potential locations in this array, which
          // just so happens to be 2^12!

          // 这意味着此阵列中有4096个潜在位置
          // 正好是2^12！
          break;
        case 2:
          // Fetch the next background tile attribute. OK, so this one is a bit
          // more involved :P

          // Recall that each nametable has two rows of cells that are not tile
          // information, instead they represent the attribute information that
          // indicates which palettes are applied to which area on the screen.
          // Importantly (and frustratingly) there is not a 1 to 1
          // correspondance between background tile and palette. Two rows of
          // tile data holds 64 attributes. Therfore we can assume that the
          // attributes affect 8x8 zones on the screen for that nametable. Given
          // a working resolution of 256x240, we can further assume that each
          // zone is 32x32 pixels in screen space, or 4x4 tiles. Four system
          // palettes are allocated to background rendering, so a palette can be
          // specified using just 2 bits. The attribute byte therefore can
          // specify 4 distinct palettes. Therefore we can even further assume
          // that a single palette is applied to a 2x2 tile combination of the
          // 4x4 tile zone. The very fact that background tiles "share" a
          // palette locally is the reason why in some games you see distortion
          // in the colours at screen edges.

          // As before when choosing the tile ID, we can use the bottom 12 bits
          // of the loopy register, but we need to make the implementation
          // "coarser" because instead of a specific tile, we want the attribute
          // byte for a group of 4x4 tiles, or in other words, we divide our
          // 32x32 address by 4 to give us an equivalent 8x8 address, and we
          // offset this address into the attribute section of the target
          // nametable.

          // Reconstruct the 12 bit loopy address into an offset into the
          // attribute memory

          // 获取下一个背景平铺属性。好的，所以这个有点
          // 更多参与：P
          // 回想一下，每个nametable都有两行不是平铺的单元格
          // 信息，而是表示
          // 指示将哪些选项板应用于屏幕上的哪个区域。
          // 重要的是（令人沮丧的是）没有1:1的对应关系
          // 在背景平铺和调色板之间。两行磁贴数据保持不变
          // 64个属性。因此，我们可以假设属性会影响
          // 屏幕上该名称表的8x8区域。给出了一个工作决议
          // 对于256x240，我们可以进一步假设每个区域为32x32像素
          // 在屏幕空间或4x4平铺中。分配了四个系统选项板
          // 要进行背景渲染，只需使用
          // 2位。因此，属性字节可以指定4个不同的选项板。
          // 因此，我们甚至可以进一步假设一个调色板是
          // 应用于4x4瓷砖区域的2x2瓷砖组合。事实本身
          // 背景瓷砖在本地“共享”调色板就是原因
          // 在一些游戏中，你会看到屏幕边缘颜色的失真。
          // 与前面一样，在选择磁贴ID时，我们可以使用
          // 循环寄存器，但我们需要使实现“更粗糙”
          // 因为我们需要的是属性字节，而不是特定的磁贴
          // 一组4x4瓷砖，或者换句话说，我们划分32x32地址
          // 乘以4，得到一个等效的8x8地址，我们将该地址偏移
          // 进入目标名称表的属性部分。
          // 将12位循环地址重新构造为
          // 属性存储器

          // "(vram_addr.coarse_x >> 2)"        : integer divide coarse x by 4,
          //                                      from 5 bits to 3 bits
          // "((vram_addr.coarse_y >> 2) << 3)" : integer divide coarse y by 4,
          //                                      from 5 bits to 3 bits,
          //                                      shift to make room for coarse
          //                                      x

          // Result so far: YX00 00yy yxxx

          // All attribute memory begins at 0x03C0 within a nametable, so OR
          // with result to select target nametable, and attribute byte offset.
          // Finally OR with 0x2000 to offset into nametable address space on
          // PPU bus.

          // 名称表中的所有属性内存从0x03C0开始，so或with
          // 结果以选择目标名称表和属性字节偏移量。最后
          // 或使用0x2000偏移到PPU总线上的nametable地址空间。
          bg_next_tile_attrib = ppuRead(0x23C0 | (vram_addr.nametable_y << 11) |
                                        (vram_addr.nametable_x << 10) |
                                        ((vram_addr.coarse_y >> 2) << 3) |
                                        (vram_addr.coarse_x >> 2));

          // Right we've read the correct attribute byte for a specified
          // address, but the byte itself is broken down further into the 2x2
          // tile groups in the 4x4 attribute zone.

          // The attribute byte is assembled thus: BR(76) BL(54) TR(32) TL(10)

          // 对，我们已经读取了指定地址的正确属性字节，
          // 但是字节本身被进一步分解为2x2平铺组
          // 在4x4属性区域中。
          // 属性字节是这样组合的：BR（76）BL（54）TR（32）TL（10）
          //
          // +----+----+                +----+----+
          // | TL | TR |                | ID | ID |
          // +----+----+ where TL =   +----+----+
          // | BL | BR |                | ID | ID |
          // +----+----+                +----+----+
          //
          // Since we know we can access a tile directly from the 12 bit
          // address, we can analyse the bottom bits of the coarse coordinates
          // to provide us with the correct offset into the 8-bit word, to yield
          // the 2 bits we are actually interested in which specifies the
          // palette for the 2x2 group of tiles. We know if "coarse y % 4" < 2
          // we are in the top half else bottom half. Likewise if "coarse x % 4"
          // < 2 we are in the left half else right half. Ultimately we want the
          // bottom two bits of our attribute word to be the palette selected.
          // So shift as required...

          // 因为我们知道可以直接从12位地址访问磁贴，所以我们
          // 可以分析粗坐标的底部位，为我们提供
          // 将正确的偏移量转换为8位字，以产生所需的2位
          // 实际感兴趣的是指定2x2组
          // 瓷砖。我们知道如果“粗y%4”<2，我们在上半部分，否则在下半部分。
          // 同样，如果“粗略x%4”小于2，则我们位于左半部分，否则位于右半部分。
          // 最终，我们希望属性词的底部两位是
          // 选择调色板。所以按要求换班。。。
          if (vram_addr.coarse_y & 0x02) bg_next_tile_attrib >>= 4;
          if (vram_addr.coarse_x & 0x02) bg_next_tile_attrib >>= 2;
          bg_next_tile_attrib &= 0x03;
          break;

          // Compared to the last two, the next two are the easy ones... :P

        case 4:
          // Fetch the next background tile LSB bit plane from the pattern
          // memory The Tile ID has been read from the nametable. We will use
          // this id to index into the pattern memory to find the correct sprite
          // (assuming the sprites lie on 8x8 pixel boundaries in that memory,
          // which they do even though 8x16 sprites exist, as background tiles
          // are always 8x8).
          //
          // Since the sprites are effectively 1 bit deep, but 8 pixels wide, we
          // can represent a whole sprite row as a single byte, so offsetting
          // into the pattern memory is easy. In total there is 8KB so we need a
          // 13 bit address.

          // 从模式内存中获取下一个背景平铺LSB位平面
          // 已从名称表中读取磁贴ID。我们将使用此id
          // 索引到模式内存中以找到正确的精灵（假设
          // 精灵位于内存中的8x8像素边界上，它们就是这样做的
          // 即使存在8x16精灵，因为背景平铺始终为8x8）。
          //
          // 由于精灵实际上是1位深，但8像素宽，我们
          // 可以将整个sprite行表示为单个字节，因此偏移
          // 进入模式记忆很容易。总共有8KB，所以我们需要一个
          // 13位地址。

          // "(control.pattern_background << 12)"  : the pattern memory selector
          //                                         from control register,
          //                                         either 0K or 4K offset
          // "((uint16_t)bg_next_tile_id << 4)"    : the tile id multiplied by
          // 16, as
          //                                         2 lots of 8 rows of 8 bit
          //                                         pixels
          // "(vram_addr.fine_y)"                  : Offset into which row based
          // on
          //                                         vertical scroll offset
          // "+ 0"                                 : Mental clarity for plane
          // offset Note: No PPU address bus offset required as it starts at
          // 0x0000
          bg_next_tile_lsb = ppuRead((control.pattern_background << 12) +
                                     ((uint16_t)bg_next_tile_id << 4) +
                                     (vram_addr.fine_y) + 0);

          break;
        case 6:
          // Fetch the next background tile MSB bit plane from the pattern
          // memory This is the same as above, but has a +8 offset to select the
          // next bit plane
          bg_next_tile_msb = ppuRead((control.pattern_background << 12) +
                                     ((uint16_t)bg_next_tile_id << 4) +
                                     (vram_addr.fine_y) + 8);
          break;
        case 7:
          // Increment the background tile "pointer" to the next tile
          // horizontally in the nametable memory. Note this may cross nametable
          // boundaries which is a little complex, but essential to implement
          // scrolling
          IncrementScrollX();
          break;
      }
    }

    // End of a visible scanline, so increment downwards...
    if (cycle == 256) {
      IncrementScrollY();
    }

    // ...and reset the x position
    if (cycle == 257) {
      // 非渲染时间 准备下一次的坐标
      LoadBackgroundShifters();
      TransferAddressX();
    }

    // Superfluous reads of tile id at end of scanline
    if (cycle == 338 || cycle == 340) {
      // 0x2000 - 0x2FFF
      bg_next_tile_id = ppuRead(0x2000 | (vram_addr.reg & 0x0FFF));
    }

    if (scanline == -1 && cycle >= 280 && cycle < 305) {
      // 开始新的一个frame,重新设定新的nametable的起始位置
      // End of vertical blank period so reset the Y address ready for rendering
      TransferAddressY();
    }
  }

  // Foreground Rendering
  // ======================================================== I'm gonna cheat a
  // bit here, which may reduce compatibility, but greatly simplifies delivering
  // an intuitive understanding of what exactly is going on. The PPU loads
  // sprite information successively during the region that background tiles are
  // not being drawn. Instead, I'm going to perform all sprite evaluation in one
  // hit. THE NES DOES NOT DO IT LIKE THIS! This makes it easier to see the
  // process of sprite evaluation.
  if (cycle == 257 && scanline >= 0) {
    // We've reached the end of a visible scanline. It is now time to determine
    // which sprites are visible on the next scanline, and preload this info
    // into buffers that we can work with while the scanline scans the row.

    // Firstly, clear out the sprite memory. This memory is used to store the
    // sprites to be rendered. It is not the OAM.

    // 我们已经到达一条可见扫描线的末端。现在是决定的时候了
    // 哪些精灵在下一条扫描线上可见，并预加载此信息
    // 我们可以在扫描线扫描行时使用缓冲区。
    // 首先，清除精灵记忆。此内存用于存储
    // 要渲染的精灵。这不是奥姆。

    std::memset(spriteScanline, 0xFF, 8 * sizeof(sObjectAttributeEntry));

    // The NES supports a maximum number of sprites per scanline. Nominally
    // this is 8 or fewer sprites. This is why in some games you see sprites
    // flicker or disappear when the scene gets busy.
    sprite_count = 0;

    // Secondly, clear out any residual information in sprite pattern shifters
    for (uint8_t i = 0; i < 8; i++) {
      sprite_shifter_pattern_lo[i] = 0;
      sprite_shifter_pattern_hi[i] = 0;
    }

    // Thirdly, Evaluate which sprites are visible in the next scanline. We need
    // to iterate through the OAM until we have found 8 sprites that have
    // Y-positions and heights that are within vertical range of the next
    // scanline. Once we have found 8 or exhausted the OAM we stop. Now, notice
    // I count to 9 sprites. This is so I can set the sprite overflow flag in
    // the event of there being > 8 sprites.
    uint8_t nOAMEntry = 0;

    // New set of sprites. Sprite zero may not exist in the new set, so clear
    // this flag.
    bSpriteZeroHitPossible = false;
    // 在每一行中的257列的周期中，将下一行需要绘制的精灵进行预写入内存中
    while (nOAMEntry < 64 && sprite_count < 9) {
      // Note the conversion to signed numbers here
      int16_t diff = ((int16_t)scanline - (int16_t)OAM[nOAMEntry].y);
      // If the difference is positive then the scanline is at least at the
      // same height as the sprite, so check if it resides in the sprite
      // vertically depending on the current "sprite height mode" FLAGGED

      if (diff >= 0 && diff < (control.sprite_size ? 16 : 8)) {
        // Sprite is visible, so copy the attribute entry over to our
        // scanline sprite cache. Ive added < 8 here to guard the array
        // being written to.
        if (sprite_count < 8) {
          // Is this sprite sprite zero?
          if (nOAMEntry == 0) {
            // It is, so its possible it may trigger a
            // sprite zero hit when drawn
            bSpriteZeroHitPossible = true;
          }

          memcpy(&spriteScanline[sprite_count], &OAM[nOAMEntry],
                 sizeof(sObjectAttributeEntry));
          sprite_count++;
        }
      }

      nOAMEntry++;
    }  // End of sprite evaluation for next scanline

    // Set sprite overflow flag
    status.sprite_overflow = (sprite_count > 8);

    // Now we have an array of the 8 visible sprites for the next scanline. By
    // the nature of this search, they are also ranked in priority, because
    // those lower down in the OAM have the higher priority.

    // We also guarantee that "Sprite Zero" will exist in spriteScanline[0] if
    // it is evaluated to be visible.
  }

  if (cycle == 340) {
    // Now we're at the very end of the scanline, I'm going to prepare the
    // sprite shifters with the 8 or less selected sprites.
    // 此时已经准备好了需要绘制的下一行的精灵信息，但是没有将精灵信息与模式表中的像素进行匹配
    // 接下来要将精灵信息转换为模式表中的像素信息
    for (uint8_t i = 0; i < sprite_count; i++) {
      // We need to extract the 8-bit row patterns of the sprite with the
      // correct vertical offset. The "Sprite Mode" also affects this as
      // the sprites may be 8 or 16 rows high. Additionally, the sprite
      // can be flipped both vertically and horizontally. So there's a lot
      // going on here :P

      uint8_t sprite_pattern_bits_lo, sprite_pattern_bits_hi;
      uint16_t sprite_pattern_addr_lo, sprite_pattern_addr_hi;

      // Determine the memory addresses that contain the byte of pattern data.
      // We only need the lo pattern address, because the hi pattern address is
      // always offset by 8 from the lo address.
      if (!control.sprite_size) {
        // 8x8 Sprite Mode - The control register determines the pattern table
        // 是否垂直翻转精灵
        if (!(spriteScanline[i].attribute & 0x80)) {
          // Sprite is NOT flipped vertically, i.e. normal
          // 这个地址直接就是PPU地址，没什么其他含义，不是一个结构体
          // 0x0000 - 0x1fff
          // sprite_pattern_addr_lo 中存储的值就是上方的值 每个瓦片 16 个字节，
          sprite_pattern_addr_lo =
              // 用来判断是在 0x0000 还是 0x1000
              (control.pattern_sprite
               << 12)  // Which Pattern Table? 0KB or 4KB offset
              // 每个瓦片是16个字节， <<4是为了乘以16
              | (spriteScanline[i].id
                 << 4)  // Which Cell? Tile ID * 16 (16 bytes per tile)
              // 获得该精灵像素点在8x1bit中的 y 像素位置
              // 乘以16之后，已经获得了哪一个瓦片title，但是还不是知道是现在哪一列
              // 加上 y 之后，可以获得需要获取的点，在瓦片的哪一排上
              | (scanline - spriteScanline[i].y);  // Which Row in cell? (0->7)
        } else {
          // Sprite is flipped vertically, i.e. upside down
          sprite_pattern_addr_lo =
              (control.pattern_sprite
               << 12)  // Which Pattern Table? 0KB or 4KB offset
              | (spriteScanline[i].id
                 << 4)  // Which Cell? Tile ID * 16 (16 bytes per tile)
              // 翻转精灵
              | (7 - (scanline -
                      spriteScanline[i].y));  // Which Row in cell? (7->0)
        }
      } else {
        // 8x16 Sprite Mode - The sprite attribute determines the pattern table
        if (!(spriteScanline[i].attribute & 0x80)) {
          // Sprite is NOT flipped vertically, i.e. normal
          // 8x16的像素块分为上下两块
          // 判断为上面的块
          if (scanline - spriteScanline[i].y < 8) {
            // Reading Top half Tile
            sprite_pattern_addr_lo =
                ((spriteScanline[i].id & 0x01)
                 << 12)  // Which Pattern Table? 0KB or 4KB offset
                // 因为8x16像素title一个是2个字节，所以下面的是奇数，上面的是偶数
                | ((spriteScanline[i].id & 0xFE)
                   << 4)  // Which Cell? Tile ID * 16 (16 bytes per tile)
                // 因为8 x 16是连续的，所有需要使用&0x07来获取正确的像素点
                | ((scanline - spriteScanline[i].y) &
                   0x07);  // Which Row in cell? (0->7)
          } else {
            // 判断为下面的块 scanline -
            // spriteScanline[i].y的值是大于8的，所以必须使用&0x07
            // Reading Bottom Half Tile
            sprite_pattern_addr_lo =
                ((spriteScanline[i].id & 0x01)
                 << 12)  // Which Pattern Table? 0KB or 4KB offset
                // 因为8x16像素title一个是2个字节，所以下面的是奇数，上面的是偶数
                | (((spriteScanline[i].id & 0xFE) + 1)
                   << 4)  // Which Cell? Tile ID * 16 (16 bytes per tile)
                // 如果不加&0x07会获取到错误的像素点
                | ((scanline - spriteScanline[i].y) &
                   0x07);  // Which Row in cell? (0->7)
          }
        } else {
          // 出现了垂直翻转 55555 好复杂啊啊啊啊啊啊啊啊啊啊   啊啊啊啊
          // 上下两块是颠倒的
          // Sprite is flipped vertically, i.e. upside down
          if (scanline - spriteScanline[i].y < 8) {
            // Reading Top half Tile
            sprite_pattern_addr_lo =
                ((spriteScanline[i].id & 0x01)
                 << 12)  // Which Pattern Table? 0KB or 4KB offset
                | (((spriteScanline[i].id & 0xFE) + 1)
                   << 4)  // Which Cell? Tile ID * 16 (16 bytes per tile)
                | (7 - ((scanline - spriteScanline[i].y) &
                   0x07 ));  // Which Row in cell? (0->7)
          } else {
            // Reading Bottom Half Tile
            sprite_pattern_addr_lo =
                ((spriteScanline[i].id & 0x01)
                 << 12)  // Which Pattern Table? 0KB or 4KB offset
                | ((spriteScanline[i].id & 0xFE)
                   << 4)  // Which Cell? Tile ID * 16 (16 bytes per tile)
                | (7 - ((scanline - spriteScanline[i].y) &
                   0x07));  // Which Row in cell? (0->7)
          }
        }
      }

      // Phew... XD I'm absolutely certain you can use some fantastic bit
      // manipulation to reduce all of that to a few one liners, but in this
      // form it's easy to see the processes required for the different
      // sizes and vertical orientations

      // Hi bit plane equivalent is always offset by 8 bytes from lo bit plane
      sprite_pattern_addr_hi = sprite_pattern_addr_lo + 8;

      // Now we have the address of the sprite patterns, we can read them
      sprite_pattern_bits_lo = ppuRead(sprite_pattern_addr_lo);
      sprite_pattern_bits_hi = ppuRead(sprite_pattern_addr_hi);

      // If the sprite is flipped horizontally, we need to flip the
      // pattern bytes.
      // 镜像翻转精灵
      if (spriteScanline[i].attribute & 0x40) {
        // This little lambda function "flips" a byte
        // so 0b11100000 becomes 0b00000111. It's very
        // clever, and stolen completely from here:
        // https://stackoverflow.com/a/2602885
        auto flipbyte = [](uint8_t b) {
          b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
          b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
          b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
          return b;
        };

        // Flip Patterns Horizontally
        sprite_pattern_bits_lo = flipbyte(sprite_pattern_bits_lo);
        sprite_pattern_bits_hi = flipbyte(sprite_pattern_bits_hi);
      }

      // Finally! We can load the pattern into our sprite shift registers
      // ready for rendering on the next scanline
      // 这里获得了每个精灵在某一行中的所有像素点，一共最多有 8
      // 个，存储的信息是需要绘制的像素点信息，每个都是 8 bit
      // 相加得到最后的像素点 绘制情况见下面代码
      sprite_shifter_pattern_lo[i] = sprite_pattern_bits_lo;
      sprite_shifter_pattern_hi[i] = sprite_pattern_bits_hi;
    }
  }
  if (scanline == 240) {
    // Post Render Scanline - Do Nothing!
  }

  if (scanline >= 241 && scanline < 261) {
    if (scanline == 241 && cycle == 1) {
      // Effectively end of frame, so set vertical blank flag
      status.vertical_blank = 1;

      // If the control register tells us to emit a NMI when
      // entering vertical blanking period, do it! The CPU
      // will be informed that rendering is complete so it can
      // perform operations with the PPU knowing it wont
      // produce visible artefacts
      if (control.enable_nmi) nmi = true;
    }
  }
  // 此时已经获得了所有的前景(精灵)和背景的像素信息，此时需要做合成了
  // 其中，背景像素块的信息中颜色信息，每8个bit需要改变一次
  // Composition - We now have background & foreground pixel information for
  // this cycle Background
  // =============================================================
  uint8_t bg_pixel = 0x00;  // The 2-bit pixel to be rendered
  uint8_t bg_palette =
      0x00;  // The 3-bit index of the palette the pixel indexes

  // We only render backgrounds if the PPU is enabled to do so. Note if
  // background rendering is disabled, the pixel and palette combine
  // to form 0x00. This will fall through the colour tables to yield
  // the current background colour in effect
  if (mask.render_background) {
    // Handle Pixel Selection by selecting the relevant bit
    // depending upon fine x scolling. This has the effect of
    // offsetting ALL background rendering by a set number
    // of pixels, permitting smooth scrolling
    uint16_t bit_mux = 0x8000 >> fine_x;

    // Select Plane pixels by extracting from the shifter
    // at the required location.
    uint8_t p0_pixel = (bg_shifter_pattern_lo & bit_mux) > 0;
    uint8_t p1_pixel = (bg_shifter_pattern_hi & bit_mux) > 0;

    // Combine to form pixel index
    bg_pixel = (p1_pixel << 1) | p0_pixel;

    // Get palette
    uint8_t bg_pal0 = (bg_shifter_attrib_lo & bit_mux) > 0;
    uint8_t bg_pal1 = (bg_shifter_attrib_hi & bit_mux) > 0;
    bg_palette = (bg_pal1 << 1) | bg_pal0;
  }

  // Foreground =============================================================
  uint8_t fg_pixel = 0x00;  // The 2-bit pixel to be rendered
  uint8_t fg_palette =
      0x00;  // The 3-bit index of the palette the pixel indexes
  uint8_t fg_priority = 0x00;  // A bit of the sprite attribute indicates if its
                               // more important than the background
  if (mask.render_sprites) {
    // Iterate through all sprites for this scanline. This is to maintain
    // sprite priority. As soon as we find a non transparent pixel of
    // a sprite we can abort

    bSpriteZeroBeingRendered = false;
    // 寻找0号精灵
    for (uint8_t i = 0; i < sprite_count; i++) {
      // Scanline cycle has "collided" with sprite, shifters taking over
      // 最上方的精灵
      if (spriteScanline[i].x == 0) {
        // Note Fine X scrolling does not apply to sprites, the game
        // should maintain their relationship with the background. So
        // we'll just use the MSB of the shifter

        // Determine the pixel value...
        uint8_t fg_pixel_lo = (sprite_shifter_pattern_lo[i] & 0x80) > 0;
        uint8_t fg_pixel_hi = (sprite_shifter_pattern_hi[i] & 0x80) > 0;
        fg_pixel = (fg_pixel_hi << 1) | fg_pixel_lo;

        // Extract the palette from the bottom two bits. Recall
        // that foreground palettes are the latter 4 in the
        // palette memory.
        fg_palette = (spriteScanline[i].attribute & 0x03) + 0x04;
        fg_priority = (spriteScanline[i].attribute & 0x20) ==
                      0;  // 判断该精灵是在背景的前面还是背景的后面

        // If pixel is not transparent, we render it, and dont
        // bother checking the rest because the earlier sprites
        // in the list are higher priority
        if (fg_pixel != 0) {
          if (i == 0) {  // Is this sprite zero?
            // 既是最上方的精灵，又是最上方的第一个精灵，所以是bSpriteZeroBeingRendered
            // = true;
            bSpriteZeroBeingRendered = true;
          }

          break;
        }
      }
    }
  }

  // Now we have a background pixel and a foreground pixel. They need
  // to be combined. It is possible for sprites to go behind background
  // tiles that are not "transparent", yet another neat trick of the PPU
  // that adds complexity for us poor emulator developers...

  uint8_t pixel = 0x00;    // The FINAL Pixel...
  uint8_t palette = 0x00;  // The FINAL Palette...

  if (bg_pixel == 0 && fg_pixel == 0) {
    // The background pixel is transparent
    // The foreground pixel is transparent
    // No winner, draw "background" colour
    pixel = 0x00;
    palette = 0x00;
  } else if (bg_pixel == 0 && fg_pixel > 0) {
    // The background pixel is transparent
    // The foreground pixel is visible
    // Foreground wins!
    pixel = fg_pixel;
    palette = fg_palette;
  } else if (bg_pixel > 0 && fg_pixel == 0) {
    // The background pixel is visible
    // The foreground pixel is transparent
    // Background wins!
    pixel = bg_pixel;
    palette = bg_palette;
  } else if (bg_pixel > 0 && fg_pixel > 0) {
    // The background pixel is visible
    // The foreground pixel is visible
    // Hmmm...
    if (fg_priority) {
      // Foreground cheats its way to victory!
      pixel = fg_pixel;
      palette = fg_palette;
    } else {
      // Background is considered more important!
      pixel = bg_pixel;
      palette = bg_palette;
    }

    // Sprite Zero Hit detection
    // bSpriteZeroHitPossible ： 0号精灵在最上方 ！ ????
    //
    if (bSpriteZeroHitPossible && bSpriteZeroBeingRendered) {
      // Sprite zero is a collision between foreground and background
      // so they must both be enabled
      if (mask.render_background & mask.render_sprites) {
        // The left edge of the screen has specific switches to control
        // its appearance. This is used to smooth inconsistencies when
        // scrolling (since sprites x coord must be >= 0)
        if (~(mask.render_background_left | mask.render_sprites_left)) {
          if (cycle >= 9 && cycle < 258) {
            status.sprite_zero_hit = 1;
          }
        } else {
          if (cycle >= 1 && cycle < 258) {
            status.sprite_zero_hit = 1;
          }
        }
      }
    }
  }

  // Now we have a final pixel colour, and a palette for this cycle
  // of the current scanline. Let's at long last, draw that ^&%*er :P
  sprScreen.SetPixel(cycle - 1, scanline,
                     GetColourFromPaletteRam(palette, pixel));

  // Fake some noise for now
  // sprScreen.SetPixel(cycle - 1, scanline, palScreen[(rand() % 2) ? 0x3F :
  // 0x30]);

  // Advance renderer - it never stops, it's relentless
  cycle++;

  if ((mask.render_background || mask.render_sprites) &&
      (cycle == 260 && scanline < 240))
    cart->GetMapper()->scanline();
  if (cycle >= 341) {
    cycle = 0;
    scanline++;
    if (scanline >= 261) {
      scanline = -1;
      frame_complete = true;
      odd_frame = !odd_frame;
    }
  }
}
