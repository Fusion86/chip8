#include <string.h>
#include <stdio.h>

#include <3ds.h>

#include <chip8/chip8.h>
#include <chip8/emulator.h>

static CHIP8::Emulator::CHIP8Emulator *chip = new CHIP8::Emulator::CHIP8Emulator();

int main(int argc, char **argv)
{
	gfxInitDefault();
	consoleInit(GFX_BOTTOM, NULL);

	printf("Lucina v%s\n", VERSION);

	while (aptMainLoop())
	{
		hidScanInput();

		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break;

		gfxFlushBuffers();
		gfxSwapBuffers();

		gspWaitForVBlank();
	}

	gfxExit();
	return 0;
}
