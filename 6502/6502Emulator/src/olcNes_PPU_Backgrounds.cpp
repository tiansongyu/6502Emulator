// Copyright (C) 2020-2022 tiansongyu
//
// This file is part of 6502Emulator.
//
// 6502Emulator is free GameEngine: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// 6502Emulator is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with 6502Emulator.  If not, see <http://www.gnu.org/licenses/>.
//
// 6502Emulator is actively maintained and developed!
#include <deque>
#include <iostream>
#include <sstream>

#include "Bus.h"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGEX_SOUND
#include "olcPGEX_Sound.h"

class Demo_olcNES : public olc::PixelGameEngine
{
  public:
    Demo_olcNES()
    {
        sAppName = "olcNES Sound Demonstration";
    }

  private:
    // The NES
    Bus nes;
    std::shared_ptr<Cartridge> cart;
    bool bEmulationRun = true;
    float fResidualTime = 0.0f;

    uint8_t nSelectedPalette = 0x00;

    std::list<uint16_t> audio[4];
    float fAccumulatedTime = 0.0f;

  private:
    // Support Utilities
    std::map<uint16_t, std::string> mapAsm;

    std::string hex(uint32_t n, uint8_t d)
    {
        std::string s(d, '0');
        for (int i = d - 1; i >= 0; i--, n >>= 4)
            s[i] = "0123456789ABCDEF"[n & 0xF];
        return s;
    }

    void DrawRam(int x, int y, uint16_t nAddr, int nRows, int nColumns)
    {
        int nRamX = x, nRamY = y;
        for (int row = 0; row < nRows; row++)
        {
            std::string sOffset = "$" + hex(nAddr, 4) + ":";
            for (int col = 0; col < nColumns; col++)
            {
                sOffset += " " + hex(nes.cpuRead(nAddr, true), 2);
                nAddr += 1;
            }
            DrawString(nRamX, nRamY, sOffset);
            nRamY += 10;
        }
    }

    void DrawCpu(int x, int y)
    {
        std::string status = "STATUS: ";
        DrawString(x, y, "STATUS:", olc::WHITE);
        DrawString(x + 64, y, "N", nes.cpu.status & Nes6502::N ? olc::GREEN : olc::RED);
        DrawString(x + 80, y, "V", nes.cpu.status & Nes6502::V ? olc::GREEN : olc::RED);
        DrawString(x + 96, y, "-", nes.cpu.status & Nes6502::U ? olc::GREEN : olc::RED);
        DrawString(x + 112, y, "B", nes.cpu.status & Nes6502::B ? olc::GREEN : olc::RED);
        DrawString(x + 128, y, "D", nes.cpu.status & Nes6502::D ? olc::GREEN : olc::RED);
        DrawString(x + 144, y, "I", nes.cpu.status & Nes6502::I ? olc::GREEN : olc::RED);
        DrawString(x + 160, y, "Z", nes.cpu.status & Nes6502::Z ? olc::GREEN : olc::RED);
        DrawString(x + 178, y, "C", nes.cpu.status & Nes6502::C ? olc::GREEN : olc::RED);
        DrawString(x, y + 10, "PC: $" + hex(nes.cpu.pc, 4));
        DrawString(x, y + 20, "A: $" + hex(nes.cpu.a, 2) + "  [" + std::to_string(nes.cpu.a) + "]");
        DrawString(x, y + 30, "X: $" + hex(nes.cpu.x, 2) + "  [" + std::to_string(nes.cpu.x) + "]");
        DrawString(x, y + 40, "Y: $" + hex(nes.cpu.y, 2) + "  [" + std::to_string(nes.cpu.y) + "]");
        DrawString(x, y + 50, "Stack P: $" + hex(nes.cpu.stkp, 4));
    }

