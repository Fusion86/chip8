#pragma once

#include <stdint.h>
#include <thread>
#include <functional>

#define ERR_ALREADY_RUNNING 1
#define ERR_UNKNOWN_OPCODE 2

#define PC_START 0x200
#define FONT_OFFSET 0

#define LOG_DEBUG(fmt, ...) \
    if (LogLevel <= LoggingLevel::Debug) LogWrite("debug", fmt, ##__VA_ARGS__);

#define LOG_INFO(fmt, ...) \
    if (LogLevel <= LoggingLevel::Info) LogWrite("info", fmt, ##__VA_ARGS__);

#define LOG_WARNING(fmt, ...) \
    if (LogLevel <= LoggingLevel::Warning) LogWrite("warning", fmt, ##__VA_ARGS__);

#define LOG_ERROR(fmt, ...) \
    if (LogLevel <= LoggingLevel::Error) LogWrite("error", fmt, ##__VA_ARGS__);

#define LOG_WRITE(fmt, ...) LogWrite(NULL, fmt, ##__VA_ARGS__);

namespace CHIP8
{
    namespace Emulator
    {
        enum class LoggingLevel
        {
            Debug,
            Info,
            Warning,
            Error,
        };

        class CHIP8Emulator
        {
          public:
            bool IsInitialized;
            bool HasGameLoaded;
            bool DrawFlag; // If true = require redraw
            bool IsRunning;
            LoggingLevel LogLevel = LoggingLevel::Info;

            uint16_t Opcode;
            uint8_t Memory[4096];
            uint8_t V[16];

            uint16_t I;
            uint16_t Pc;

            uint16_t Stack[16];
            uint8_t Sp;

            uint8_t Display[64 * 32];

            uint8_t DelayTimer;
            uint8_t SoundTimer;

          private:
#ifndef _3DS
            std::thread thread_main;
            std::thread thread_timers;
#endif

          public:
            CHIP8Emulator();
            ~CHIP8Emulator();

            int Initialize(bool load_font = true);
            int LoadGame(uint8_t *buffer, size_t buffer_size);
            int LoadFont();
            int EmulateCycleStep();

            void SetIsKeyDownCallback(std::function<bool(uint8_t keycode)> f);
            void SetLogWriteCallback(std::function<void(const char *str)> f);

          private:
            bool ShutdownRequested;

            std::function<bool(uint8_t keycode)> IsKeyDownCallback;
            std::function<void(const char *str)> LogWriteCallback;

            int EmulateCycle(uint16_t opcode);
            bool IsKeyDown(uint8_t keycode);
            void LogWrite(const char *level, const char *fmt, ...);

#ifndef _3DS
            int RunMain();
            int RunTimers();
#endif
        };
    } // namespace Emulator
} // namespace CHIP8
