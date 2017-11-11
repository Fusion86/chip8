#include <stdlib.h>

#include "chip8/chip8.h"

namespace CHIP8
{
    int LoadRom(FILE *file, uint8_t **buffer, uint32_t *size)
    {
		if (file == NULL)
		{
			return 1;
		}

        fseek(file, 0, SEEK_END);
        *size = ftell(file);
        rewind(file);

        *buffer = (uint8_t *)malloc(*size);
        fread(*buffer, *size, 1, file);

		return 0;
    }
}