#include <stdio.h>
#include <string.h>

#include <3ds.h>

#include <chip8/chip8.h>
#include <chip8/emulator.h>

#define SCREEN_SCALE 2

static int KeyMap[16] = {
    0, // 0
    KEY_DUP, // 1
    0, // 2
    0, // 3
    KEY_DDOWN, // 4
    0, // 5
    0, // 6
    0, // 7
    0, // 8
    0, // 9
    0, // A
    0, // B
    0, // C
    0, // D
    0, // E
    0, // F
};

static CHIP8::Emulator::CHIP8Emulator *chip = new CHIP8::Emulator::CHIP8Emulator();

static uint32_t kDown, kHeld;
static uint32_t frame_count = 0;
static uint32_t cycles = 0;
static bool paused = true;

void setPixel(u8 *fb, u16 x, u16 y, u8 red, u8 green, u8 blue)
{
    fb[3 * (240 - y + (x - 1) * 240)] = blue;
    fb[3 * (240 - y + (x - 1) * 240) + 1] = green;
    fb[3 * (240 - y + (x - 1) * 240) + 2] = red;
}

int main(int argc, char **argv)
{
    gfxInitDefault();
    consoleInit(GFX_BOTTOM, NULL);

    gfxSetDoubleBuffering(GFX_TOP, false);
    uint8_t *fb = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);

    printf("Lucina v%s\n", lucina8_VERSION);

    //
    // Rom load stuff
    //

    FILE *file = fopen("roms/PONG", "rb");
    if (file == NULL)
    {
        printf("Couldn't find sdmc://roms/PONG\n");
        printf("Press start to exit...\n");

        while (aptMainLoop())
        {
            hidScanInput();
            kDown = hidKeysDown();

            if (kDown & KEY_START) return 0;
        }
    }

    //
    // Emulator stuff
    //

    chip->Initialize();

    chip->SetLogWriteCallback([](const char *str) {
        printf("%s", str);
    });

    chip->SetIsKeyDownCallback([](uint8_t keycode) {
        return KeyMap[keycode] & kDown || KeyMap[keycode] & kHeld; // FIXME:
    });

    uint8_t *buffer;
    size_t buffer_size;

    CHIP8::ReadFileIntoBuffer(file, &buffer, &buffer_size);
    chip->LoadGame(buffer, buffer_size);

    fclose(file);
    free(buffer);

    printf("Starting emulation!\n");

    while (aptMainLoop())
    {
        hidScanInput();
        kDown = hidKeysDown();
        kHeld = hidKeysHeld();

        if (kDown & KEY_B) paused = !paused;
        if (kDown & KEY_START) return 0;

        if (!paused)
        {
            chip->EmulateCycleStep();

            // Make accurate
            chip->DelayTimer--;
            chip->SoundTimer--;
            cycles++;
        }

        if (chip->DrawFlag == true)
        {
            uint8_t pixel;

            for (int y = 0; y < 32; y++)
            {
                for (int x = 0; x < 64; x++)
                {
                    pixel = chip->Display[y * 64 + x] == 1 ? 0xFF : 0;
                    setPixel(fb, x + 1, y + 1, pixel, pixel, pixel);
                }
            }
            chip->DrawFlag = false;
            // printf("Frame: %lu    Cycles: %lu\n", frame_count++, cycles);
            cycles = 0;

            gfxFlushBuffers();
            gfxSwapBuffers();

            gspWaitForVBlank();
        }
    }

    gfxExit();
    return 0;
}
