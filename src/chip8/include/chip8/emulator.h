#pragma once

#include <stdint.h>

namespace CHIP8
{
namespace Emulator
{
    class CHIP8Emulator
    {
      public:
        bool IsInitialized;

        uint16_t Opcode;
        uint8_t Memory[4096];
        uint8_t V[16];

        uint8_t I;
        uint8_t Pc;

        uint16_t Stack[16];
        uint16_t Sp;

        uint8_t Display[64 * 32];

        uint8_t DelayTimer;
        uint8_t SoundTimer;

        // uint8_t Key[16];

        CHIP8Emulator();

        int Initialize();
        int LoadGame();
        int EmulateCycle();
    };
}
}