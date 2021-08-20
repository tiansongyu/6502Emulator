#include "Bus.h"

Bus::Bus()
{
	// 在nes最初启动时，连接cpu到bus
	cpu.ConnectBus(this);
}

Bus::~Bus()
{
}

void Bus::SetSampleFrequency(uint32_t sample_rate)
{
	// sample_rate是声音采样率，这个采样率决定一个声音数据的发生周期
	dAudioTimePerSystemSample = 1.0 / (double)sample_rate;
	dAudioTimePerNESClock = 1.0 / 5369318.0; //   1 / ppu的时钟频率  = ppu的时钟周期
}

void Bus::cpuWrite(uint16_t addr, uint8_t data)
{
	if (cart->cpuWrite(addr, data))
	{
		// cartridge进行首次判断
		// cartridge可以根據需要將內容映射到其他任何地址
		// 通过这种方式，可以扩展硬件外设，比如枪。
	}
	else if (addr >= 0x0000 && addr <= 0x1FFF)
	{
		// 系统内存地址
		// 只有一共8kb，但是只有2kb可以用
		// 其他地址是2kb的镜像，使用&0x07ff运算，只使用2kb
		cpuRam[addr & 0x07FF] = data;
	}
	else if (addr >= 0x2000 && addr <= 0x3FFF)
	{
		// PPU地址
		// ppu只有8个寄存器，在0x2000 - 0x3fff这个范围内，都是这8个寄存器的重复镜像数据
		// 所以直接使用&0x0007获取正确地址即可
		ppu.cpuWrite(addr & 0x0007, data);
	}
	else if ((addr >= 0x4000 && addr <= 0x4013) || addr == 0x4015 || addr == 0x4017) 
	{
		// APU地址
		// apu的几个寄存器地址 0x4000 - 0x4013 0x4015 0x4017
		apu.cpuWrite(addr, data);
	}
	else if (addr == 0x4014)
	{
		// 0x4014地址是DMA传送开始标志
		// 当此地址被写入数据时，说明发生DMA传送
		// CPU周期将停止，将OAM寄存器中的数据传送到PPU中，64个精灵，直到传送结束
		dma_page = data;
		dma_addr = 0x00;
		dma_transfer = true;
	}
	else if (addr >= 0x4016 && addr <= 0x4017)
	{
		// 控制寄存器地址
		// 0x4016 - 0x4017是控制寄存器地址，cpu通过读取此地址中的数据
		// 获取控制命令
		controller_state[addr & 0x0001] = controller[addr & 0x0001];
	}
}

uint8_t Bus::cpuRead(uint16_t addr, bool bReadOnly)
{
	uint8_t data = 0x00;
	if (cart->cpuRead(addr, data))
	{
		// Cartridge 地址
	}
	else if (addr >= 0x0000 && addr <= 0x1FFF)
	{
		// 系统内存地址RAM, 每2kb都是镜像  2kb = 0x07FF
		data = cpuRam[addr & 0x07FF];
	}
	else if (addr >= 0x2000 && addr <= 0x3FFF)
	{
		// PPU地址 每8个字节都是镜像数据
		data = ppu.cpuRead(addr & 0x0007, bReadOnly);
	}
	else if (addr == 0x4015)
	{
		// APU读寄存器
		data = apu.cpuRead(addr);
	}
	else if (addr >= 0x4016 && addr <= 0x4017)
	{
		// 读取控制命令数据
		data = (controller_state[addr & 0x0001] & 0x80) > 0;
		controller_state[addr & 0x0001] <<= 1;
	}

	return data;
}

void Bus::insertCartridge(const std::shared_ptr<Cartridge> &cartridge)
{
	// 插入卡带，
	// 将卡带连接到bus和ppu
	this->cart = cartridge;
	ppu.ConnectCartridge(cartridge);
}

