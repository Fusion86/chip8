#include <chip8/emulator.h>

#include <chip8/disassembler.h>
#include <chip8/font.h>
#include <chip8/chip8_opcode_helper.h>

#include <chrono>
#include <math.h>
#include <string.h>
#include <thread>

namespace CHIP8
{
    namespace Emulator
    {
        CHIP8Emulator::CHIP8Emulator()
        {
            Initialize();
#ifndef _3DS
            thread_main = std::thread(&CHIP8Emulator::RunMain, this);
            thread_timers = std::thread(&CHIP8Emulator::RunTimers, this);
#endif
        }

        CHIP8Emulator::~CHIP8Emulator()
        {
            ShutdownRequested = true;

#ifndef _3DS
            thread_main.join();
            thread_timers.join();
#endif
        }

        int CHIP8Emulator::Initialize(bool load_font)
        {
            LOG_INFO("[info] [chip8emulator] Initializing emulator...\n");

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

            if (load_font)
            {
                LoadFont();
            }

            HasGameLoaded = false;
            IsInitialized = true;
            IsRunning = false;
            ShutdownRequested = false;

            DrawFlag = false;

            LOG_INFO("[info] [chip8emulator] Initialized emulator.\n");

            return 0;
        }

        int CHIP8Emulator::LoadGame(uint8_t *buffer, size_t buffer_size)
        {
            if (HasGameLoaded)
            {
                Initialize(); // Reset if game is loaded (a reset is not needed if no game/rom is loaded)
            }

            LOG_INFO("[info] [loadgame] Loading rom...");
            memcpy((void *)(Memory + PC_START), buffer, buffer_size);
            LOG_INFO(" OK (size %d)\n", buffer_size);

            Pc = PC_START;
            HasGameLoaded = true;

            LOG_INFO("[info] [loadgame] Set pc to 0x%04X (%d)\n", Pc, Pc);

            return 0;
        }

        int CHIP8Emulator::LoadFont()
        {
            LOG_INFO("[info] [loadfont] Loading font starting at 0x%04X (%d)...", FONT_OFFSET, FONT_OFFSET);
            memcpy(Memory + FONT_OFFSET, &FontSet, sizeof(FontSet));
            LOG_INFO(" OK\n");
            return 0;
        }

        int CHIP8Emulator::EmulateCycleStep()
        {
            if (IsRunning)
            {
                return ERR_ALREADY_RUNNING;
            }

            uint16_t opcode = Memory[Pc] << 8 | Memory[Pc + 1]; // lowercase because our macro function uses 'opcode'
            Opcode = opcode; // Save opcode in class

            //
            // AsmLog
            //

            char asm_str[64];
            CHIP8::Disassembler::OpcodeToAsmString(opcode, asm_str, sizeof(asm_str));

            // LOG_INFO("[debug] [emulatecycle] Opcode: %04X\n", opcode);
            LOG_INFO("[debug] [emulatecycle] [disassembler] %s\n", asm_str);

            //
            // Emulate
            //

            return EmulateCycle(opcode);
        }

