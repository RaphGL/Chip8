#include <string.h>
#include <stdlib.h>
#include "cpu.h"

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
    uint16_t addr = chip8->inst & 0x0FFF;
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
    uint8_t Vy = (chip8->inst & 0x00F0) >> 4;

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
    // uint8_t Vy = (chip8->inst & 0x00F0) >> 4;

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

    if (chip8->registers[Vy] > chip8->registers[Vx]) {
        chip8->registers[VF] = 1;
    } else {
        chip8->registers[VF] = 0;
    }

    chip8->registers[Vx] = chip8->registers[Vy] - chip8->registers[Vx];
}

// SHL Vx {, Vy}
// Set Vx = Vx SHL 1.
// If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
void chip8_op_8xye(struct Chip8 *chip8) {
    uint8_t Vx = (chip8->inst & 0x0F00) >> 8;
    // uint8_t Vy = (chip8->inst & 0x00F0) >> 4;

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
    uint16_t nnn = chip8->inst & 0x0FFF;
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

// DRW Vx, Vy, nibble
// Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
// The interpreter reads n bytes from memory, starting at the address stored in I.
// These bytes are then displayed as sprites on screen at coordinates (Vx, Vy). Sprites are XORed onto the existing screen.
// If this causes any pixels to be erased, VF is set to 1, otherwise it is set to 0.
// If the sprite is positioned so part of it is outside the coordinates of the display, it wraps around to the opposite side of the screen.
void chip8_op_dxyn(struct Chip8 *chip8) {
    uint8_t Vx = (chip8->inst & 0x0F00) >> 8;
    uint8_t Vy = (chip8->inst & 0x00F0) >> 4;
    uint8_t n = chip8->inst & 0x000F;

    const uint8_t x = chip8->registers[Vx] % VIDEO_W;
    const uint8_t y = chip8->registers[Vy] % VIDEO_H;

    for (size_t row = 0; row < n; row++) {
        uint8_t sprite_byte = chip8->memory[chip8->index + row];
        for (size_t col = 0; col < 8; col++) {
            uint8_t pixel = sprite_byte & (0x80 >> col);
            uint32_t *screen_pixel = &chip8->video[(y + row) * VIDEO_W + (x + col)];

            if (*screen_pixel == 0xFFFFFFFF) {
                chip8->registers[VF] = 1;
            } else {
                chip8->registers[VF] = 0;
            }

            if (pixel) {
                *screen_pixel  ^= 0xFFFFFFFF;
            }
        }
    }
}

// SKP Vx
// Skip next instruction if key with the value of Vx is pressed.
// Checks the keyboard, and if the key corresponding to the value of Vx is currently in the down position, PC is increased by 2.
void chip8_op_ex9e(struct Chip8 *chip8) {
    uint8_t Vx = (chip8->inst & 0x0F00) >> 8;

    if (chip8->keypad[chip8->registers[Vx]]) {
        chip8->pc += 2;
    }
}

// SKNP Vx
// Skip next instruction if key with the value of Vx is not pressed.
// Checks the keyboard, and if the key corresponding to the value of Vx is currently in the up position, PC is increased by 2.
void chip8_op_exa1(struct Chip8 *chip8) {
    uint8_t Vx = (chip8->inst & 0x0F00) >> 8;

    if (!chip8->keypad[chip8->registers[Vx]]) {
        chip8->pc += 2;
    }
}

// LD Vx, DT
// Set Vx = delay timer value.
// The value of DT is placed into Vx.
void chip8_op_fx07(struct Chip8 *chip8) {
    uint8_t Vx = (chip8->inst  & 0x0F00) >> 8;
    chip8->registers[Vx] = chip8->delay_timer;
}

// LD Vx, K
// Wait for a key press, store the value of the key in Vx.
// All execution stops until a key is pressed, then the value of that key is stored in Vx.
void chip8_op_fx0a(struct Chip8 *chip8) {
    uint8_t Vx = (chip8->inst  & 0x0F00) >> 8;

    for (;;) {
        for (int i = 0; i < KEYPADSIZ; i++) {
            if (chip8->keypad[i]) {
                chip8->registers[Vx] = i;
                return;
            }
        }
    }
}

// LD DT, Vx
// Set delay timer = Vx.
// DT is set equal to the value of Vx.
void chip8_op_fx15(struct Chip8 *chip8) {
    uint8_t Vx = (chip8->inst  & 0x0F00) >> 8;
    chip8->delay_timer = chip8->registers[Vx];
}

// LD ST, Vx
// Set sound timer = Vx.
// ST is set equal to the value of Vx.
void chip8_op_fx18(struct Chip8 *chip8) {
    uint8_t Vx = (chip8->inst  & 0x0F00) >> 8;
    chip8->sound_timer = chip8->registers[Vx];
}

// ADD I, Vx
// Set I = I + Vx.
// The values of I and Vx are added, and the results are stored in I.
void chip8_op_fx1e(struct Chip8 *chip8) {
    uint8_t Vx = (chip8->inst  & 0x0F00) >> 8;
    chip8->index += chip8->registers[Vx];
}

// LD F, Vx
// Set I = location of sprite for digit Vx.
// The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx. See section 2.4,
// Display, for more information on the Chip-8 hexadecimal font.
void chip8_op_fx29(struct Chip8 *chip8) {
    uint8_t Vx = (chip8->inst  & 0x0F00) >> 8;
    chip8->index = chip8->registers[Vx] * 5 + FONTADDR;
}

// LD B, Vx
// Store BCD representation of Vx in memory locations I, I+1, and I+2.
// The interpreter takes the decimal value of Vx, and places the hundreds digit in memory at location in I,
// the tens digit at location I+1, and the ones digit at location I+2.
void chip8_op_fx33(struct Chip8 *chip8) {
    uint8_t Vx = (chip8->inst  & 0x0F00) >> 8;
    uint8_t value = chip8->registers[Vx];

    chip8->memory[chip8->index + 2] = value % 10;
    value /= 10;
    chip8->memory[chip8->index + 1] = value % 10;
    value /= 10;
    chip8->memory[chip8->index] = value % 10;
}

// LD [I], Vx
// Store registers V0 through Vx in memory starting at location I.
// The interpreter copies the values of registers V0 through Vx into memory, starting at the address in I.
void chip8_op_fx55(struct Chip8 *chip8) {
    uint8_t Vx = (chip8->inst  & 0x0F00) >> 8;

    for (size_t i = V0; i <= Vx; i++) {
        chip8->memory[chip8->index + i] = chip8->registers[i];
    }
}

// LD Vx, [I]
// Read registers V0 through Vx from memory starting at location I.
// The interpreter reads values from memory starting at location I into registers V0 through Vx.
void chip8_op_fx65(struct Chip8 *chip8) {
    uint8_t Vx = (chip8->inst  & 0x0F00) >> 8;

    for (uint8_t i = V0; i <= Vx; i++) {
        chip8->registers[i] = chip8->memory[chip8->index + i];
    }
}
