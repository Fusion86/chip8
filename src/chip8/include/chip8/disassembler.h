#pragma once

#include <vector>

#include <stdint.h>

#include "chip8/chip8.h"

namespace CHIP8
{
    namespace Disassembler
    {
        void Disassemble(FILE *output, uint8_t *rom, uint32_t size, std::vector<uint16_t> *missing_opcodes = nullptr);

        int OpcodeToAsmString(uint16_t opcode, char *buffer, uint32_t buffer_size);
    } // namespace Disassembler
} // namespace CHIP8
