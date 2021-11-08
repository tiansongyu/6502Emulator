#include "Mapper_001.h"

Mapper_001::Mapper_001(uint8_t prgBanks, uint8_t chrBanks) : Mapper(prgBanks, chrBanks)
{
	vRAMStatic.resize(32 * 1024);
}

Mapper_001::~Mapper_001()
{
}

bool Mapper_001::cpuMapRead(uint16_t addr, uint32_t &mapped_addr, uint8_t &data)
{
	if (addr >= 0x6000 && addr <= 0x7FFF)
	{
		// 0x6000 -> 0x7FFF 是mapper中的ram
		// Read is from static ram on cartridge
		mapped_addr = 0xFFFFFFFF;

		// Read data from RAM
		data = vRAMStatic[addr & 0x1FFF];

		// Signal mapper has handled request
		return true;
	}

	if (addr >= 0x8000)
	{
		if (nControlRegister & 0b01000)
		{
			// 16K Mode
			if (addr >= 0x8000 && addr <= 0xBFFF)
			{
				mapped_addr = nPRGBankSelect16Lo * 0x4000 + (addr & 0x3FFF);
				return true;
			}

			if (addr >= 0xC000 && addr <= 0xFFFF)
			{
				mapped_addr = nPRGBankSelect16Hi * 0x4000 + (addr & 0x3FFF);
				return true;
			}
		}
		else
		{
			// 32K Mode
			mapped_addr = nPRGBankSelect32 * 0x8000 + (addr & 0x7FFF);
			return true;
		}
	}

	return false;
}

bool Mapper_001::cpuMapWrite(uint16_t addr, uint32_t &mapped_addr, uint8_t data)
{
	if (addr >= 0x6000 && addr <= 0x7FFF)
	{
		// Write is to static ram on cartridge
		mapped_addr = 0xFFFFFFFF;

		// Write data to RAM
		vRAMStatic[addr & 0x1FFF] = data;

		// Signal mapper has handled request
		return true;
	}

	if (addr >= 0x8000)
	{
		// 与几乎所有其他映射器不同，MMC1 通过串行端口进行配置以减少引脚数。
		// CPU $8000 - $FFFF 连接到一个公共移位寄存器。将位 7 设置（$80 到 $FF）的值写入 $8000 - $FFFF 中的任何地址，
		// 会将移位寄存器清除到其初始状态。要更改寄存器的值，CPU 会在第 7 位清除并在第 0 位中写入所需值的位进行五次写入。
		// 在前四次写入时，MMC1 将位 0 ​​移入移位寄存器。在第五次写入时，MMC1 将第 0 位和移位寄存器的内容复制到由地址的第 14 和 13 位选择的内部寄存器中，
		// 然后清除移位寄存器。只有在第五次写入时地址才重要，即使如此，地址的第 14 位和第 13 位也很重要，
		// 因为映射寄存器未完全解码，如PPU 寄存器。第 5 次写入后，移位寄存器会自动清零，因此不需要写入第 7 位的移位寄存器来重置它。

		// 当 CPU 在连续周期写入串行端口时，MMC1 将忽略除第一个之外的所有写入。当 6502 执行读 -
		// 修改 - 写(RMW) 指令时会发生这种情况，例如 DEC 和 ROR，通过写回旧值然后在下一个周期写入新值。至少Bill &Ted's Excellence Adventure通过在包含 $FF 的 ROM 位置执行 INC 来重置 MMC1；MMC1 看到写回的 $FF 并忽略在下一个周期写入的 $00。[1] 这样做的原因是 MMC1 具有显式逻辑来忽略另一个写周期之后的任何写周期。写入的位置无关紧要，例如，即使在写入 $7fff 之后的一个周期内写入 $8000 也会被 MMC1 忽略，实际上，6502 处理器无法做到这一点。 if (data & 0x80) // 0b10000000
		if (data & 0x80)
		{
			// MSB is set, so reset serial loading
			// 7 位 0
			// ---- ----
			// Rxxx xxxD
			// |       |
			// |       +- 要移入移位寄存器的数据位，LSB 在前
			// +--------- 1：复位移位寄存器并用（Control OR $0C）写控制，
			//               将 PRG ROM 锁定在 $C000-$FFFF 到最后一个银行。
			nLoadRegister = 0x00;
			nLoadRegisterCount = 0;
			nControlRegister = nControlRegister | 0x0C;
		}
		else
		{
			// Load data in serially into load register
			// It arrives LSB first, so implant this at
			// bit 5. After 5 writes, the register is ready

			// ;
			// ;
			// Sets the switchable PRG ROM bank to the value of A.;
			// ;
			// A MMC1_SR MMC1_PB
			// 	setPRGBank:;
			// 000edcba 10000 Start with an empty shift register(SR).The 1 is used
			// 	sta $E000;
			// 000edcba->a1000 to detect when the SR has become full.lsr a;
			// > 0000edcb a1000
			// 		sta $E000;
			// 0000edcb->ba100
			// 	lsr a;
			// > 00000edc ba100
			// 		sta $E000;
			// 00000edc->cba10
			// 	lsr a;
			// > 000000ed cba10
			// 		sta $E000;
			// 000000ed->dcba1 Once a 1 is shifted into the last position, the SR is full.lsr a;
			// > 0000000e dcba1
			// 		sta $E000;
			// 0000000e dcba1->edcba A write with the SR full copies D0 and the SR to a bank register;
			// 10000($E000 - $FFFF means PRG bank number) and then clears the SR.rts
			
			nLoadRegister >>= 1;
			nLoadRegister |= (data & 0x01) << 4;
			nLoadRegisterCount++;

			if (nLoadRegisterCount == 5)
			{
				// Get Mapper Target Register, by examining
				// bits 13 & 14 of the address
				uint8_t nTargetRegister = (addr >> 13) & 0x03;

				if (nTargetRegister == 0) // 0x8000 - 0x9FFF
				{
					// Set Control Register
					nControlRegister = nLoadRegister & 0x1F;

					switch (nControlRegister & 0x03)
					{
					case 0:
						mirrormode = ONESCREEN_LO;
						break;
					case 1:
						mirrormode = ONESCREEN_HI;
						break;
					case 2:
						mirrormode = VERTICAL;
						break;
					case 3:
						mirrormode = HORIZONTAL;
						break;
					}
				}
				else if (nTargetRegister == 1) // 0xA000 - 0xBFFF
				{
					// Set CHR Bank Lo
					if (nControlRegister & 0b10000)
					{
						// 4K CHR Bank at PPU 0x0000
						nCHRBankSelect4Lo = nLoadRegister & 0x1F;
					}
					else
					{
						// 8K CHR Bank at PPU 0x0000
						nCHRBankSelect8 = nLoadRegister & 0x1E;
					}
				}
				else if (nTargetRegister == 2) // 0xC000 - 0xDFFF
				{
					// Set CHR Bank Hi
					if (nControlRegister & 0b10000)
					{
						// 4K CHR Bank at PPU 0x1000
						nCHRBankSelect4Hi = nLoadRegister & 0x1F;
					}
				}
				else if (nTargetRegister == 3) // 0xE000 - 0xFFFF
				{
					// Configure PRG Banks
					uint8_t nPRGMode = (nControlRegister >> 2) & 0x03;

					if (nPRGMode == 0 || nPRGMode == 1)
					{
						// Set 32K PRG Bank at CPU 0x8000
						nPRGBankSelect32 = (nLoadRegister & 0x0E) >> 1;
					}
					else if (nPRGMode == 2)
					{
						// Fix 16KB PRG Bank at CPU 0x8000 to First Bank
						nPRGBankSelect16Lo = 0;
						// Set 16KB PRG Bank at CPU 0xC000
						nPRGBankSelect16Hi = nLoadRegister & 0x0F; // ?????
					}
					else if (nPRGMode == 3)
					{
						// Set 16KB PRG Bank at CPU 0x8000
						nPRGBankSelect16Lo = nLoadRegister & 0x0F;  // ?????
						// Fix 16KB PRG Bank at CPU 0xC000 to Last Bank
						nPRGBankSelect16Hi = nPRGBanks - 1;
					}
				}

				// 5 bits were written, and decoded, so
				// reset load register
				nLoadRegister = 0x00;
				nLoadRegisterCount = 0;
			}
		}
	}

	// Mapper has handled write, but do not update ROMs
	return false;
}

