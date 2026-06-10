// Copyright [2020-2022] <tiansongyu>

#include "Nes2C02.h"

#include <cstring>

#include "StateIO.h"

// 名称表镜像：PPU 地址 0x2000-0x2FFF 共 4 个逻辑名称表象限，
// 物理内存只有 2 KB（两张表）。镜像模式决定象限 -> 物理表的映射。
// 行索引与 MIRROR 枚举一致：HARDWARE, HORIZONTAL, VERTICAL,
// ONESCREEN_LO, ONESCREEN_HI。HARDWARE 在 Cartridge::Mirror() 中已被
// 解析为具体模式，这里仅作防御性兜底（按水平镜像处理）。
static const uint8_t kMirrorTable[5][4] = {
    {0, 0, 1, 1},  // HARDWARE (defensive fallback = horizontal)
    {0, 0, 1, 1},  // HORIZONTAL
    {0, 1, 0, 1},  // VERTICAL
    {0, 0, 0, 0},  // ONESCREEN_LO
    {1, 1, 1, 1},  // ONESCREEN_HI
};

// "翻转"一个字节, 0b11100000 -> 0b00000111
// https://stackoverflow.com/a/2602885
static uint8_t flipbyte(uint8_t b) {
  b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
  b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
  b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
  return b;
}

// NES 系统调色板：64 种颜色，打包为 32 位 RGBA（字节序 r,g,b,a）。
static constexpr uint32_t RGB(uint32_t r, uint32_t g, uint32_t b) {
  return 0xFF000000u | (b << 16) | (g << 8) | r;
}
static constexpr uint32_t kPalette[0x40] = {
    RGB(84, 84, 84), RGB(0, 30, 116), RGB(8, 16, 144), RGB(48, 0, 136),
    RGB(68, 0, 100), RGB(92, 0, 48), RGB(84, 4, 0), RGB(60, 24, 0),
    RGB(32, 42, 0), RGB(8, 58, 0), RGB(0, 64, 0), RGB(0, 60, 0),
    RGB(0, 50, 60), RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0),
    RGB(152, 150, 152), RGB(8, 76, 196), RGB(48, 50, 236), RGB(92, 30, 228),
    RGB(136, 20, 176), RGB(160, 20, 100), RGB(152, 34, 32), RGB(120, 60, 0),
    RGB(84, 90, 0), RGB(40, 114, 0), RGB(8, 124, 0), RGB(0, 118, 40),
    RGB(0, 102, 120), RGB(0, 0, 0), RGB(0, 0, 0), RGB(0, 0, 0),
    RGB(236, 238, 236), RGB(76, 154, 236), RGB(120, 124, 236), RGB(176, 98, 236),
    RGB(228, 84, 236), RGB(236, 88, 180), RGB(236, 106, 100), RGB(212, 136, 32),
    RGB(160, 170, 0), RGB(116, 196, 0), RGB(76, 208, 32), RGB(56, 204, 108),
    RGB(56, 180, 204), RGB(60, 60, 60), RGB(0, 0, 0), RGB(0, 0, 0),
    RGB(236, 238, 236), RGB(168, 204, 236), RGB(188, 188, 236), RGB(212, 178, 236),
    RGB(236, 174, 236), RGB(236, 174, 212), RGB(236, 180, 176), RGB(228, 196, 144),
    RGB(204, 210, 120), RGB(180, 222, 120), RGB(168, 226, 144), RGB(152, 226, 180),
    RGB(160, 214, 228), RGB(160, 162, 160), RGB(0, 0, 0), RGB(0, 0, 0),
};

Nes2C02::Nes2C02() {
  // 上电状态与按下 Reset 相同：所有内部内存清零，
  // 保证模拟器开机行为是确定性的。
  reset();
}

Nes2C02::~Nes2C02() {}

const uint32_t *Nes2C02::GetScreen() const {
  // 返回当前需要绘制的屏幕中的像素 256 x 240（行主序）
  return framebuffer;
}

