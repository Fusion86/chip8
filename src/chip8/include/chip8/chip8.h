#pragma once

#include <stdint.h>
#include <stdio.h>

namespace CHIP8
{
    int LoadRom(FILE *file, uint8_t **buffer, uint32_t *size);
}
