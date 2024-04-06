#include <stdio.h>
#include <string.h>
#include "cpu.h"
#include "io.h"

void test_instructions(struct Chip8 *chip8);

int main(int argc, char **argv) {
    if (argc == 1) {
        fputs("Error: No ROM was supplied.", stderr);
        return 0;
    }

    struct Chip8 chip8 = chip8_new();
    chip8_load_rom(&chip8, argv[1]);

    #ifndef NDEBUG
    test_instructions(&chip8);
    // resets chip8 state after running tests
    chip8 = chip8_new();
    chip8_load_rom(&chip8, argv[1]);
    #endif

    chip8_init_video(&chip8);
    chip8_init_input(&chip8);
    chip8_init_audio(&chip8);

    while (running) {
        chip8_cycle(&chip8);
        chip8_video_draw(chip8.video);
    }

    chip8_quit_input();
    chip8_quit_audio();
    chip8_quit_video();
    return 0;
}

#ifndef NDEBUG
#include <assert.h>
#include "opcode.h"
void test_instructions(struct Chip8 *chip8) {
    int sp;
    int pc;
    // CLS
    memset(chip8->video, 0xFFFFFFFF, sizeof(chip8->video));
    assert(chip8->video[0] == 0xFFFFFFFF);
    chip8->inst = 0x00E0;
    chip8_op_00e0(chip8);
    assert(chip8->video[0] == 0);

    // RET 
    chip8->sp = 1;
    chip8->stack[1] = 0x200;
    chip8_op_00ee(chip8);
    assert(chip8->pc == 0x200);
    assert(chip8->sp == 0);

    // JP addr
    chip8->inst = 0x1234;
    chip8_op_1nnn(chip8);
    assert(chip8->pc == 0x234);

    // CALL addr
    chip8->inst = 0x2555;
    sp = chip8->sp;
    chip8_op_2nnn(chip8);
    assert(chip8->sp == sp + 1);
    assert(chip8->pc == 0x555);

    // SE Vx, byte
    pc = chip8->pc;
    chip8->registers[V2] = 0x89;
    chip8->inst = 0x3289;
    chip8_op_3xkk(chip8);
    assert(chip8->pc == pc + 2);

    pc = chip8->pc;
    chip8->inst = 0x3211;
    chip8_op_3xkk(chip8);
    assert(chip8->pc == pc);

    // SNE Vx, byte
    pc = chip8->pc;
    chip8->registers[V3] = 0x80;
    chip8->inst = 0x4320;
    chip8_op_4xkk(chip8);
    assert(chip8->pc == pc + 2);

    pc = chip8->pc;
    chip8->inst = 0x4380;
    chip8_op_4xkk(chip8);
    assert(chip8->pc == pc);

    // SE Vx, Vy
    pc = chip8->pc;
    chip8->registers[V2] = 0x89;
    chip8->registers[VA] = 0x89;
    chip8->inst = 0x52A0;
    chip8_op_5xy0(chip8);
    assert(chip8->pc == pc + 2);

    pc = chip8->pc;
    chip8->registers[V2] = 0x89;
    chip8->registers[VA] = 0x20;
    chip8->inst = 0x52A0;
    chip8_op_5xy0(chip8);
    assert(chip8->pc == pc);

    // LD Vx, byte
    chip8->inst = 0x6D99;
    chip8_op_6xkk(chip8);
    assert(chip8->registers[VD] == 0x99);

    // ADD Vx, byte
    chip8->inst = 0x7110;
    chip8->registers[V1] = 0x10;
    chip8_op_7xkk(chip8);
    assert(chip8->registers[V1] == 0x20);

    // LD Vx, Vy
    chip8->registers[VC] = 0x66;
    chip8->inst = 0x87C0;
    chip8_op_8xy0(chip8);
    assert(chip8->registers[7] == 0x66);

    // OR Vx, Vy
    chip8->inst = 0x8EA1;
    chip8->registers[VE] = 0b10101010;
    chip8->registers[VA] = 0b01010101;
    chip8_op_8xy1(chip8);
    assert(chip8->registers[VE] == 0b11111111);

    // AND Vx, Vy 
    chip8->inst = 0x8562;
    chip8->registers[V5] = 0xAF;
    chip8->registers[V6] =  0xF0;
    chip8_op_8xy2(chip8);
    assert(chip8->registers[V5] == 0xA0);

    // XOR Vx, Vy
    chip8->inst = 0x8343;
    chip8->registers[V3] = 0b11110010;
    chip8->registers[V4] = 0b11001101;
    chip8_op_8xy3(chip8);
    assert(chip8->registers[V3] = 0b00111111);


    // ADD Vx, Vy
    chip8->inst = 0x8CD4;
    chip8->registers[VC] = 100;
    chip8->registers[VD] = 150;
    chip8_op_8xy4(chip8);
    assert(chip8->registers[VC] == 250);
    
    // SUB Vx, Vy
    chip8->inst = 0x8AB5;
    chip8->registers[VA] = 0x80;
    chip8->registers[VB] = 0x75;
    chip8_op_8xy5(chip8);
    assert(chip8->registers[VA] == 0xB);
    
    // SHR Vx, {, Vy}
    chip8->inst = 0x8006;
    chip8->registers[V0] = 0xFF;
    chip8_op_8xy6(chip8);
    assert(chip8->registers[V0] == 0x7F);

    // SUBN Vx, Vy
    chip8->inst = 0x8127;
    chip8->registers[V1] = 2;
    chip8->registers[V2] = 4;
    chip8_op_8xy7(chip8);
    assert(chip8->registers[VF] == 1);
    assert(chip8->registers[V1] == 2);

    chip8->registers[V1] = 4;
    chip8->registers[V2] = 2;
    chip8_op_8xy7(chip8);
    assert(chip8->registers[VF] == 0);
    assert(chip8->registers[V1] == 254);

    // SHL Vx {, Vy}
    chip8->inst = 0x890e;
    chip8->registers[V9] = 0xFF;
    chip8_op_8xye(chip8);
    assert(chip8->registers[V9] == 0b11111110);

    // SNE Vx, Vy
    pc = chip8->pc;
    chip8->registers[V3] = 0x80;
    chip8->registers[V4] = 0x81;
    chip8->inst = 0x9340;
    chip8_op_9xy0(chip8);
    assert(chip8->pc == pc + 2);

    pc = chip8->pc;
    chip8->registers[V3] = 0x80;
    chip8->registers[V4] = 0x80;
    chip8->inst = 0x9340;
    chip8_op_9xy0(chip8);
    assert(chip8->pc == pc);

    // LD I, addr
    chip8->inst = 0xA999;
    chip8_op_annn(chip8);
    assert(chip8->index == 0x999);
    
    // JP V0, addr
    chip8->registers[V0] = 0x88;
    chip8->inst = 0xB080;
    chip8_op_bnnn(chip8);
    assert(chip8->pc == 0x108);

    // RND Vx, byte
    chip8->inst = 0xCAFF;
    chip8_op_cxkk(chip8);
    int rnd = chip8->registers[VA];
    chip8_op_cxkk(chip8);
    assert(rnd != chip8->registers[VA]);
    assert(rnd >= 0 && rnd < 256);

    // DRW Vx, Vy, nibble
    chip8->registers[V2] = 2;
    chip8->registers[V3] = 2;
    chip8->inst = 0xD232;
    chip8->index = 0;
    chip8->memory[0] = 0xFF;
    chip8->memory[1] = 0x0F;
    chip8_op_dxyn(chip8);
    for (size_t i = 0; i < 8; i++) {
        assert(chip8->video[2 * VIDEO_W + (i + 2)] == 0xFFFFFFFF);
    }
    for (size_t i = 0; i < 4; i++) {
        assert(chip8->video[3 * VIDEO_W + (i + 2)] == 0);
    }
    for (size_t i = 4; i < 8; i++) {
        assert(chip8->video[3 * VIDEO_W + (i + 2)] == 0xFFFFFFFF);
    }
    
    // SKP Vx
    chip8->keypad[5] = 1;
    chip8->registers[V1] = 5;
    chip8->inst = 0xE19E;
    pc = chip8->pc;
    chip8_op_ex9e(chip8);
    assert(chip8->pc == pc + 2);

    chip8->keypad[5] = 0;
    pc = chip8->pc;
    chip8_op_ex9e(chip8);
    assert(chip8->pc == pc);
    
    // SKNP Vx
    chip8->keypad[5] = 0;
    chip8->registers[V1] = 5;
    chip8->inst = 0xE19E;
    pc = chip8->pc;
    chip8_op_exa1(chip8);
    assert(chip8->pc == pc + 2);

    chip8->keypad[5] = 1;
    pc = chip8->pc;
    chip8_op_exa1(chip8);
    assert(chip8->pc == pc);
    chip8->keypad[5] = 0;

    // LD Vx, DT
    chip8->inst = 0xF207;
    chip8->delay_timer = 255;
    chip8_op_fx07(chip8);
    assert(chip8->registers[V2] == 255);

    // LD Vx, K
    chip8->inst = 0xF80A;
    chip8->keypad[10] = 1;
    chip8_op_fx0a(chip8);
    assert(chip8->registers[V8] == 0xA);
    chip8->keypad[10] = 0;

    // LD DT, Vx
    chip8->inst = 0xF315;
    chip8->registers[V3] = 140;
    chip8_op_fx15(chip8);
    assert(chip8->delay_timer == 140);

    // LD ST, Vx
    chip8->inst = 0xFE18;
    chip8->registers[VE] = 140;
    chip8_op_fx18(chip8);
    assert(chip8->sound_timer == 140);

    // ADD I, Vx
    chip8->inst = 0xF11E;
    chip8->registers[V1] = 11;
    chip8->index = 2;
    chip8_op_fx1e(chip8);
    assert(chip8->index == 13);

    // LD F, Vx
    chip8->inst = 0xFD29;
    chip8->registers[VD] = 0xA;
    chip8_op_fx29(chip8);
    assert(chip8->index == (10 * 5 + FONTADDR));
    
    // LD B, Vx 
    chip8->inst = 0xFE33;
    chip8->registers[VE] = 127;
    chip8_op_fx33(chip8);
    assert(chip8->memory[chip8->index] == 1);
    assert(chip8->memory[chip8->index + 1] == 2);
    assert(chip8->memory[chip8->index + 2] == 7);
   
    // LD [I], Vx
    chip8->index = 0x250;
    chip8->inst = 0xFF55;
    for (int i = V0; i <= VF; i++) {
        chip8->registers[i] = 251;
    }
    chip8_op_fx55(chip8);
    for (size_t i = 0x250; i <= (0x250 + VF); i++) {
        assert(chip8->memory[i] == 251);
    }
    
    // LD Vx, [I]
    chip8->index = 0x250;
    chip8->inst = 0xFF65;
    for (size_t i = 0x250; i <= (0x250 + VF); i++) {
        chip8->memory[i] = 123;
    }
    chip8_op_fx65(chip8);
    for (int i = V0; i <= VF; i++) {
        assert(chip8->registers[i] == 123);
    }
}
#endif
