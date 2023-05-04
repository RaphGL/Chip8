#include "memory.h"
#include <string.h>
#include <stdlib.h>

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

// SE Vx, Vy
// Skips next instruction if Vx = Vy
// Increments PC by 2 if equal
void chip8_op_5xy0(struct Chip8 *chip8) {
    uint8_t Vx = (chip8->inst & 0x0F00) >> 8;
    uint8_t Vy = chip8->inst & 0x00F0 >> 4;

    if (chip8->registers[Vx] == chip8->registers[Vy]) {
        chip8->pc += 2;
    }
}

// LD Vx, byte
// Set Vx = kk
void chip8_op_6xkk(struct Chip8 *chip8) {
    uint8_t Vx = (chip8->inst & 0x0F00) >> 8;
    uint8_t kk = chip8->inst & 0x00FF;

    chip8->registers[Vx] = kk;
}

// ADD Vx, byte
// Set Vx = Vx + kk
void chip8_op_7xkk(struct Chip8 *chip8) {
    uint8_t Vx = (chip8->inst & 0x0F00) >> 8;
    uint8_t kk = chip8->inst & 0x00FF;

    chip8->registers[Vx] += kk;
}


// LD Vx, Vy
// Set Vx = Vy
void chip8_op_8xy0(struct Chip8 *chip8) {
    uint8_t Vx = (chip8->inst & 0x0F00) >> 8;
    uint8_t Vy = (chip8->inst & 0x00F0) >> 4;

    chip8->registers[Vx] = chip8->registers[Vy];
}

// OR Vx, Vy
// Set Vx = Vx | Vy
void chip8_op_8xy1(struct Chip8 *chip8) {
    uint8_t Vx = (chip8->inst & 0x0F00) >> 8;
    uint8_t Vy = (chip8->inst & 0x00F0) >> 4;

    chip8->registers[Vx] |= chip8->registers[Vy];
}

// AND Vx, Vy 
// Set Vx = Vx & Vy
void chip8_op_8xy2(struct Chip8 *chip8) {
    uint8_t Vx = (chip8->inst & 0x0F00) >> 8;
    uint8_t Vy = (chip8->inst & 0x00F0) >> 4;

    chip8->registers[Vx] &= chip8->registers[Vy];
}

// XOR Vx, Vy
// Set Vx = Vx ^ Vy
void chip8_op_8xy3(struct Chip8 *chip8) {
    uint8_t Vx = (chip8->inst & 0x0F00) >> 8;
    uint8_t Vy = (chip8->inst & 0x00F0) >> 4;

    chip8->registers[Vx] ^= chip8->registers[Vy];
}

// ADD Vx, Vy
// Set Vx = Vx & Vy, VF = carry
// The values of Vx and Vy are added together. 
// If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0.
// Only the lowest 8 bits of the result are kept, and stored in Vx.
void chip8_op_8xy4(struct Chip8 *chip8) {
    uint8_t Vx = (chip8->inst & 0x0F00) >> 8;
    uint8_t Vy = (chip8->inst & 0x00F0) >> 4;

    int sum = chip8->registers[Vx] + chip8->registers[Vy];
    if (sum > 255) {
        chip8->registers[VF] = 1;
    } else {
        chip8->registers[VF] = 0;
    }

    chip8->registers[Vx] = sum & 0xFF;
}

// SUB Vx, Vy
// Set Vx = Vx - Vy
// If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.
void chip8_op_8xy5(struct Chip8 *chip8) {
    uint8_t Vx = (chip8->inst & 0x0F00) >> 8;
    uint8_t Vy = (chip8->inst & 0x00F0) >> 4;

    if (chip8->registers[Vx] > chip8->registers[Vy]) {
        chip8->registers[VF] = 1;
    } else {
        chip8->registers[VF] = 0;
    }

    chip8->registers[Vx] -= chip8->registers[Vy];
}

// SHR Vx, {, Vy}
// Set Vx = Vx >> 1.
//If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.
void chip8_op_8xy6(struct Chip8 *chip8) {
    uint8_t Vx = (chip8->inst & 0x0F00) >> 8;
    uint8_t Vy = (chip8->inst & 0x00F0) >> 4;

    if (chip8->registers[Vx] & 1) {
        chip8->registers[VF] = 1;
    } else {
        chip8->registers[VF] = 0;
    }

    chip8->registers[Vx] >>= 1;
}

// SUBN Vx, Vy
// Set Vx = Vy - Vx, set VF = NOT borrow.
// If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.
void chip8_op_8xy7(struct Chip8 *chip8) {
    uint8_t Vx = (chip8->inst & 0x0F00) >> 8;
    uint8_t Vy = (chip8->inst & 0x00F0) >> 4;

    if (chip8->registers[Vx] > chip8->registers[Vy]) {
        chip8->registers[VF] = 1;
    } else {
        chip8->registers[VF] = 0;
    }

    chip8->registers[Vx] -= chip8->registers[Vy];
}

// SHL Vx {, Vy}
// Set Vx = Vx SHL 1.
// If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
void chip8_op_8xye(struct Chip8 *chip8) {
    uint8_t Vx = (chip8->inst & 0x0F00) >> 8;
    uint8_t Vy = (chip8->inst & 0x00F0) >> 4;

    if (chip8->registers[Vx] & 1) {
        chip8->registers[VF] = 1;
    } else {
        chip8->registers[VF] = 0;
    }

    chip8->registers[Vx] <<= 1;
}

// SNE Vx, Vy
// Skip next instruction if Vx != Vy.
// The values of Vx and Vy are compared, and if they are not equal, the program counter is increased by 2.
void chip8_op_9xy0(struct Chip8 *chip8) {
    uint8_t Vx = (chip8->inst & 0x0F00) >> 8;
    uint8_t Vy = (chip8->inst & 0x00F0) >> 4;

    if (chip8->registers[Vx] !=  chip8->registers[Vy]) {
        chip8->pc += 2;
    }
}

// LD I, addr
// Set I = nnn.
// The value of register I is set to nnn 
void chip8_op_annn(struct Chip8 *chip8) {
    uint8_t nnn = chip8->inst & 0x0FFF;
    chip8->index = nnn;
}

// JP V0, addr
// Jump to location nnn + V0.
// The program counter is set to nnn plus the value of V0.
void chip8_op_bnnn(struct Chip8 *chip8) {
    uint8_t nnn = chip8->inst & 0x0FFF;
    chip8->pc = nnn + chip8->registers[V0];
}

// RND Vx, byte
// Set Vx = random byte AND kk.
// The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk.
// The results are stored in Vx. See instruction 8xy2 for more information on AND.
void chip8_op_cxkk(struct Chip8 *chip8) {
    uint8_t Vx = (chip8->inst & 0x0F00) >> 8;
    uint8_t kk = chip8->inst & 0x00FF;

    int random = (rand() % 255) & kk;
    chip8->registers[Vx] = random;
}
