#include "chip8/disassembler.h"

#include <string.h>

#define COMMENT_PADDING_WIDTH 24

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
                fprintf(output, "%s %*s; %X    op: %04X\n", asm_str, padding, "", i, opcode);
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
                    snprintf(buffer, buffer_size, "SYS %X", NNN);
                    break;
                }
                break;
            case 0x1000:
                snprintf(buffer, buffer_size, "JP %X", NNN);
                break;
            case 0x2000:
                snprintf(buffer, buffer_size, "CALL %X", NNN);
                break;
            case 0x3000:
                snprintf(buffer, buffer_size, "SE v%X, %X", X, KK);
                break;
            case 0x4000:
                snprintf(buffer, buffer_size, "SNE v%X, %X", X, KK);
                break;
            case 0x5000:
                snprintf(buffer, buffer_size, "SE v%X, v%X", X, Y);
                break;
            case 0x6000:
                snprintf(buffer, buffer_size, "LD v%X, %i", X, KK);
                break;
            case 0x7000:
                snprintf(buffer, buffer_size, "ADD v%X, %X", X, KK);
                break;
            case 0x8000:
                switch (N) {
                case 0x0:
                    snprintf(buffer, buffer_size, "LD v%X, v%X, %X", X, Y, KK);
                    break;
                }
                break;
            case 0xA000:
                snprintf(buffer, buffer_size, "LD I, %X", NNN);
                break;
            case 0xC000:
                snprintf(buffer, buffer_size, "RND v%X, %X", X, KK);
                break;
            case 0xD000:
                snprintf(buffer, buffer_size, "DRW v%X, v%X, %i", X, Y, N);
                break;
            case 0xE000:
                switch (KK) {
                case 0xA1:
                    snprintf(buffer, buffer_size, "SKNP v%X", X);
                    break;
                default: goto unknown_opcode; break;
                }
                break;
            case 0xF000:
                switch (KK) {
                case 0x07:
                    snprintf(buffer, buffer_size, "LD v%X, DT", X);
                    break;
                case 0x15:
                    snprintf(buffer, buffer_size, "LD DT, v%X", X);
                    break;
                default: goto unknown_opcode; break;
                }
                break;
            default:
            unknown_opcode:
                snprintf(buffer, buffer_size, "UNKNOWN");
                break;
            }
        }
    }
}