void Bus::reset()
{
	cart->reset();
	cpu.reset();
	ppu.reset();
	nSystemClockCounter = 0;
	dma_page = 0x00;
	dma_addr = 0x00;
	dma_data = 0x00;
	dma_dummy = true;
	dma_transfer = false;
}

bool Bus::clock()
{
	// 时钟， 模拟器的核心
	// 整个模拟器的运行"节奏"
	// 其中，ppu时钟通常用来决定整个模拟器速度
	ppu.clock();

	// APU时钟
	apu.clock();

	// CPU的时钟周期比PPU和APU的慢3倍，也就是说，PPU和APU每执行3次，CPU执行一次
	// 这是NES模拟器中规定的
	// 使用nSystemClockCounter来记录ppu的时钟周期. 除3取余即可得到cpu时钟
	if (nSystemClockCounter % 3 == 0)
	{
		// 判断是否发生DMA传送(将OAM数据传送到PPU中)
		if (dma_transfer)
		{
			// TODO 可能存在cpu的周期bug
			// 这里虽然在cpu的周期中，但实际cpu不进行clock，
			// 此时DMA占用cpu周期
			// 通常占用513或514个周期
			// （等待写入完成时为 1 个等待状态周期，如果在奇数 CPU 周期中为 + 1，则为 256 个交替读 / 写周期。）
			// dma_dummy的默认是true
			if (dma_dummy)
			{
				// 执行周期必须是偶数，需要等待一个cpu周期
				if (nSystemClockCounter % 2 == 1)
				{
					// DMA开始传送
					dma_dummy = false;
				}
			}
			else
			{
				if (nSystemClockCounter % 2 == 0)
				{
					// 偶数周期从cpu中读取数据，
					// dma_page存储着 0x(dma_page)xx数据 
					// xx通常从0开始 ，0xFF结束，
					// 64个精灵，每个精灵4个字节，所以是256个字节，也就是0xFF大小
					// dma_page是地址的高位，dma_addr是地址的低位
					// dma_data即为从cpu中读取的数据
					dma_data = cpuRead(dma_page << 8 | dma_addr);
				}
				else
				{
					// 奇数周期，将数据写到PPU中
					ppu.pOAM[dma_addr] = dma_data;
					// 需要增加dma的地址偏移
					dma_addr++;
					// dma_addr是一个字节的数据
					// 当dma_addr大于0xFF时，会产生溢出
					// dma_addr = 0 说明此时DMA传送停止
					// 将标志寄存器更改
					if (dma_addr == 0x00)
					{
						dma_transfer = false;
						dma_dummy = true;
					}
				}
			}
		}
		else
		{
			// 没有发生DMA传送时，CPU时钟正常进行。
			cpu.clock();
		}
	}

	// 音频同步
	bool bAudioSampleReady = false;
	// dAudioTimePerNESClock = 1.0 / 5369318.0; //
	dAudioTime += dAudioTimePerNESClock;
	if (dAudioTime >= dAudioTimePerSystemSample)
	{
		// dAudioTimePerSystemSample = 1.0 / (double)sample_rate;
		// 当运行的时间大于一次采样周期的时候，进行一次发声。。。。这个速度是极快的
		dAudioTime -= dAudioTimePerSystemSample;
		// 获取需要发出的声音信息
		dAudioSample = apu.GetOutputSample();
		bAudioSampleReady = true;
	}
	if (ppu.nmi)
	{
		// PPU的中断通常发生在vertical blanking period，垂直消隐期间
		// 也就是scanline大于251?? 的时候，属于在屏幕的下方，
		// 此时cpu需要进行下一帧的名称表的准备
		// 不可能在绘制的时间进行准备，只能等到绘制结束，否则会产生很多问题
		// 唯一的机会就是在垂直消隐期间进行，也是nes设计巧妙之处
		ppu.nmi = false;
		cpu.nmi();
	}
	nSystemClockCounter++;
	// 如果准备好声音数据，开始发声。。
	return bAudioSampleReady;
}
