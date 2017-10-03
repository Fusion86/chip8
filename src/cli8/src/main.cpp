#include <string>

#include "stdio.h"

#include "chip8/chip8.h"
#include "chip8/disassembler.h"

using namespace std;

int main(int argc, char** argv)
{
    printf("cli8 v%s\n", cli8_VERSION);

    if (argc < 2) {
        printf("No mode specified!\n");
        return 1;
    }

    string opt = string(argv[1]);

    if (opt == "disassemble") {
        puts("Selected mode: disassemble");

        if (argc < 3) {
            puts("No binary file given!");
            return 1;
        }

        FILE* file = fopen(argv[2], "rb");

        if (file == NULL) {
            puts("File not found!");
            return 1;
        }

        uint8_t* buffer; // Remember to free()
        uint32_t buffer_size;
        CHIP8::LoadRom(file, &buffer, &buffer_size);
        printf("Loaded: %s\nROM size: %u bytes\n", argv[2], buffer_size);

        CHIP8::Disassembler::Disassemble(stdout, buffer, buffer_size);

        return 0;
    } else {
        printf("Unknown mode: %s\n", opt.c_str());
        return 1;
    }

    return 0;
}