const uint32_t *Nes2C02::GetPatternTable(uint8_t i, uint8_t palette) {
  // 此函数使用指定的调色板将给定模式表的 CHR ROM 绘制到 olc::Sprite 中。
  // 模式表由 16x16 个"瓷砖(tile)"组成，每个 tile 是 8x8 像素，每像素 2 bit，
  // 存储为两个分离的位平面（低位面 8 字节 + 高位面 8 字节，共 16 字节）。
  for (uint16_t nTileY = 0; nTileY < 16; nTileY++) {
    for (uint16_t nTileX = 0; nTileX < 16; nTileX++) {
      uint16_t nOffset = nTileY * 256 + nTileX * 16;

      for (uint16_t row = 0; row < 8; row++) {
        uint8_t tile_lsb = ppuRead(i * 0x1000 + nOffset + row + 0x0000);
        uint8_t tile_msb = ppuRead(i * 0x1000 + nOffset + row + 0x0008);

        for (uint16_t col = 0; col < 8; col++) {
          uint8_t pixel = ((tile_lsb & 0x01) << 1) + (tile_msb & 0x01);

          tile_lsb >>= 1;
          tile_msb >>= 1;

          patternTable[i][(nTileY * 8 + row) * 128 + nTileX * 8 +
                          (7 - col)] =
              GetColourFromPaletteRam(palette, pixel);
        }
      }
    }
  }

  return patternTable[i];
}

uint32_t Nes2C02::GetColourFromPaletteRam(uint8_t palette, uint8_t pixel) {
  // 输入调色板中制定位置信息，返回颜色
  // "0x3F00"       - 内存中调色板的偏移
  // "palette << 2" - 每个调色板的大小为4字节
  // "pixel"        - 每个像素索引为0、1、2或3
  // "& 0x3F"       - 阻止读取超出调色板边界的内容
  return kPalette[ppuRead(0x3F00 + (palette << 2) + pixel) & 0x3F];
}

uint8_t Nes2C02::cpuRead(uint16_t addr, bool rdonly) {
  uint8_t data = 0x00;

  if (rdonly) {
    // 仅读取ppu寄存器的内容(不改变值)，供调试器使用
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
      default:
        break;
    }
  } else {
    // 读取寄存器中的值，也可能会改变一些状态值
    switch (addr) {
      case 0x0002:  // Status
        // 只有高三位存有信息，低五位是数据总线上的残留
        // https://wiki.nesdev.com/w/index.php?title=PPU_programmer_reference#Status_.28.242002.29_.3C_read
        data = (status.reg & 0xE0) | (ppu_data_buffer & 0x1F);

        // 读取状态寄存器的副作用：清除垂直消隐标志和地址锁存器
        status.vertical_blank = 0;
        address_latch = 0;
        break;

      case 0x0004:  // OAM Data
        // https://wiki.nesdev.com/w/index.php?title=PPU_programmer_reference#OAM_data_.28.242004.29_.3C.3E_read.2Fwrite
        // 这个读取几乎不用（DMA 才是主要途径）
        data = pOAM[oam_addr];
        break;

      case 0x0007:  // PPU Data
        // CPU 从 PPU 读数据要经过一个内部缓冲区，读到的是上一次的缓冲值，
        // 随后缓冲区才更新为当前 VRAM 地址的内容（延迟一拍）。
        data = ppu_data_buffer;
        ppu_data_buffer = ppuRead(vram_addr.reg);
        // 调色板区域例外：数据立即返回
        if (vram_addr.reg >= 0x3F00) data = ppu_data_buffer;
        // 每次读取后 VRAM 地址自动递增：水平模式 +1，垂直模式 +32
        vram_addr.reg += (control.increment_mode ? 32 : 1);
        break;

      default:
        break;
    }
  }

  return data;
}

