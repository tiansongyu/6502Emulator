#pragma once
#include <cstdint>
#include <memory>

#include "olcPixelGameEngine.h"

#include "Cartridge.h"
class olc2C02
{
public:
	olc2C02();
	~olc2C02();

private:
	uint8_t tblName[2][1024];	 // 名称表
	uint8_t tblPattern[2][4096]; // 模式表
	uint8_t tblPalette[32];		 // 调色板

private:
	olc::Pixel palScreen[0x40];														 // 颜色
	olc::Sprite sprScreen = olc::Sprite(256, 240);									 // 显示器
	olc::Sprite sprNameTable[2] = {olc::Sprite(256, 240), olc::Sprite(256, 240)};	 // 读取需要显示的名称表
	olc::Sprite sprPatternTable[2] = {olc::Sprite(128, 128), olc::Sprite(128, 128)}; // 需要显示的模式表
	olc::Sprite sprSpriteTitle[26];													 // 0-26个OAM中的精灵

public:
	// 调试函数(打印各种信息)
	olc::Sprite &GetScreen();								  // 屏幕
	olc::Sprite &GetNameTable(uint8_t i);					  // 获取名称表
	olc::Sprite &GetPatternTable(uint8_t i, uint8_t palette); // 获取模式表
	olc::Sprite &GetSpriteTitle(uint8_t x,					  // 打印部分精灵OAM信息
								uint8_t y,
								uint8_t title,
								uint8_t attr,
								uint8_t palette,
								int i);

	olc::Pixel &GetColourFromPaletteRam(uint8_t palette, uint8_t pixel);	// 获取调色后的pixel

	bool frame_complete = false;							  // 用来判断帧的绘制是否完成

private:
	union
	{
		struct
		{
			uint8_t unused : 5;
			uint8_t sprite_overflow : 1;
			uint8_t sprite_zero_hit : 1;
			uint8_t vertical_blank : 1;
		};

		uint8_t reg;
	} status; // 状态寄存器

	union
	{
		struct
		{
			uint8_t grayscale : 1;
			uint8_t render_background_left : 1;
			uint8_t render_sprites_left : 1;
			uint8_t render_background : 1;
			uint8_t render_sprites : 1;
			uint8_t enhance_red : 1;
			uint8_t enhance_green : 1;
			uint8_t enhance_blue : 1;
		};

		uint8_t reg;
	} mask;		//掩码寄存器

	union PPUCTRL
	{
		struct
		{
			uint8_t nametable_x : 1;
			uint8_t nametable_y : 1;
			uint8_t increment_mode : 1;
			uint8_t pattern_sprite : 1;
			uint8_t pattern_background : 1;
			uint8_t sprite_size : 1;
			uint8_t slave_mode : 1; // unused
			uint8_t enable_nmi : 1;
		};

		uint8_t reg;
	} control;		//控制寄存器

	union loopy_register
	{
		// Credit to Loopy for working this out :D
		struct
		{

			uint16_t coarse_x : 5;	  // 0 - 31 nametable的 x 轴坐标
			uint16_t coarse_y : 5;	  // 0 - 31 nametable的 y 轴坐标
			uint16_t nametable_x : 1; //
			uint16_t nametable_y : 1;
			uint16_t fine_y : 3; //0- 8 用来记录此 pixel 绘制的y 坐标，
			uint16_t unused : 1;
		};

		uint16_t reg = 0x0000;
	}; // ppu中的绘制寄存器
	// 将活动“指针”地址插入nametable以提取背景磁贴信息
	loopy_register vram_addr; // Active "pointer" address into nametable to extract background tile info
	// 在不同时间将信息临时存储到“指针”中
	loopy_register tram_addr; // Temporary store of information to be "transferred" into "pointer" at various times

	// 像素水平偏移
	uint8_t fine_x = 0x00;

	// ppu 内部寄存器
	uint8_t address_latch = 0x00;
	uint8_t ppu_data_buffer = 0x00;

	// 像素“点”位置信息
	int16_t scanline = 0; // 用来记录扫描的行数
	int16_t cycle = 0;	  // 记录扫描点的列数 

	// 背景像素渲染信息
	uint8_t bg_next_tile_id = 0x00;
	uint8_t bg_next_tile_attrib = 0x00;
	uint8_t bg_next_tile_lsb = 0x00;
	uint8_t bg_next_tile_msb = 0x00;
	uint16_t bg_shifter_pattern_lo = 0x0000;
	uint16_t bg_shifter_pattern_hi = 0x0000;
	uint16_t bg_shifter_attrib_lo = 0x0000;
	uint16_t bg_shifter_attrib_hi = 0x0000;

	// 前景"精灵"渲染信息
	// OAM是PPU内部的附加内存。
	// 未通过任何总线连接。它
	// 存储在下一帧上绘制64个8x8（或8x16）瓷砖。
	struct sObjectAttributeEntry
	{
		uint8_t y;		   // 精灵在名称表的Y轴坐标 Y  1- 256
		uint8_t id;		   // 精灵在模式表中的ID号，一共两张表，
		uint8_t attribute; // 存放该精灵的title属性
		uint8_t x;		   // 精灵在名称表的Y轴坐标 X 1-240
	} OAM[64];

	//当CPU手动通信时存储地址的寄存器
	//通过PPU寄存器使用OAM。不常用，因为它
	//非常慢，使用256字节的DMA传输。
	uint8_t oam_addr = 0x00;

	sObjectAttributeEntry spriteScanline[8];    // 在一行中，存放的最多8个精灵
	uint8_t sprite_count;						// 在一行中的精灵数量
	uint8_t sprite_shifter_pattern_lo[8];		// 下一行将要绘制的精灵像素的低8个字节 信息
	uint8_t sprite_shifter_pattern_hi[8]; 		// 下一行将要绘制的精灵像素的高8个字节 信息
										  		//  这两个字节相加的结果组成一个8x8bit的像素图片
	// 0号精灵命中标志
	bool bSpriteZeroHitPossible = false;
	bool bSpriteZeroBeingRendered = false;
	bool odd_frame = false;

public:
	// 上面的OAM可以方便地打包使用，但是DMA
	// 机制需要访问它，以便一次编写一个byte
	// 将OAM强转为字节指针，方便DMA直接传输
	uint8_t *pOAM = (uint8_t *)OAM;

public:
	// PPU与主线CPU进行通信函数
	uint8_t cpuRead(uint16_t addr, bool rdonly = false);
	void cpuWrite(uint16_t addr, uint8_t data);

	// 与PPU内部总线进行通信函数
	// 其中包含一些镜像地址需要转换
	uint8_t ppuRead(uint16_t addr, bool rdonly = false);
	void ppuWrite(uint16_t addr, uint8_t data);

private:
	// 卡带
	std::shared_ptr<Cartridge> cart;

public:
	// 外部接口
	void ConnectCartridge(const std::shared_ptr<Cartridge> &cartridge);
	void clock();
	void reset();
	bool nmi = false;
};
