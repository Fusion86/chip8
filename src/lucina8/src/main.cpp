#include <stdio.h>
#include <string.h>

#include <3ds.h>

#include <chip8/chip8.h>
#include <chip8/emulator.h>

static CHIP8::Emulator::CHIP8Emulator *chip = new CHIP8::Emulator::CHIP8Emulator();

static uint32_t kDown, kHeld, pixel;
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
            for (int y = 0; y < 32; y++)
            {
                for (int x = 0; x < 64; x++)
                {
                    pixel = chip->Display[y * 64 + x] == 1 ? 0xFF : 0x00;
                    setPixel(fb, x + 1, y + 1, pixel, pixel, pixel);
                }
            }
            chip->DrawFlag = false;
            printf("Frame: %lu    Cycles: %lu\n", frame_count++, cycles);
            cycles = 0;

            gfxFlushBuffers();
            gfxSwapBuffers();

            gspWaitForVBlank();
        }
    }

    gfxExit();
    return 0;
}