void Nes2C02::cpuWrite(uint16_t addr, uint8_t data) {
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
    case 0x0002:  // Status - not writable
      break;
    case 0x0003:  // OAM Address
      oam_addr = data;
      break;
    case 0x0004:  // OAM Data
      pOAM[oam_addr] = data;
      break;
    case 0x0005:  // Scroll
      // 滚动寄存器写两次：第一次 X 偏移，第二次 Y 偏移。
      // 细(fine)值是 tile 内的像素偏移，粗(coarse)值是名称表内的 tile 偏移。
      if (address_latch == 0) {
        fine_x = data & 0x07;
        tram_addr.coarse_x = data >> 3;
        address_latch = 1;
      } else {
        tram_addr.fine_y = data & 0x07;
        tram_addr.coarse_y = data >> 3;
        address_latch = 0;
      }
      break;
    case 0x0006:  // PPU Address
      // ppu地址是一个16位地址，分两次传输：先高字节，后低字节。
      // 写入暂存在 tram，第二次写完成后整体转移进 vram 指针。
      if (address_latch == 0) {
        // 高字节写入只替换高位，保留低字节——游戏依赖这一点在
        // 渲染中途做分屏滚动（如状态栏分割）。
        tram_addr.reg =
            (uint16_t)((data & 0x3F) << 8) | (tram_addr.reg & 0x00FF);
        address_latch = 1;
      } else {
        tram_addr.reg = (tram_addr.reg & 0xFF00) | data;
        vram_addr = tram_addr;
        address_latch = 0;
      }
      break;
    case 0x0007:  // PPU Data
      ppuWrite(vram_addr.reg, data);
      // 与读取相同：每次写入后 VRAM 地址自动递增
      vram_addr.reg += (control.increment_mode ? 32 : 1);
      break;
  }
}

uint8_t &Nes2C02::NametableByte(uint16_t addr) {
  // 0x2000-0x3EFF 的低 12 位选择 4 个名称表象限之一(addr >> 10)，
  // 镜像表把象限映射到两张物理表。
  addr &= 0x0FFF;
  return tblName[kMirrorTable[cart->Mirror()][addr >> 10]][addr & 0x03FF];
}

uint8_t &Nes2C02::PaletteByte(uint16_t addr) {
  // 调色板共 32 字节；0x10/0x14/0x18/0x1C 是 0x00/0x04/0x08/0x0C 的镜像
  //（精灵调色板的"透明色"槽位映射回背景）。
  addr &= 0x001F;
  if ((addr & 0x0013) == 0x0010) addr &= ~0x0010;
  return tblPalette[addr];
}

uint8_t Nes2C02::ppuRead(uint16_t addr, bool rdonly) {
  (void)rdonly;
  uint8_t data = 0x00;
  addr &= 0x3FFF;

  if (cart->ppuRead(addr, data)) {
    // 卡带（CHR ROM/RAM）已处理
  } else if (addr <= 0x1FFF) {
    // 模式表（卡带未映射时的内部后备）
    data = tblPattern[(addr & 0x1000) >> 12][addr & 0x0FFF];
  } else if (addr <= 0x3EFF) {
    data = NametableByte(addr);
  } else {
    data = PaletteByte(addr) & (mask.grayscale ? 0x30 : 0x3F);
  }

  return data;
}

void Nes2C02::ppuWrite(uint16_t addr, uint8_t data) {
  addr &= 0x3FFF;

  if (cart->ppuWrite(addr, data)) {
    // 卡带已处理
  } else if (addr <= 0x1FFF) {
    tblPattern[(addr & 0x1000) >> 12][addr & 0x0FFF] = data;
  } else if (addr <= 0x3EFF) {
    NametableByte(addr) = data;
  } else {
    PaletteByte(addr) = data;
  }
}

void Nes2C02::ConnectCartridge(const std::shared_ptr<Cartridge> &cartridge) {
  this->cart = cartridge;
}

