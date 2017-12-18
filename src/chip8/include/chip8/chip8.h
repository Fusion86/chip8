#pragma once

#include <stdint.h>
#include <stdio.h>

namespace CHIP8
{
    int ReadFileIntoBuffer(FILE *file, uint8_t **buffer, size_t *size);
}
