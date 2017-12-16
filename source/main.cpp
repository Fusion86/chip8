#include <string.h>
#include <stdio.h>

#include <3ds.h>

#include <chip8/chip8.h>
#include <chip8/emulator.h>

static CHIP8::Emulator::CHIP8Emulator *chip = new CHIP8::Emulator::CHIP8Emulator();

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
	uint8_t* fb = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);

	printf("Lucina v%s\n", VERSION);

	chip->Initialize();

	FILE *file = fopen("roms/PONG", "rb");
	if (file == NULL)
	{
		printf("Couldn't find roms/PONG\n");
		return 1;
	}

	uint8_t *buffer;
	uint32_t buffer_size;

	CHIP8::ReadFileIntoBuffer(file, &buffer, &buffer_size);
	chip->LoadGame(buffer, buffer_size);

	fclose(file);
	free(buffer);

	printf("Starting emulation!\n");

	while (aptMainLoop())
	{
		hidScanInput();
		uint32_t kDown = hidKeysDown();

		chip->EmulateCycleStep();

		if (chip->DrawFlag == true)
		{

		}

		gfxFlushBuffers();
		gfxSwapBuffers();

		gspWaitForVBlank();
	}

	gfxExit();
	return 0;
}
