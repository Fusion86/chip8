#include <chip8/disassembler.h>
#include <chip8/chip8_opcode_helper.h>

#include <string.h>

#define COMMENT_PADDING_WIDTH 24

namespace CHIP8
{
    namespace Disassembler
    {
        void Disassemble(FILE *output, uint8_t *rom, uint32_t size, std::vector<uint16_t> *missing_opcodes)
        {
            uint16_t opcode;
            char asm_str[64];
            int padding;
            int opcode_to_asm_result;

            for (int i = 0; i < size; i += 2)
            {
                opcode = rom[i] << 8 | rom[i + 1];
                opcode_to_asm_result = OpcodeToAsmString(opcode, asm_str, sizeof(asm_str));
                padding = COMMENT_PADDING_WIDTH - (int)strlen(asm_str);
                if (padding < 0) padding = 0;
                fprintf(output, "%s %*s; %X (%i)    op: %04X\n", asm_str, padding, "", i, i, opcode);

                if (missing_opcodes != nullptr && opcode_to_asm_result == 1)
                {
                    // Check if the opcode is not in the list
                    if (std::find(missing_opcodes->begin(), missing_opcodes->end(), opcode) == missing_opcodes->end())
                    {
                        missing_opcodes->push_back(opcode);
                    }
                }
            }
        }

        int OpcodeToAsmString(uint16_t opcode, char *buffer, uint32_t buffer_size)
        {
            switch (opcode & 0xF000)
            {
            case 0x0000:
                switch (opcode)
                {
                case 0x00E0:
                    snprintf(buffer, buffer_size, "CLS");
                    return 0;
                case 0x00EE:
                    snprintf(buffer, buffer_size, "RET");
                    return 0;
                default:
                    snprintf(buffer, buffer_size, "SYS %X", NNN);
                    return 0;
                }
                return 0;
            case 0x1000:
                snprintf(buffer, buffer_size, "JP %X", NNN);
                return 0;
            case 0x2000:
                snprintf(buffer, buffer_size, "CALL %X", NNN);
                return 0;
            case 0x3000:
                snprintf(buffer, buffer_size, "SE v%X, %X", X, KK);
                return 0;
            case 0x4000:
                snprintf(buffer, buffer_size, "SNE v%X, %X", X, KK);
                return 0;
            case 0x5000:
                snprintf(buffer, buffer_size, "SE v%X, v%X", X, Y);
                return 0;
            case 0x6000:
                snprintf(buffer, buffer_size, "LD v%X, %i", X, KK);
                return 0;
            case 0x7000:
                snprintf(buffer, buffer_size, "ADD v%X, %X", X, KK);
                return 0;
            case 0x8000:
                switch (N)
                {
                case 0x0:
                    snprintf(buffer, buffer_size, "LD v%X, v%X", X, Y);
                    return 0;
                case 0x1:
                    snprintf(buffer, buffer_size, "OR v%X, v%X", X, Y);
                    return 0;
                case 0x2:
                    snprintf(buffer, buffer_size, "AND v%X, v%X", X, Y);
                    return 0;
                case 0x3:
                    snprintf(buffer, buffer_size, "XOR v%X, v%X", X, Y);
                    return 0;
                case 0x4:
                    snprintf(buffer, buffer_size, "ADD v%X, v%X", X, Y);
                    return 0;
                case 0x5:
                    snprintf(buffer, buffer_size, "SUB v%X, v%X", X, Y);
                    return 0;
                case 0x6:
                    snprintf(buffer, buffer_size, "SHR v%X {, v%X}", X, Y); // TODO: Check if correct
                    return 0;
                case 0x7:
                    snprintf(buffer, buffer_size, "SUBN v%X, v%X", X, Y);
                    return 0;
                case 0xE:
                    snprintf(buffer, buffer_size, "SHL v%X {, v%X}", X, Y); // TODO: Check if correct
                    return 0;
                default: goto unknown_opcode;
                }
                return 0;
            case 0xA000:
                snprintf(buffer, buffer_size, "LD I, %X", NNN);
                return 0;
            case 0xC000:
                snprintf(buffer, buffer_size, "RND v%X, %X", X, KK);
                return 0;
            case 0xD000:
                snprintf(buffer, buffer_size, "DRW v%X, v%X, %i", X, Y, N);
                return 0;
            case 0xE000:
                switch (KK)
                {
                case 0x9E:
                    snprintf(buffer, buffer_size, "SKP v%X", X);
                    return 0;
                case 0xA1:
                    snprintf(buffer, buffer_size, "SKNP v%X", X);
                    return 0;
                default: goto unknown_opcode;
                }
                return 0;
            case 0xF000:
                switch (KK)
                {
                case 0x07:
                    snprintf(buffer, buffer_size, "LD v%X, DT", X);
                    return 0;
                case 0x0A:
                    snprintf(buffer, buffer_size, "LD v%X, K", X);
                    return 0;
                case 0x15:
                    snprintf(buffer, buffer_size, "LD DT, v%X", X);
                    return 0;
                case 0x18:
                    snprintf(buffer, buffer_size, "LD ST, v%X", X);
                    return 0;
                case 0x1E:
                    snprintf(buffer, buffer_size, "ADD I, v%X", X);
                    return 0;
                case 0x29:
                    snprintf(buffer, buffer_size, "LD F, v%X", X);
                    return 0;
                case 0x33:
                    snprintf(buffer, buffer_size, "LD B, v%X", X);
                    return 0;
                case 0x55:
                    snprintf(buffer, buffer_size, "LD [I], v%X", X);
                    return 0;
                case 0x65:
                    snprintf(buffer, buffer_size, "LD v%X, [I]", X);
                    return 0;
                default: goto unknown_opcode;
                }
                return 0;
            default:
            unknown_opcode:
                snprintf(buffer, buffer_size, "UNKNOWN");
                return 1;
            }
        }
    } // namespace Disassembler
} // namespace CHIP8