void Nes2C02::reset() {
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
  // 精灵状态与内部内存也要有确定的初始值——否则上电后头几帧
  // 会渲染出随机垃圾（旧代码 sprite_count 甚至可能越界索引）。
  oam_addr = 0x00;
  sprite_count = 0;
  bSpriteZeroHitPossible = false;
  bSpriteZeroBeingRendered = false;
  nmi = false;
  frame_complete = false;
  std::memset(OAM, 0, sizeof(OAM));
  std::memset(spriteScanline, 0, sizeof(spriteScanline));
  std::memset(sprite_shifter_pattern_lo, 0, sizeof(sprite_shifter_pattern_lo));
  std::memset(sprite_shifter_pattern_hi, 0, sizeof(sprite_shifter_pattern_hi));
  std::memset(tblName, 0, sizeof(tblName));
  std::memset(tblPattern, 0, sizeof(tblPattern));
  std::memset(tblPalette, 0, sizeof(tblPalette));
  for (auto &px : framebuffer) px = 0xFF000000u;  // 不透明黑
  for (auto &tbl : patternTable)
    for (auto &px : tbl) px = 0xFF000000u;
}

// ==============================================================================
// 背景滚动“指针”操作。Loopy 寄存器的滚动机制详见：
// https://wiki.nesdev.com/w/index.php?title=PPU_scrolling

// 将背景平铺“指针”水平增加一个平铺/列。
// 单个名称表为 32x30 tile，水平越界时回绕并翻转目标名称表位。
void Nes2C02::IncrementScrollX() {
  if (mask.render_background || mask.render_sprites) {
    if (vram_addr.coarse_x == 31) {
      vram_addr.coarse_x = 0;
      vram_addr.nametable_x = ~vram_addr.nametable_x;
    } else {
      vram_addr.coarse_x++;
    }
  }
}

// 将背景平铺“指针”垂直增加一条扫描线。
// fine_y 是 tile 内的像素行(0-7)；coarse_y 是名称表内的 tile 行。
// 名称表可见区为 32x30，底部两行是属性表，所以 coarse_y 在 29 处回绕
// 并翻转名称表位；29 之后（30/31，指针落在属性区）只回绕不翻转。
void Nes2C02::IncrementScrollY() {
  if (mask.render_background || mask.render_sprites) {
    if (vram_addr.fine_y < 7) {
      vram_addr.fine_y++;
    } else {
      vram_addr.fine_y = 0;

      if (vram_addr.coarse_y == 29) {
        vram_addr.coarse_y = 0;
        vram_addr.nametable_y = ~vram_addr.nametable_y;
      } else if (vram_addr.coarse_y == 31) {
        vram_addr.coarse_y = 0;
      } else {
        vram_addr.coarse_y++;
      }
    }
  }
}

// 把暂存的水平名称表信息转移进“指针”。fine_x 不属于指针寻址机制。
void Nes2C02::TransferAddressX() {
  if (mask.render_background || mask.render_sprites) {
    vram_addr.nametable_x = tram_addr.nametable_x;
    vram_addr.coarse_x = tram_addr.coarse_x;
  }
}

// 把暂存的垂直名称表信息转移进“指针”。fine_y 属于指针寻址机制。
void Nes2C02::TransferAddressY() {
  if (mask.render_background || mask.render_sprites) {
    vram_addr.fine_y = tram_addr.fine_y;
    vram_addr.nametable_y = tram_addr.nametable_y;
    vram_addr.coarse_y = tram_addr.coarse_y;
  }
}

// 为“有效”背景平铺移位器装填下一组 8 像素。
// 移位器 16 位宽：高 8 位是正在绘制的 8 像素，低 8 位是下一组。
// 属性位每 8 像素才变化一次，为方便与图样移位器同步，把 2 bit
// 调色板选择“充气”成 8 bit。
void Nes2C02::LoadBackgroundShifters() {
  bg_shifter_pattern_lo = (bg_shifter_pattern_lo & 0xFF00) | bg_next_tile_lsb;
  bg_shifter_pattern_hi = (bg_shifter_pattern_hi & 0xFF00) | bg_next_tile_msb;

  bg_shifter_attrib_lo = (bg_shifter_attrib_lo & 0xFF00) |
                         ((bg_next_tile_attrib & 0b01) ? 0xFF : 0x00);
  bg_shifter_attrib_hi = (bg_shifter_attrib_hi & 0xFF00) |
                         ((bg_next_tile_attrib & 0b10) ? 0xFF : 0x00);
}

