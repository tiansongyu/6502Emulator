/*
	olc::NES - System Bus
	"Thanks Dad for believing computers were gonna be a big deal..." - javidx9

	License (OLC-3)
	~~~~~~~~~~~~~~~

	Copyright 2018-2019 OneLoneCoder.com

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:

	1. Redistributions or derivations of source code must retain the above
	copyright notice, this list of conditions and the following disclaimer.

	2. Redistributions or derivative works in binary form must reproduce
	the above copyright notice. This list of conditions and the following
	disclaimer must be reproduced in the documentation and/or other
	materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its
	contributors may be used to endorse or promote products derived
	from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


	Relevant Video: https://youtu.be/xdzOvpYPmGE

	Links
	~~~~~
	YouTube:	https://www.youtube.com/javidx9
				https://www.youtube.com/javidx9extra
	Discord:	https://discord.gg/WhwHUMV
	Twitter:	https://www.twitter.com/javidx9
	Twitch:		https://www.twitch.tv/javidx9
	GitHub:		https://www.github.com/onelonecoder
	Patreon:	https://www.patreon.com/javidx9
	Homepage:	https://www.onelonecoder.com

	Author
	~~~~~~
	David Barr, aka javidx9, �OneLoneCoder 2019
*/

#include "Bus.h"

Bus::Bus()
{
	// Connect CPU to communication bus
	//连接cpu
	cpu.ConnectBus(this);
	
}


Bus::~Bus()
{
}
// 总线可以访问所有位置,并根据地址位置写入不同设备数据

// 这里注意，Bus中可以读取 cpu ppu mapper ram等所有的内存区域
// 所以当 cpu需要执行read 或者 write 执行时，其实是先使用bus->read(write)去读取，
// 再根据bus 中的地址，来判断是读写 cpu 还是 ppu 还是 mapper 还是ram 等区域

// 这里的关系是bus中包含cpu ,cpu中也有bus 
// cpu 将调用指令传递到 bus中，然后由bus再去执行读写，这样就达到了，使用bus来控制所有的内存区域
// 但其实所有的操作都是由cpu来执行的，bus只不过充当了一个运输read write指令的作用，使得cpu可以访问所有的内存区域
// 因为bus中是包含所有的设备的!!!!!!!!!!
void Bus::cpuWrite(uint16_t addr, uint8_t data)
{	
	// addr >= 0x8000 && addr <= 0xFFFF
	if (cart->cpuWrite(addr, data))
	{
		// The cartridge "sees all" and has the facility to veto
		// the propagation of the bus transaction if it requires.
		// This allows the cartridge to map any address to some
		// other data, including the facility to divert transactions
		// with other physical devices. The NES does not do this
		// but I figured it might be quite a flexible way of adding
		// "custom" hardware to the NES in the future!
	}
	else if (addr >= 0x0000 && addr <= 0x1FFF)
	{
		// System RAM Address Range. The range covers 8KB, though
		// there is only 2KB available. That 2KB is "mirrored"
		// through this address range. Using bitwise AND to mask
		// the bottom 11 bits is the same as addr % 2048.
		cpuRam[addr & 0x07FF] = data;

	}
	else if (addr >= 0x2000 && addr <= 0x3FFF)
	{
		// PPU Address range. The PPU only has 8 primary registers
		// and these are repeated throughout this range. We can
		// use bitwise AND operation to mask the bottom 3 bits, 
		// which is the equivalent of addr % 8.
		ppu.cpuWrite(addr & 0x0007, data);
	}		
}

uint8_t Bus::cpuRead(uint16_t addr, bool bReadOnly)
{
	uint8_t data = 0x00;	
	if (cart->cpuRead(addr, data))
	{
		// Cartridge Address Range
	}
	else if (addr >= 0x0000 && addr <= 0x1FFF)
	{
		// System RAM Address Range, mirrored every 2048
		data = cpuRam[addr & 0x07FF];
	}
	else if (addr >= 0x2000 && addr <= 0x3FFF)
	{
		// PPU Address range, mirrored every 8
		data = ppu.cpuRead(addr & 0x0007, bReadOnly);
	}

	return data;
}

void Bus::insertCartridge(const std::shared_ptr<Cartridge>& cartridge)
{
	// Connects cartridge to both Main Bus and CPU Bus
	this->cart = cartridge;
	ppu.ConnectCartridge(cartridge);
}

void Bus::reset()
{
	cpu.reset();
	nSystemClockCounter = 0;
}

void Bus::clock()
{
	// Clocking. The heart and soul of an emulator. The running
	// frequency is controlled by whatever calls this function.
	// So here we "divide" the clock as necessary and call
	// the peripheral devices clock() function at the correct
	// times.

	// The fastest clock frequency the digital system cares
	// about is equivalent to the PPU clock. So the PPU is clocked
	// each time this function is called.
	ppu.clock();

	// The CPU runs 3 times slower than the PPU so we only call its
	// clock() function every 3 times this function is called. We
	// have a global counter to keep track of this.
	if (nSystemClockCounter % 3 == 0)
	{
		cpu.clock();
	}

	nSystemClockCounter++;
}
