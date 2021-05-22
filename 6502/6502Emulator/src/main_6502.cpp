#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "olcPGEX_Sound.h"
#include "main_6502.h"
class Emulator6502 : public olc::PixelGameEngine
{
public:
	m6502::Mem mem;
	m6502::CPU cpu;
	Emulator6502()
	{
		sAppName = "6502Emulator";
	}

public:
	bool OnUserCreate() override
	{
		cpu.Reset(mem);
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::DARK_BLUE);

		DrawCPU(510, 2);

		return true;
	}
private:
	void DrawCPU(int x,int y)
	{
		std::string status = "STATUS: ";
		DrawString(x, y, "STATUS:", olc::WHITE);
		DrawString(x + 64, y, "N", cpu.ps & m6502::NegativeFlagBit? olc::GREEN : olc::RED);
		DrawString(x + 80, y, "V", cpu.ps& m6502::OverFlowFlagBit ? olc::GREEN : olc::RED);
		DrawString(x + 96, y, "-", cpu.ps& m6502::UnusedFlagBit ? olc::GREEN : olc::RED);
		DrawString(x + 112, y, "B",cpu.ps & m6502::BreakFlagBit ? olc::GREEN : olc::RED);
		DrawString(x + 128, y, "D",cpu.ps & m6502::DecimalMode ? olc::GREEN : olc::RED);
		DrawString(x + 144, y, "I",cpu.ps & m6502::InterruptDisableFlagBit ? olc::GREEN : olc::RED);
		DrawString(x + 160, y, "Z",cpu.ps & m6502::ZeroBit ? olc::GREEN : olc::RED);
		DrawString(x + 178, y, "C",cpu.ps & m6502::CarryFlag ? olc::GREEN : olc::RED);
		DrawString(x, y + 10, "PC: $" + hex(cpu.PC, 4));
		DrawString(x, y + 20, "A: $" + hex(cpu.A, 2) + "  [" + std::to_string(cpu.A) + "]");
		DrawString(x, y + 30, "X: $" + hex(cpu.X, 2) + "  [" + std::to_string(cpu.X) + "]");
		DrawString(x, y + 40, "Y: $" + hex(cpu.Y, 2) + "  [" + std::to_string(cpu.Y) + "]");
		DrawString(x, y + 50, "Stack P: $" + hex(cpu.SP+0x100, 4));
	}
	std::string hex(uint32_t Data, uint8_t length)
	{
		std::string result(length, '0');
		for (int i = length - 1; i >= 0; i--, Data >>= 4)
			result[i] = "0123456789ABCDEF"[Data & 0xf];
		return result;
	}
};
int main()
{
	Emulator6502 demo;
	if (demo.Construct(780, 480, 2, 2))
		demo.Start();
	return 0;
}