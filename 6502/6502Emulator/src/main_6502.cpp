#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "olcPGEX_Sound.h"
#include "main_6502.h"
#include "Bus.h"
class Emulator6502 : public olc::PixelGameEngine
{
public:
	Bus nes;
	Emulator6502()
	{
		sAppName = "6502Emulator";
	}
public:
	bool OnUserCreate() override
	{
		nes.cpu.Reset();
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::DARK_BLUE);
		//cpu RUN


		//Draw Everything
		DrawCPU(510, 2);

		return true;
	}
private:

	void DrawCPU(int x,int y)
	{
		std::string status = "STATUS: ";
		DrawString(x, y, "STATUS:", olc::WHITE);
		DrawString(x + 64, y, "N", nes.cpu.ps & NegativeFlagBit? olc::GREEN : olc::RED);
		DrawString(x + 80, y, "V", nes.cpu.ps& OverFlowFlagBit ? olc::GREEN : olc::RED);
		DrawString(x + 96, y, "-", nes.cpu.ps& UnusedFlagBit ? olc::GREEN : olc::RED);
		DrawString(x + 112, y, "B",nes.cpu.ps & BreakFlagBit ? olc::GREEN : olc::RED);
		DrawString(x + 128, y, "D",nes.cpu.ps & DecimalMode ? olc::GREEN : olc::RED);
		DrawString(x + 144, y, "I",nes.cpu.ps & InterruptDisableFlagBit ? olc::GREEN : olc::RED);
		DrawString(x + 160, y, "Z",nes.cpu.ps & ZeroBit ? olc::GREEN : olc::RED);
		DrawString(x + 178, y, "C",nes.cpu.ps & CarryFlag ? olc::GREEN : olc::RED);
		DrawString(x, y + 10, "PC: $" + hex(nes.cpu.PC, 4));
		DrawString(x, y + 20, "A: $" + hex(nes.cpu.A, 2) + "  [" + std::to_string(nes.cpu.A) + "]");
		DrawString(x, y + 30, "X: $" + hex(nes.cpu.X, 2) + "  [" + std::to_string(nes.cpu.X) + "]");
		DrawString(x, y + 40, "Y: $" + hex(nes.cpu.Y, 2) + "  [" + std::to_string(nes.cpu.Y) + "]");
		DrawString(x, y + 50, "Stack P: $" + hex(nes.cpu.SP+0x100, 4));
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