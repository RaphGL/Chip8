#include "memory.h"
#include <string.h>

// CLS
// Clear the display
void chip8_op_00e0(struct Chip8 *chip8) {
    memset(chip8->video, 0, sizeof(chip8->video));
}

// RET
// Return from a subroutine
// PC is set to top of stack and 1 is substracted from SP
void chip8_op_00ee(struct Chip8 *chip8) {
    chip8->pc = chip8->stack[chip8->sp];
    --chip8->sp;
}

// JP addr
// Jump to location nnn
// Sets PC to nnn
void chip8_op_1nnn(struct Chip8 *chip8) {
    int8_t addr = chip8->inst & 0x0FFF;
    chip8->pc = addr;
}

// CALL addr
// Call subroutine at nnn
// Increments SP then puts current PC on top of stack, then PC is set to nnn
void chip8_op_2nnn(struct Chip8 *chip8) {
    ++chip8->sp;
    chip8->stack[chip8->sp] = chip8->pc;
    chip8->pc = chip8->inst & 0x0FFF;
}

// SE Vx, byte
// Skip next instruction if Vx = kk
// Increments PC by 2 if equal
void chip8_op_3xkk(struct Chip8 *chip8) {
    uint8_t Vx = (chip8->inst & 0x0F00) >> 8;
    uint8_t kk = chip8->inst & 0x00FF;

    if (chip8->registers[Vx] == kk) {
        chip8->pc += 2;
    }
}

// SNE Vx, byte
// Skips next instruction if Vx != kk
// Increments PC by 2 if not equal
void chip8_op_4xkk(struct Chip8 *chip8) {
    uint8_t Vx = (chip8->inst & 0x0F00) >> 8;
    uint8_t kk = chip8->inst & 0x00FF;

    if (chip8->registers[Vx] != kk) {
        chip8->pc += 2;
    }
}