    void DrawCode(int x, int y, int nLines)
    {
        auto it_a = mapAsm.find(nes.cpu.pc);
        int nLineY = (nLines >> 1) * 10 + y;
        if (it_a != mapAsm.end())
        {
            DrawString(x, nLineY, (*it_a).second, olc::CYAN);
            while (nLineY < (nLines * 10) + y)
            {
                nLineY += 10;
                if (++it_a != mapAsm.end())
                {
                    DrawString(x, nLineY, (*it_a).second);
                }
            }
        }

        it_a = mapAsm.find(nes.cpu.pc);
        nLineY = (nLines >> 1) * 10 + y;
        if (it_a != mapAsm.end())
        {
            while (nLineY > y)
            {
                nLineY -= 10;
                if (--it_a != mapAsm.end())
                {
                    DrawString(x, nLineY, (*it_a).second);
                }
            }
        }
    }

    void DrawAudio(int channel, int x, int y)
    {
        FillRect(x, y, 120, 120, olc::BLACK);
        int i = 0;
        for (auto s : audio[channel])
        {
            Draw(x + i, y + (s >> (channel == 2 ? 5 : 4)), olc::YELLOW);
            i++;
        }
    }

    // This function is called by the underlying sound hardware
    // which runs in a different thread. It is automatically
    // synchronised with the sample rate of the sound card, and
    // expects a single "sample" to be returned, whcih ultimately
    // makes its way to your speakers, and then your ears, for that
    // lovely 8-bit bliss... but, that means we've some thread
    // handling to deal with, since we want both the PGE thread
    // and the sound system thread to interact with the emulator.

    static Demo_olcNES *pInstance; // Static variable that will hold a pointer to "this"

    static float SoundOut(int nChannel, float fGlobalTime, float fTimeStep)
    {
        if (nChannel == 0)
        {
            while (!pInstance->nes.clock())
            {
            }
            return static_cast<float>(pInstance->nes.dAudioSample); // dAudioSample是最后的播放数据
        }
        else
            return 0.0f;
    }

