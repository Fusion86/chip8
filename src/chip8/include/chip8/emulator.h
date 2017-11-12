#pragma once

#include <stdint.h>
#include <thread>

#include <GLFW/glfw3.h>

#include "../../chip8gl/include/imgui_applog.h"

#define ERR_ALREADY_RUNNING 1
#define ERR_UNKNOWN_OPCODE 2

#define PC_START 0x200
#define FONT_OFFSET 0

#define ADD_LOG(fmt, ...)                     \
    if (m_AsmLog != nullptr)                  \
    {                                         \
        m_AsmLog->AddLog(fmt, ##__VA_ARGS__); \
    }

namespace CHIP8
{
    namespace Emulator
    {
        class CHIP8Emulator
        {
          public:
            bool IsInitialized;
            bool HasGameLoaded;
            bool DrawFlag; // If true = require redraw
            bool IsRunning;

            uint16_t Opcode = 0;
            uint8_t Memory[4096] = { 0 };
            uint8_t V[16] = { 0 };

            uint16_t I = 0;
            uint16_t Pc = 0;

            uint16_t Stack[16] = { 0 };
            uint8_t Sp = 0;

            uint8_t Display[64 * 32] = { 0 };

            uint8_t DelayTimer = 0;
            uint8_t SoundTimer = 0;

            int KeyMap[16] = {
                GLFW_KEY_X,
                GLFW_KEY_1,
                GLFW_KEY_2,
                GLFW_KEY_3,

                GLFW_KEY_4,
                GLFW_KEY_5,
                GLFW_KEY_6,
                GLFW_KEY_7,

                GLFW_KEY_8,
                GLFW_KEY_9,
                GLFW_KEY_Z,
                GLFW_KEY_C,

                GLFW_KEY_4,
                GLFW_KEY_R,
                GLFW_KEY_F,
                GLFW_KEY_V
            };

          private:
            AppLog *m_AsmLog = nullptr;
            GLFWwindow *m_pWindow = nullptr;
            std::thread thread_main;
            std::thread thread_timers;

          public:
            CHIP8Emulator();
            ~CHIP8Emulator();

            int Initialize(bool load_font = true);
            int LoadGame(uint8_t *buffer, uint32_t buffer_size);
            int LoadFont();
            int EmulateCycleStep();

            bool GetKeyDown(uint8_t keycode);

            int SetAsmLog(AppLog *ptr);
            int SetWindowContext(GLFWwindow *window);

          private:
            bool ShutdownRequested;

            int EmulateCycle(uint16_t opcode);
            int RunMain();
            int RunTimers();
        };
    }
}
