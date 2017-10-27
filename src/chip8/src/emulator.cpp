#include "chip8/emulator.h"

#include <string.h>

namespace CHIP8
{
    namespace Emulator
    {
        CHIP8Emulator::CHIP8Emulator()
        {
            IsInitialized = false;
            AsmLog = nullptr;
        }

        int CHIP8Emulator::Initialize()
        {
            IsInitialized = true;

            return 0;
        }

        int CHIP8Emulator::LoadGame(uint8_t* buffer, uint32_t buffer_size)
        {
            AsmLog->AddLog("[info] Loading rom...");
            memcpy((void *)(Memory + PC_START), buffer, buffer_size);

            AsmLog->AddLog(" OK (size %d)\n", buffer_size);

            Pc = PC_START;
            AsmLog->AddLog("[info] Set pc to 0x%04X (%d)\n", Pc, Pc);

            return 0;
        }

        int CHIP8Emulator::EmulateCycle()
        {
            Opcode = Memory[Pc] << 8 | Memory[Pc + 1];

            AsmLog->AddLog("[debug] Opcode: %04X\n", Opcode);

            return 0;
        }

        int CHIP8Emulator::SetAsmLog(AppLog *ptr)
        {
            this->AsmLog = ptr;
            return 0;
        }
    }
}