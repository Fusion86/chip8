#include "chip8/emulator.h"

namespace CHIP8
{
namespace Emulator
{
    int CHIP8Emulator::Initialize()
    {
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