        int CHIP8Emulator::EmulateCycle(uint16_t opcode)
        {
            switch (opcode & 0xF000)
            {
            case 0x0000:
                switch (opcode)
                {
                case 0x00E0:
                    memset(Display, 0, sizeof(Display));
                    return 0;
                case 0x00EE:
                    Pc = Stack[--Sp] + 2;
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
                Stack[Sp] = Pc;
                Sp++;
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
                V[X] += KK;
                Pc += 2;
                return 0;
            case 0x8000:
                switch (N)
                {
                case 0x0:
                    V[X] = V[Y];
                    Pc += 2;
                    return 0;
                case 0x1:
                    V[X] |= V[Y];
                    Pc += 2;
                    return 0;
                case 0x2:
                    V[X] &= V[Y];
                    Pc += 2;
                    return 0;
                case 0x3:
                    V[X] ^= V[Y];
                    Pc += 2;
                    return 0;
                case 0x4:
                    V[0xF] = ((uint16_t)V[X] + (uint16_t)V[Y] > 256) ? 1 : 0;
                    V[X] += V[Y];
                    Pc += 2;
                    return 0;
                case 0x5:
                    V[0xF] = (V[X] > V[Y]) ? 1 : 0;
                    V[X] -= V[Y];
                    Pc += 2;
                    return 0;
                case 0x6:
                    // snprintf(buffer, buffer_size, "SHR v%X {, v%X}", X, Y); // TODO: Check if correct
                    return 0;
                case 0x7:
                    V[0xF] = (V[Y] > V[X]) ? 1 : 0;
                    V[X] = V[Y] - V[X];
                    Pc += 2;
                    return 0;
                case 0xE:
                    // snprintf(buffer, buffer_size, "SHL v%X {, v%X}", X, Y); // TODO: Check if correct
                    return 0;
                default: goto unknown_opcode;
                }
                return 0;
            case 0xA000:
                I = NNN;
                Pc += 2;
                return 0;
            case 0xC000:
                V[X] = rand() & KK;
                Pc += 2;
                return 0;
            case 0xD000:
                uint8_t pixel;

                V[0xF] = 0;
                for (int yline = 0; yline < N; yline++) // N = height
                {
                    pixel = Memory[I + yline];
                    for (int xline = 0; xline < 8; xline++)
                    {
                        if ((pixel & (0x80 >> xline)) != 0) // wtf does this do?
                        {
                            if (Display[V[X] + xline + ((V[Y] + yline) * 64)] == 1)
                            {
                                V[0xF] = 1; // Set VF = 1 because there is a collision
                            }
                            Display[V[X] + xline + ((V[Y] + yline) * 64)] ^= 1; // XOR pixel
                        }
                    }
                }
                DrawFlag = true;
                Pc += 2;
                return 0;
            case 0xE000:
                switch (KK)
                {
                case 0x9E:
                    if (GetKeyDown(V[X]))
                    {
                        Pc += 2;
                    }
                    Pc += 2;
                    return 0;
                case 0xA1:
                    if (GetKeyDown(V[X]) == false)
                    {
                        Pc += 2;
                    }
                    Pc += 2;
                    return 0;
                default: goto unknown_opcode;
                }
                return 0;
            case 0xF000:
                switch (KK)
                {
                case 0x07:
                    V[X] = DelayTimer;
                    Pc += 2;
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
                    I += V[X];
                    Pc += 2;
                    return 0;
                case 0x29:
                    I = FONT_OFFSET + V[X] * 5;
                    Pc += 2;
                    return 0;
                case 0x33:
                    Memory[I] = floor(V[X] / 100);
                    Memory[I + 1] = floor(V[X] / 10 % 10);
                    Memory[I + 2] = V[X] % 100 % 10;
                    Pc += 2;
                    return 0;
                case 0x55:
                    for (int i = 0; i < X; i++)
                    {
                        Memory[I + i] = V[i];
                    }
                    Pc += 2;
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
                LOG_DEBUG("[debug] [emulatecycle] Unknown opcode (%04X)\n", Opcode);
                return ERR_UNKNOWN_OPCODE;
            }
        }

        bool CHIP8Emulator::GetKeyDown(uint8_t keycode)
        {
        }

        void CHIP8Emulator::LogWrite(const char *level, const char *fmt, ...)
        {
        }

        //
        // Thread Code
        //
        // This is not accurate at all, and both threads will also not run in sync (esp. when stopped and resumed)
        //

        int CHIP8Emulator::RunMain()
        {
            if (IsRunning)
            {
                return ERR_ALREADY_RUNNING;
            }

            uint16_t opcode;

            while (!ShutdownRequested)
            {
                if (!IsRunning)
                {
#ifndef _3DS
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
#endif
                    continue;
                }

                opcode = Memory[Pc] << 8 | Memory[Pc + 1]; // lowercase because our macro function uses 'opcode'
                Opcode = opcode; // Save opcode in class

                if (EmulateCycle(opcode) != 0)
                {
                    IsRunning = false;
                }

#ifndef _3DS
                std::this_thread::sleep_for(std::chrono::milliseconds(2));
#endif
                // TODO: This is most likely not accurate
            }

            // TODO: Probably a leak here

            return 0;
        }

        int CHIP8Emulator::RunTimers()
        {
            while (!ShutdownRequested)
            {
                if (!IsRunning)
                {
#ifndef _3DS
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
#endif
                    continue;
                }

                if (DelayTimer > 0)
                    DelayTimer--;

                if (SoundTimer > 0)
                    SoundTimer--;

#ifndef _3DS
                std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));
#endif
                // TODO: This is most likely not accurate
            }

            // TODO: Probably a leak here

            return 0;
        }
    } // namespace Emulator
} // namespace CHIP8