bool Mapper_001::ppuMapRead(uint16_t addr, uint32_t &mapped_addr)
{
	if (addr < 0x2000)
	{
		if (nCHRBanks == 0)
		{
			mapped_addr = addr;
			return true;
		}
		else
		{
			if (nControlRegister & 0b10000)
			{
				// 4K CHR Bank Mode
				if (addr >= 0x0000 && addr <= 0x0FFF)
				{
					mapped_addr = nCHRBankSelect4Lo * 0x1000 + (addr & 0x0FFF);
					return true;
				}

				if (addr >= 0x1000 && addr <= 0x1FFF)
				{
					mapped_addr = nCHRBankSelect4Hi * 0x1000 + (addr & 0x0FFF);
					return true;
				}
			}
			else
			{
				// 8K CHR Bank Mode
				mapped_addr = nCHRBankSelect8 * 0x2000 + (addr & 0x1FFF);
				return true;
			}
		}
	}

	return false;
}

bool Mapper_001::ppuMapWrite(uint16_t addr, uint32_t &mapped_addr)
{
	if (addr < 0x2000)
	{
		if (nCHRBanks == 0)
		{
			mapped_addr = addr;
			return true;
		}

		return true;
	}
	else
		return false;
}

void Mapper_001::reset()
{
	nControlRegister = 0x1C;
	nLoadRegister = 0x00;
	nLoadRegisterCount = 0x00;

	nCHRBankSelect4Lo = 0;
	nCHRBankSelect4Hi = 0;
	nCHRBankSelect8 = 0;

	nPRGBankSelect32 = 0;
	nPRGBankSelect16Lo = 0;
	nPRGBankSelect16Hi = nPRGBanks - 1;
}

MIRROR Mapper_001::mirror()
{

	return mirrormode;
}