// 每个周期输出推进 1 像素，所有移位器同步左移 1 位。
// 精灵的 x 计数器先倒数到 0，其图样才开始移位（精灵相对扫描线的延迟）。
void Nes2C02::UpdateShifters() {
  if (mask.render_background) {
    bg_shifter_pattern_lo <<= 1;
    bg_shifter_pattern_hi <<= 1;
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
}

// ==============================================================================
// 可见扫描线（含 -1 预渲染行）上的背景瓦片状态机。
// 背景渲染每 8 个周期重复一组事件：取瓦片 ID -> 取属性 ->
// 取低位面 -> 取高位面 -> 水平推进。
void Nes2C02::BackgroundFetch() {
  // "Odd Frame" cycle skip：渲染开启时，奇数帧跳过 (0,0) 周期
  if (scanline == 0 && cycle == 0 && odd_frame &&
      (mask.render_background || mask.render_sprites)) {
    cycle = 1;
  }

  // 预渲染行的第 1 周期：新一帧开始，清各种状态标志
  if (scanline == -1 && cycle == 1) {
    status.vertical_blank = 0;
    status.sprite_overflow = 0;
    status.sprite_zero_hit = 0;

    for (int i = 0; i < 8; i++) {
      sprite_shifter_pattern_lo[i] = 0;
      sprite_shifter_pattern_hi[i] = 0;
    }
  }

  if ((cycle >= 2 && cycle < 258) || (cycle >= 321 && cycle < 338)) {
    UpdateShifters();

    switch ((cycle - 1) % 8) {
      case 0:
        // 装填移位器，并取下一个瓦片 ID。
        // loopy 寄存器低 12 位（nametable_y nametable_x coarse_y coarse_x）
        // 恰好是 4 个名称表共 4096 个位置的索引，| 0x2000 偏移进
        // PPU 地址空间的名称表区。
        LoadBackgroundShifters();
        bg_next_tile_id = ppuRead(0x2000 | (vram_addr.reg & 0x0FFF));
        break;
      case 2:
        // 取属性字节。属性表在每个名称表的 0x03C0 偏移处，每字节
        // 覆盖 4x4 tile 区域（coarse 坐标各除以 4），字节内按 2x2 tile
        // 分成 BR(76) BL(54) TR(32) TL(10) 四组，各 2 bit 选调色板。
        bg_next_tile_attrib = ppuRead(0x23C0 | (vram_addr.nametable_y << 11) |
                                      (vram_addr.nametable_x << 10) |
                                      ((vram_addr.coarse_y >> 2) << 3) |
                                      (vram_addr.coarse_x >> 2));

        if (vram_addr.coarse_y & 0x02) bg_next_tile_attrib >>= 4;
        if (vram_addr.coarse_x & 0x02) bg_next_tile_attrib >>= 2;
        bg_next_tile_attrib &= 0x03;
        break;

      case 4:
        // 取瓦片低位面。瓦片 ID * 16（每瓦片 16 字节）+ fine_y 选行，
        // 控制寄存器选择 0K/4K 模式表。
        bg_next_tile_lsb = ppuRead((control.pattern_background << 12) +
                                   ((uint16_t)bg_next_tile_id << 4) +
                                   (vram_addr.fine_y) + 0);
        break;
      case 6:
        // 取瓦片高位面（低位面 +8 字节）
        bg_next_tile_msb = ppuRead((control.pattern_background << 12) +
                                   ((uint16_t)bg_next_tile_id << 4) +
                                   (vram_addr.fine_y) + 8);
        break;
      case 7:
        // 水平推进到下一个瓦片（可能跨名称表）
        IncrementScrollX();
        break;
    }
  }

  // 可见区结束，垂直推进一条扫描线
  if (cycle == 256) {
    IncrementScrollY();
  }

  // 行尾：装填移位器并恢复水平位置
  if (cycle == 257) {
    LoadBackgroundShifters();
    TransferAddressX();
  }

  // 行尾多余的瓦片 ID 读取（真实硬件行为）
  if (cycle == 338 || cycle == 340) {
    bg_next_tile_id = ppuRead(0x2000 | (vram_addr.reg & 0x0FFF));
  }

  // 预渲染行尾：垂直消隐结束，恢复 Y 位置准备渲染新帧
  if (scanline == -1 && cycle >= 280 && cycle < 305) {
    TransferAddressY();
  }
}

// ==============================================================================
// 计算精灵某一行的低位面图样地址。8x8 与 8x16、是否垂直翻转
// 由同一组行算术统一处理：
//   row  = 扫描线相对精灵顶部的行号
//   翻转 = 把 row 镜像（8x8 内为 7-row；8x16 跨上下两块为 15-row）
//   8x16 模式：bit0 选模式表，(id & 0xFE) + row/8 选上下块
uint16_t Nes2C02::SpritePatternAddrLo(const sObjectAttributeEntry &s) const {
  uint16_t row = (uint16_t)(scanline - s.y);
  bool flipped_v = s.attribute & 0x80;

  if (!control.sprite_size) {
    // 8x8：控制寄存器选模式表
    if (flipped_v) row = 7 - row;
    return (control.pattern_sprite << 12) | (s.id << 4) | row;
  }

  // 8x16：精灵 ID 的 bit0 选模式表，上下两块各 8 行
  if (flipped_v) row = 15 - row;
  return ((s.id & 0x01) << 12) | (((s.id & 0xFE) + ((row >> 3) & 0x01)) << 4) |
         (row & 0x07);
}

// 前景（精灵）处理。真实 PPU 在背景空闲周期里逐步进行精灵评估，
// 这里为了易于理解在两个时间点一次性完成（视频教程的刻意简化）：
//   周期 257：评估下一条扫描线可见的至多 8 个精灵
//   周期 340：为这 8 个精灵装载图样移位器
void Nes2C02::ForegroundFetch() {
  if (cycle == 257 && scanline >= 0) {
    // 清空扫描线精灵缓存（0xFF 使 y 坐标处于不可见区）
    std::memset(spriteScanline, 0xFF, 8 * sizeof(sObjectAttributeEntry));
    sprite_count = 0;

    for (uint8_t i = 0; i < 8; i++) {
      sprite_shifter_pattern_lo[i] = 0;
      sprite_shifter_pattern_hi[i] = 0;
    }

    // 遍历 OAM，找出与下一条扫描线相交的精灵。0 号精灵入选时
    // 记录“可能发生 0 号精灵命中”。
    uint8_t nOAMEntry = 0;
    bSpriteZeroHitPossible = false;

    while (nOAMEntry < 64) {
      int16_t diff = ((int16_t)scanline - (int16_t)OAM[nOAMEntry].y);

      if (diff >= 0 && diff < (control.sprite_size ? 16 : 8)) {
        if (sprite_count < 8) {
          if (nOAMEntry == 0) {
            bSpriteZeroHitPossible = true;
          }

          memcpy(&spriteScanline[sprite_count], &OAM[nOAMEntry],
                 sizeof(sObjectAttributeEntry));
          sprite_count++;
        } else {
          // 第 9 个落在本行的精灵：置溢出标志（硬件的真实扫描还有
          // 一个著名的对角线 bug，这里采用简单的正确近似）
          status.sprite_overflow = 1;
        }
      }

      nOAMEntry++;
    }
  }

  if (cycle == 340) {
    // 行末：把选中精灵的图样字节装入移位器
    for (uint8_t i = 0; i < sprite_count; i++) {
      uint16_t addr_lo = SpritePatternAddrLo(spriteScanline[i]);
      uint8_t bits_lo = ppuRead(addr_lo);
      uint8_t bits_hi = ppuRead(addr_lo + 8);  // 高位面恒为低位面 +8 字节

      // 水平翻转
      if (spriteScanline[i].attribute & 0x40) {
        bits_lo = flipbyte(bits_lo);
        bits_hi = flipbyte(bits_hi);
      }

      sprite_shifter_pattern_lo[i] = bits_lo;
      sprite_shifter_pattern_hi[i] = bits_hi;
    }
  }
}

// ==============================================================================
// 背景与前景像素合成，并写入屏幕。
void Nes2C02::ComposePixel() {
  // 背景像素：fine_x 决定从移位器的哪一位取样（平滑滚动）
  uint8_t bg_pixel = 0x00;    // 2 bit 像素值
  uint8_t bg_palette = 0x00;  // 3 bit 调色板索引

  if (mask.render_background) {
    uint16_t bit_mux = 0x8000 >> fine_x;

    uint8_t p0_pixel = (bg_shifter_pattern_lo & bit_mux) > 0;
    uint8_t p1_pixel = (bg_shifter_pattern_hi & bit_mux) > 0;
    bg_pixel = (p1_pixel << 1) | p0_pixel;

    uint8_t bg_pal0 = (bg_shifter_attrib_lo & bit_mux) > 0;
    uint8_t bg_pal1 = (bg_shifter_attrib_hi & bit_mux) > 0;
    bg_palette = (bg_pal1 << 1) | bg_pal0;
  }

  // 前景像素：按优先级遍历本行精灵，取第一个不透明像素
  uint8_t fg_pixel = 0x00;
  uint8_t fg_palette = 0x00;
  uint8_t fg_priority = 0x00;

  if (mask.render_sprites) {
    bSpriteZeroBeingRendered = false;

    for (uint8_t i = 0; i < sprite_count; i++) {
      if (spriteScanline[i].x == 0) {
        // fine_x 不作用于精灵，直接取移位器 MSB
        uint8_t fg_pixel_lo = (sprite_shifter_pattern_lo[i] & 0x80) > 0;
        uint8_t fg_pixel_hi = (sprite_shifter_pattern_hi[i] & 0x80) > 0;
        fg_pixel = (fg_pixel_hi << 1) | fg_pixel_lo;

        // 前景调色板是调色板内存的后 4 个
        fg_palette = (spriteScanline[i].attribute & 0x03) + 0x04;
        fg_priority = (spriteScanline[i].attribute & 0x20) == 0;

        if (fg_pixel != 0) {
          if (i == 0) {
            bSpriteZeroBeingRendered = true;
          }
          break;
        }
      }
    }
  }

  // 合成：透明(0)的一方让位；都不透明时由精灵的优先级位决定
  uint8_t pixel = 0x00;
  uint8_t palette = 0x00;

  if (bg_pixel == 0 && fg_pixel > 0) {
    pixel = fg_pixel;
    palette = fg_palette;
  } else if (bg_pixel > 0 && fg_pixel == 0) {
    pixel = bg_pixel;
    palette = bg_palette;
  } else if (bg_pixel > 0 && fg_pixel > 0) {
    if (fg_priority) {
      pixel = fg_pixel;
      palette = fg_palette;
    } else {
      pixel = bg_pixel;
      palette = bg_palette;
    }

    // 0 号精灵命中：前景与背景同时不透明且双双开启渲染。
    // 若背景或精灵任意一方关闭了左缘 8 像素渲染，则命中不会发生在
    // 屏幕最左 8 像素内（nesdev: Sprite 0 hit）。
    if (bSpriteZeroHitPossible && bSpriteZeroBeingRendered) {
      if (mask.render_background & mask.render_sprites) {
        int first_hit_cycle =
            (mask.render_background_left && mask.render_sprites_left) ? 1 : 9;
        if (cycle >= first_hit_cycle && cycle < 258) {
          status.sprite_zero_hit = 1;
        }
      }
    }
  }

  // 只有可见窗口内的像素才落入帧缓冲
  if (cycle >= 1 && cycle <= 256 && scanline >= 0 && scanline < 240) {
    framebuffer[scanline * 256 + (cycle - 1)] =
        GetColourFromPaletteRam(palette, pixel);
  }
}

// 推进扫描位置，并在每条可见扫描线的 260 周期通知 mapper
//（MMC3 用它近似 A12 边沿来驱动扫描线计数 IRQ）。
void Nes2C02::AdvanceCounters() {
  cycle++;

  if ((mask.render_background || mask.render_sprites) &&
      (cycle == 260 && scanline < 240)) {
    cart->GetMapper()->scanline();
  }

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

// PPU 时钟主入口。PPU 是一个跟随 scanline/cycle 前进的状态机：
// 取背景信息、取精灵信息、把两者合成为一个输出像素。
void Nes2C02::clock() {
  if (scanline >= -1 && scanline < 240) {
    BackgroundFetch();
  }

  ForegroundFetch();

  // scanline 240 是后渲染行，什么都不发生。
  // 241 行第 1 周期进入垂直消隐，按需触发 NMI——CPU 由此得知渲染
  // 完毕，可以安全地更新 PPU 内存。
  if (scanline == 241 && cycle == 1) {
    status.vertical_blank = 1;
    if (control.enable_nmi) nmi = true;
  }

  ComposePixel();
  AdvanceCounters();
}

void Nes2C02::SaveState(std::ostream &os) const {
  PutPod(os, tblName);
  PutPod(os, tblPattern);
  PutPod(os, tblPalette);
  PutPod(os, status.reg);
  PutPod(os, mask.reg);
  PutPod(os, control.reg);
  PutPod(os, vram_addr.reg);
  PutPod(os, tram_addr.reg);
  PutPod(os, fine_x);
  PutPod(os, address_latch);
  PutPod(os, ppu_data_buffer);
  PutPod(os, scanline);
  PutPod(os, cycle);
  PutPod(os, bg_next_tile_id);
  PutPod(os, bg_next_tile_attrib);
  PutPod(os, bg_next_tile_lsb);
  PutPod(os, bg_next_tile_msb);
  PutPod(os, bg_shifter_pattern_lo);
  PutPod(os, bg_shifter_pattern_hi);
  PutPod(os, bg_shifter_attrib_lo);
  PutPod(os, bg_shifter_attrib_hi);
  PutPod(os, OAM);
  PutPod(os, oam_addr);
  PutPod(os, spriteScanline);
  PutPod(os, sprite_count);
  PutPod(os, sprite_shifter_pattern_lo);
  PutPod(os, sprite_shifter_pattern_hi);
  PutPod(os, bSpriteZeroHitPossible);
  PutPod(os, bSpriteZeroBeingRendered);
  PutPod(os, odd_frame);
  PutPod(os, nmi);
}

void Nes2C02::LoadState(std::istream &is) {
  GetPod(is, tblName);
  GetPod(is, tblPattern);
  GetPod(is, tblPalette);
  GetPod(is, status.reg);
  GetPod(is, mask.reg);
  GetPod(is, control.reg);
  GetPod(is, vram_addr.reg);
  GetPod(is, tram_addr.reg);
  GetPod(is, fine_x);
  GetPod(is, address_latch);
  GetPod(is, ppu_data_buffer);
  GetPod(is, scanline);
  GetPod(is, cycle);
  GetPod(is, bg_next_tile_id);
  GetPod(is, bg_next_tile_attrib);
  GetPod(is, bg_next_tile_lsb);
  GetPod(is, bg_next_tile_msb);
  GetPod(is, bg_shifter_pattern_lo);
  GetPod(is, bg_shifter_pattern_hi);
  GetPod(is, bg_shifter_attrib_lo);
  GetPod(is, bg_shifter_attrib_hi);
  GetPod(is, OAM);
  GetPod(is, oam_addr);
  GetPod(is, spriteScanline);
  GetPod(is, sprite_count);
  GetPod(is, sprite_shifter_pattern_lo);
  GetPod(is, sprite_shifter_pattern_hi);
  GetPod(is, bSpriteZeroHitPossible);
  GetPod(is, bSpriteZeroBeingRendered);
  GetPod(is, odd_frame);
  GetPod(is, nmi);
  frame_complete = false;
}
