#pragma once

#include <stdint.h>
#include <stdio.h>

#define X ((opcode >> 8) & 0x0F)
#define Y ((opcode >> 4) & 0x0F)
#define N (opcode & 0x0F)
#define NNN (opcode & 0x0FFF)
#define KK (opcode & 0xFF)

namespace CHIP8
{
    int ReadFileIntoBuffer(FILE *file, uint8_t **buffer, off_t *size);
}