    bool OnUserCreate() override
    {
        // Load the cartridge
        cart = std::make_shared<Cartridge>("./rom/smb.nes");

        if (!cart->ImageValid())
            return false;

        // Insert into NES
        nes.insertCartridge(cart);

        // Extract dissassembly
        // mapAsm = nes.cpu.disassemble(0x0000, 0xFFFF);

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 120; j++)
                audio[i].push_back(0);
        }

        // Reset NES
        nes.reset();

        // Initialise PGEX sound system, and give it a function to
        // call which returns a sound sample on demand
        pInstance = this;
        nes.SetSampleFrequency(44100);
        olc::SOUND::InitialiseAudio(44100, 1, 8, 512);
        olc::SOUND::SetUserSynthFunction(SoundOut);
        return true;
    }

    // We must play nicely now with the sound hardware, so unload
    // it when the application terminates
    bool OnUserDestroy() override
    {
        olc::SOUND::DestroyAudio();
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
#ifdef __APPLE__
        {
            EmulatorUpdateWithoutAudio(fElapsedTime);
        }
#else
        {
            EmulatorUpdateWithAudio(fElapsedTime);
        }
#endif
        return true;
    }

    // This performs an emulation update but synced to audio, so it cant
    // perform stepping through code or frames. Essentially, it runs
    // the emulation in real time now, so only accepts "controller" input
    // and updates the display
    bool EmulatorUpdateWithAudio(float fElapsedTime)
    {
        // Sample audio channel output roughly once per frame
        fAccumulatedTime += fElapsedTime;
        if (fAccumulatedTime >= 1.0f / 60.0f)
        {
            fAccumulatedTime -= (1.0f / 60.0f);
            audio[0].pop_front();
            audio[0].push_back(nes.apu.pulse1_visual); // 可视化音频
            audio[1].pop_front();
            audio[1].push_back(nes.apu.pulse2_visual);
            audio[2].pop_front();
            audio[2].push_back(nes.apu.noise_visual);
        }

        Clear(olc::DARK_BLUE);

        // Handle input for controller in port #1
        nes.controller[0] = 0x00;
        nes.controller[0] |= GetKey(olc::Key::K).bHeld ? 0x80 : 0x00; // A Button
        nes.controller[0] |= GetKey(olc::Key::J).bHeld ? 0x40 : 0x00; // B Button
        nes.controller[0] |= GetKey(olc::Key::Y).bHeld ? 0x20 : 0x00; // Select
        nes.controller[0] |= GetKey(olc::Key::T).bHeld ? 0x10 : 0x00; // Start
        nes.controller[0] |= GetKey(olc::Key::W).bHeld ? 0x08 : 0x00;
        nes.controller[0] |= GetKey(olc::Key::S).bHeld ? 0x04 : 0x00;
        nes.controller[0] |= GetKey(olc::Key::A).bHeld ? 0x02 : 0x00;
        nes.controller[0] |= GetKey(olc::Key::D).bHeld ? 0x01 : 0x00;

        if (GetKey(olc::Key::BACK).bPressed)
            nes.reset();
        if (GetKey(olc::Key::P).bPressed)
            (++nSelectedPalette) &= 0x07;

        // 存储游戏状态
        if (GetKey(olc::Key::F1).bPressed)
        {
            std::ofstream ofs;
            ofs.open("./save_0.dat", std::ofstream::binary);
            ofs.write(reinterpret_cast<char *>(&nes), sizeof(nes));
            ofs.close();
        }

        // 还原游戏状态
        if (GetKey(olc::Key::F2).bPressed)
        {
            std::ifstream ifs;
            ifs.open("./save_0.dat", std::ofstream::binary);
            ifs.read(reinterpret_cast<char *>(&nes), sizeof(nes));
            ifs.close();
        }

        DrawCpu(516, 2);
        // DrawCode(516, 72, 26);

        // Draw OAM Contents (first 26 out of 64)
        // ======================================
        for (int i = 0; i < 26; i++)
        {
            std::string s = hex(i, 2) + ": (" + std::to_string(nes.ppu.pOAM[i * 4 + 3]) + ", " +
                            std::to_string(nes.ppu.pOAM[i * 4 + 0]) + ") " + "ID: " + hex(nes.ppu.pOAM[i * 4 + 1], 2) +
                            +" AT: " + hex(nes.ppu.pOAM[i * 4 + 2], 2);
            DrawString(516, 72 + i * 10, s);
            // DrawSprite(516 + 231, 72 + i * 10, &nes.ppu.GetSpriteTitle(516 +
            // 231, 72 + i * 10, nes.ppu.pOAM[i * 4 + 1], nes.ppu.pOAM[i * 4 +
            // 2], nSelectedPalette, i));
        }

        // Draw AUDIO Channels
        // DrawAudio(0, 520, 72);
        // DrawAudio(1, 644, 72);
        // DrawAudio(2, 520, 196);
        // DrawAudio(3, 644, 196);

        // Draw Palettes & Pattern Tables
        // ==============================================
        const int nSwatchSize = 6;
        for (int p = 0; p < 8; p++)     // For each palette
            for (int s = 0; s < 4; s++) // For each index
                FillRect(516 + p * (nSwatchSize * 5) + s * nSwatchSize, 340, nSwatchSize, nSwatchSize,
                         nes.ppu.GetColourFromPaletteRam(p, s));

        // Draw selection reticule around selected palette
        DrawRect(516 + nSelectedPalette * (nSwatchSize * 5) - 1, 339, (nSwatchSize * 4), nSwatchSize, olc::WHITE);

        // Generate Pattern Tables
        DrawSprite(516, 348, &nes.ppu.GetPatternTable(0, nSelectedPalette));
        DrawSprite(648, 348, &nes.ppu.GetPatternTable(1, nSelectedPalette));

        // Draw rendered output
        // ========================================================
        DrawSprite(0, 0, &nes.ppu.GetScreen(), 2);
        return true;
    }

    // This performs emulation with no audio synchronisation, so it is just
    // as before, in all the previous videos
    bool EmulatorUpdateWithoutAudio(float fElapsedTime)
    {
        Clear(olc::DARK_BLUE);

        // Handle input for controller in port #1
        nes.controller[0] = 0x00;
        nes.controller[0] |= GetKey(olc::Key::K).bHeld ? 0x80 : 0x00; // A Button
        nes.controller[0] |= GetKey(olc::Key::J).bHeld ? 0x40 : 0x00; // B Button
        nes.controller[0] |= GetKey(olc::Key::Y).bHeld ? 0x20 : 0x00; // Select
        nes.controller[0] |= GetKey(olc::Key::T).bHeld ? 0x10 : 0x00; // Start
        nes.controller[0] |= GetKey(olc::Key::W).bHeld ? 0x08 : 0x00;
        nes.controller[0] |= GetKey(olc::Key::S).bHeld ? 0x04 : 0x00;
        nes.controller[0] |= GetKey(olc::Key::A).bHeld ? 0x02 : 0x00;
        nes.controller[0] |= GetKey(olc::Key::D).bHeld ? 0x01 : 0x00;

        if (GetKey(olc::Key::SPACE).bPressed)
            bEmulationRun = !bEmulationRun;
        if (GetKey(olc::Key::BACK).bPressed)
            nes.reset();
        if (GetKey(olc::Key::P).bPressed)
            (++nSelectedPalette) &= 0x07;

        if (bEmulationRun)
        {
            if (fResidualTime > 0.0f)
                fResidualTime -= fElapsedTime;
            else
            {
                fResidualTime += (1.0f / 60.0f) - fElapsedTime;
                do
                {
                    nes.clock();
                } while (!nes.ppu.frame_complete);
                nes.ppu.frame_complete = false;
            }
        }
        else
        {
            // Emulate code step-by-step
            if (GetKey(olc::Key::C).bPressed)
            {
                // Clock enough times to execute a whole CPU instruction
                do
                {
                    nes.clock();
                } while (!nes.cpu.complete());
                // CPU clock runs slower than system clock, so it may be
                // complete for additional system clock cycles. Drain
                // those out
                do
                {
                    nes.clock();
                } while (nes.cpu.complete());
            }

            // Emulate one whole frame
            if (GetKey(olc::Key::F).bPressed)
            {
                // Clock enough times to draw a single frame
                do
                {
                    nes.clock();
                } while (!nes.ppu.frame_complete);
                // Use residual clock cycles to complete current instruction
                do
                {
                    nes.clock();
                } while (!nes.cpu.complete());
                // Reset frame completion flag
                nes.ppu.frame_complete = false;
            }
            // 存储游戏状态
            if (GetKey(olc::Key::Z).bPressed)
            {
                std::ofstream ofs;
                ofs.open("./save_0.dat", std::ofstream::binary);
                ofs.write(reinterpret_cast<char *>(&nes), sizeof(nes));
                ofs.close();
            }
        }

        DrawCpu(516, 2);
        // DrawCode(516, 72, 26);

        // Draw OAM Contents (first 26 out of 64)
        // ======================================
        for (int i = 0; i < 26; i++)
        {
            std::string s = hex(i, 2) + ": (" + std::to_string(nes.ppu.pOAM[i * 4 + 3]) + ", " +
                            std::to_string(nes.ppu.pOAM[i * 4 + 0]) + ") " + "ID: " + hex(nes.ppu.pOAM[i * 4 + 1], 2) +
                            +" AT: " + hex(nes.ppu.pOAM[i * 4 + 2], 2);
            DrawString(516, 72 + i * 10, s);
        }

        // Draw Palettes & Pattern Tables
        // ==============================================
        const int nSwatchSize = 6;
        for (int p = 0; p < 8; p++)     // For each palette
            for (int s = 0; s < 4; s++) // For each index
                FillRect(516 + p * (nSwatchSize * 5) + s * nSwatchSize, 340, nSwatchSize, nSwatchSize,
                         nes.ppu.GetColourFromPaletteRam(p, s));

        // Draw selection reticule around selected palette
        DrawRect(516 + nSelectedPalette * (nSwatchSize * 5) - 1, 339, (nSwatchSize * 4), nSwatchSize, olc::WHITE);

        // Generate Pattern Tables
        DrawSprite(516, 348, &nes.ppu.GetPatternTable(0, nSelectedPalette));
        DrawSprite(648, 348, &nes.ppu.GetPatternTable(1, nSelectedPalette));

        // Draw rendered output
        // ========================================================
        DrawSprite(0, 0, &nes.ppu.GetScreen(), 2);
        return true;
    }
};

// Provide implementation for our static pointer
Demo_olcNES *Demo_olcNES::pInstance = nullptr;

int main()
{
    Demo_olcNES demo;
    demo.Construct(780, 480, 2, 2);
    demo.Start();
    return 0;
}
