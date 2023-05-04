#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "memory.h"

const uint8_t chip8_font_set[FONTSETSIZ] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

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
