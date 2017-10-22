#include "chip8/emulator.h"

namespace CHIP8
{
    namespace Emulator
    {
        CHIP8Emulator::CHIP8Emulator()
        {
            this->IsInitialized = false;
        }

        int CHIP8Emulator::Initialize()
        {
            this->IsInitialized = true;

            return 0;
        }

        int CHIP8Emulator::LoadGame()
        {
            return 0;
        }

        int CHIP8Emulator::EmulateCycle()
        {
            Opcode = Memory[Pc] << 8 | Memory[Pc + 1];

            return 0;
        }
    }
}