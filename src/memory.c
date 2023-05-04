#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "memory.h"

struct Chip8 chip8_new(void) {
    return (struct Chip8) {
        .memory = {0},
        .stack = {0},
        .registers = {0},
        .keypad = {0},
        .video = {0},
        .index = 0,
        .pc = 0,
        .sound_timer = 0,
        .delay_timer = 0,
        .sp = 0,
        .inst = 0,
    };
} 

void chip8_load_rom(struct Chip8 *chip8, const char *restrict filename) {
    FILE *rom = fopen(filename, "r");
    if (!rom) {
        fputs("Error: Could not open ROM.", stderr);
        exit(1);
    }

    fseek(rom, 0, SEEK_END);
    const int rom_size = ftell(rom);
    fseek(rom, 0, SEEK_SET);

    if (rom_size > MEMORYSIZ - INSTADDR) {
        fputs("Error: ROM is bigger than the total memory size.", stderr);
        exit(1);
    }

    char byte = fgetc(rom);
    for (int i = 0; i <  MEMORYSIZ - INSTADDR && byte != EOF; i++) {
        chip8->memory[i] = byte;
        byte = fgetc(rom);
    }
}
