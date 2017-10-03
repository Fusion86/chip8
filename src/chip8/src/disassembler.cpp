#include "chip8/disassembler.h"

#include <string.h>

#define COMMENT_PADDING_WIDTH 30

namespace CHIP8
{
    namespace Disassembler
    {
        void Disassemble(FILE* output, uint8_t* rom, uint32_t size)
        {
            uint16_t opcode;
            char asm_str[64];
            int16_t padding;

            for (int i = 0; i < size; i += 2) {
                opcode = rom[i] << 8 | rom[i + 1];
                OpcodeToAsmString(opcode, asm_str, sizeof(asm_str));
                padding = COMMENT_PADDING_WIDTH - (int)strlen(asm_str);
                if (padding < 0) padding = 0;
                fprintf(output, "%s %*s; %04X\n", asm_str, padding, "", opcode);
            }
        }

        void OpcodeToAsmString(uint16_t opcode, char* buffer, uint32_t buffer_size)
        {
            switch (opcode & 0xF000) {
            case 0x0000:
                switch (opcode) {
                case 0x00E0:
                    snprintf(buffer, buffer_size, "CLS");
                    break;
                case 0x00EE:
                    snprintf(buffer, buffer_size, "RET");
                    break;
                default:
                    snprintf(buffer, buffer_size, "SYS addr");
                    break;
                }
            case 0x1000:
                snprintf(buffer, buffer_size, "JP %X", NNN);
                break;
            case 0x2000:
                snprintf(buffer, buffer_size, "CALL %X", NNN);
                break;
            case 0x3000:
                snprintf(buffer, buffer_size, "SE V%X, %X", X, KK);
                break;
            case 0x4000:
                snprintf(buffer, buffer_size, "SNE V%X, %X", X, KK);
                break;
            case 0x5000:
                snprintf(buffer, buffer_size, "SE V%X, V%X", X, Y);
                break;
            case 0x6000:
                snprintf(buffer, buffer_size, "LD V%X, %X", X, KK);
                break;
            case 0x7000:
                snprintf(buffer, buffer_size, "ADD V%X, %X", X, KK);
                break;
            default:
                snprintf(buffer, buffer_size, "UNKNOWN");
                break;
            }
        }
    }
}