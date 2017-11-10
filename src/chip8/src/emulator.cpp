#include "chip8/emulator.h"

#include "chip8/disassembler.h"
#include "chip8/font.h"

#include <string.h>
#include <math.h>

namespace CHIP8
{
    namespace Emulator
    {
        CHIP8Emulator::CHIP8Emulator()
        {
            // IsInitialized = false;
            // AsmLog = nullptr;
        }

        int CHIP8Emulator::Initialize()
        {
            Opcode = 0;
            memset(Memory, 0, sizeof(Memory));
            memset(V, 0, sizeof(V));

            I = 0;
            Pc = 0;

            memset(Stack, 0, sizeof(Stack));
            Sp = 0;

            memset(Display, 0, sizeof(Display));

            DelayTimer = 0;
            SoundTimer = 0;

            LoadFont();

            IsInitialized = true;

            return 0;
        }

        int CHIP8Emulator::LoadGame(uint8_t *buffer, uint32_t buffer_size)
        {
            ADD_LOG("[info] [loadgame] Loading rom...");
            memcpy((void *)(Memory + PC_START), buffer, buffer_size);

            ADD_LOG(" OK (size %d)\n", buffer_size);

            Pc = PC_START;
            ADD_LOG("[info] [loadgame] Set pc to 0x%04X (%d)\n", Pc, Pc);

            return 0;
        }

        int CHIP8Emulator::LoadFont() 
        {
            ADD_LOG("[info] [loadfont] Loading font starting at 0x%04X (%d)...", FONT_OFFSET, FONT_OFFSET);
            memcpy(Memory + FONT_OFFSET, &FontSet, sizeof(FontSet));
            return 0;
        }

        int CHIP8Emulator::EmulateCycle()
        {
            uint16_t opcode = Memory[Pc] << 8 | Memory[Pc + 1]; // lowercase because our macro function uses 'opcode'
            Opcode = opcode; // Save opcode in class

            //
            // AsmLog
            //

            char asm_str[64];
            CHIP8::Disassembler::OpcodeToAsmString(opcode, asm_str, sizeof(asm_str));

            ADD_LOG("[debug] [emulatecycle] Opcode: %04X\n", opcode);
            ADD_LOG("[debug] [emulatecycle] [disassembler] %s\n", asm_str);

            //
            // Emulate
            //

            switch (opcode & 0xF000)
            {
            case 0x0000:
                switch (opcode)
                {
                case 0x00E0:
                    // snprintf(buffer, buffer_size, "CLS");
                    return 0;
                case 0x00EE:
                    // snprintf(buffer, buffer_size, "RET");
                    return 0;
                default:
                    // snprintf(buffer, buffer_size, "SYS %X", NNN);
                    return 0;
                }
                return 0;
            case 0x1000:
                Pc = NNN;
                return 0;
            case 0x2000:
                // TODO: Probably broken
                if (Stack[Sp] != 0)
                {
                    Sp++;
                }
                Stack[Sp] = Pc;
                Pc = NNN;
                return 0;
            case 0x3000:
                if (V[X] == KK)
                {
                    Pc += 2;
                }
                Pc += 2;
                return 0;
            case 0x4000:
                if (V[X] != KK)
                {
                    Pc += 2;
                }
                Pc += 2;
                return 0;
            case 0x5000:
                // snprintf(buffer, buffer_size, "SE v%X, v%X", X, Y);
                return 0;
            case 0x6000:
                V[X] = KK;
                Pc += 2;
                return 0;
            case 0x7000:
                // snprintf(buffer, buffer_size, "ADD v%X, %X", X, KK);
                return 0;
            case 0x8000:
                switch (N)
                {
                case 0x0:
                    // snprintf(buffer, buffer_size, "LD v%X, v%X, %X", X, Y, KK);
                    return 0;
                }
                return 0;
            case 0xA000:
                I = NNN;
                Pc += 2;
                return 0;
            case 0xC000:
                // snprintf(buffer, buffer_size, "RND v%X, %X", X, KK);
                return 0;
            case 0xD000:
                // TODO: Skipped
                Pc += 2;
                return 0;
            case 0xE000:
                switch (KK)
                {
                case 0x9E:
                    // snprintf(buffer, buffer_size, "SKP v%X", X);
                    return 0;
                case 0xA1:
                    // snprintf(buffer, buffer_size, "SKNP v%X", X);
                    return 0;
                default: goto unknown_opcode;
                }
                return 0;
            case 0xF000:
                switch (KK)
                {
                case 0x07:
                    // snprintf(buffer, buffer_size, "LD v%X, DT", X);
                    return 0;
                case 0x0A:
                    // snprintf(buffer, buffer_size, "LD v%X, K", X);
                    return 0;
                case 0x15:
                    DelayTimer = V[X];
                    Pc += 2;
                    return 0;
                case 0x18:
                    SoundTimer = V[X];
                    Pc += 2;
                    return 0;
                case 0x1E:
                    // snprintf(buffer, buffer_size, "ADD I, v%X", X);
                    return 0;
                case 0x29:
                    I = FONT_OFFSET + V[X] * 5;
                    Pc += 2;
                    return 0;
                case 0x33:
                    Memory[I] = floor(V[X] / 100);
                    Memory[I+1] = floor(V[X] / 10 % 10);;
                    Memory[I+2] = V[X] % 100 % 10;
                    Pc += 2;
                    return 0;
                case 0x55:
                    // snprintf(buffer, buffer_size, "LD [I], v%X", X);
                    return 0;
                case 0x65:
                    for (int i = 0; i < X; i++) 
                    {
                        V[i] = Memory[I + i];
                    }
                    Pc += 2;
                    return 0;
                default: goto unknown_opcode;
                }
                return 0;
            default:
            unknown_opcode:
                ADD_LOG("[debug] [emulatecycle] Unknown opcode\n");
                return 1;
            }
        }

        int CHIP8Emulator::SetAsmLog(AppLog *ptr)
        {
            this->AsmLog = ptr;
            return 0;
        }
    }
}