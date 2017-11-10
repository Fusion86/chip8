#pragma once

#include <stdint.h>

#include "../../chip8gl/include/imgui_applog.h"

#define PC_START 0x200
#define FONT_OFFSET 0

#define ADD_LOG(fmt, ...)                   \
    if (AsmLog != nullptr)                  \
    {                                       \
        AsmLog->AddLog(fmt, ##__VA_ARGS__); \
    }

namespace CHIP8
{
    namespace Emulator
    {
        class CHIP8Emulator
        {
          public:
            bool IsInitialized;

            uint16_t Opcode = 0;
            uint8_t Memory[4096] = {0};
            uint8_t V[16] = {0};

            uint16_t I = 0;
            uint16_t Pc = 0;

            uint16_t Stack[16] = {0};
            uint8_t Sp = 0;

            uint8_t Display[64 * 32] = {0};

            uint8_t DelayTimer = 0;
            uint8_t SoundTimer = 0;

          private:
            AppLog *AsmLog = nullptr;

          public:
            CHIP8Emulator();

            int Initialize();
            int LoadGame(uint8_t *buffer, uint32_t buffer_size);
            int LoadFont();
            int EmulateCycle();

            int SetAsmLog(AppLog *ptr);
        };
    }
}