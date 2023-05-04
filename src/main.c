#include "memory.h"
#include <stdio.h>

int main(void)
{
    struct Chip8 chip8 = chip8_new();
    chip8_load_rom(&chip8, "1-chip8-logo.ch8");
    fgetc(stdin);
    return 0;
}
