#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "cpu.h"
#include "opcode.h"

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
        .pc = INSTADDR,
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

    fread(chip8->memory + INSTADDR, MEMORYSIZ - INSTADDR, rom_size, rom);
}

uint64_t end_time = 0;
uint64_t start_500hz = 0;
uint64_t start_60hz = 0;

void chip8_cycle(struct Chip8 *chip8) {
    end_time = SDL_GetPerformanceCounter();
    float delta_inst = (end_time - start_500hz) / (float)SDL_GetPerformanceFrequency() * 1000.0;
    float delta_timer = (end_time - start_60hz) / (float)SDL_GetPerformanceFrequency() * 1000.0;

    // timers are run at 60hz
    if (delta_timer > 1000/60.0) {
        start_60hz = end_time;
        if (chip8->sound_timer > 0) {
            --chip8->sound_timer;
        }

        if (chip8->delay_timer > 0) {
            --chip8->delay_timer;
        }
    }

    // the instructions are run at 500hz
    if (delta_inst > 1000/500.0) {
        start_500hz = end_time;
        chip8->inst = (chip8->memory[chip8->pc] >> 8) | chip8->memory[chip8->pc + 1];
        chip8->pc += 2;

        // parses and runs instructions
        switch (chip8->inst & 0xF000) {
            case 0x0000:
                switch (chip8->inst) {
                    case 0x00E0:
                        chip8_op_00e0(chip8);
                    break;
                    case 0x00EE:
                        chip8_op_00ee(chip8);
                    break;
                }
            break;
            case 0x1000:
                chip8_op_1nnn(chip8);
            break;
            case 0x2000:
                chip8_op_2nnn(chip8);
            break;
            case 0x3000:
                chip8_op_3xkk(chip8);
            break;
            case 0x4000:
                chip8_op_4xkk(chip8);
            break;
            case 0x5000:
                chip8_op_5xy0(chip8);
            break;
            case 0x6000:
                chip8_op_6xkk(chip8);
            break;
            case 0x7000:
                chip8_op_7xkk(chip8);
            break;
            case 0x8000:
                switch (chip8->inst & 0x000F) {
                    case 0x0000:
                        chip8_op_8xy0(chip8);
                    break;
                    case 0x0001:
                        chip8_op_8xy1(chip8);
                    break;
                    case 0x0002:
                        chip8_op_8xy2(chip8);
                    break;
                    case 0x0003:
                        chip8_op_8xy3(chip8);
                    break;
                    case 0x0004:
                        chip8_op_8xy4(chip8);
                    break;
                    case 0x0005:
                        chip8_op_8xy5(chip8);
                    break;
                    case 0x0006:
                        chip8_op_8xy6(chip8);
                    break;
                    case 0x0007:
                        chip8_op_8xy7(chip8);
                    break;
                    case 0x000E:
                        chip8_op_8xye(chip8);
                    break;
                }
            break;
            case 0x9000:
                chip8_op_9xy0(chip8);
            break;
            case 0xA000:
                chip8_op_annn(chip8);
            break;
            case 0xB000:
                chip8_op_bnnn(chip8);
            break;
            case 0xC000:
                chip8_op_cxkk(chip8);
            break;
            case 0xD000:
                chip8_op_dxyn(chip8);
            break;
            case 0xE000:
                switch (chip8->inst & 0x00FF) {
                    case 0x00A1:
                        chip8_op_exa1(chip8);
                    break;
                    case 0x009E:
                        chip8_op_ex9e(chip8);
                    break;
                }
            break;
            case 0xF000:
                switch (chip8->inst & 0x00FF) {
                    case 0x0007:
                        chip8_op_fx07(chip8);
                    break;
                    case 0x000A:
                        chip8_op_fx0a(chip8);
                    break;
                    case 0x0015:
                        chip8_op_fx15(chip8);
                    break;
                    case 0x0018:
                        chip8_op_fx18(chip8);
                    break;
                    case 0x001E:
                        chip8_op_fx1e(chip8);
                    break;
                    case 0x0029:
                        chip8_op_fx29(chip8);
                    break;
                    case 0x0033:
                        chip8_op_fx33(chip8);
                    break;
                    case 0x0055:
                        chip8_op_fx55(chip8);
                    break;
                    case 0x0065:
                        chip8_op_fx65(chip8);
                    break;
                }
            break;
        }
    }
}
