#pragma once

#include <stdint.h>

#include "../../chip8gl/include/imgui_applog.h"

#define PC_START 0x200

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

            uint16_t I;
            uint16_t Pc;

            uint16_t Stack[16];
            uint16_t Sp;

            uint8_t Display[64 * 32];

            uint8_t DelayTimer;
            uint8_t SoundTimer;

          private:
            AppLog *AsmLog;

          public:

            CHIP8Emulator();

            int Initialize();
            int LoadGame(uint8_t* buffer, uint32_t buffer_size);
            int EmulateCycle();

            int SetAsmLog(AppLog *ptr);
        };
    }
}