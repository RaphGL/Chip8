#ifndef CHIP8_MEMORY
#define CHIP8_MEMORY
#include <stdint.h>

enum Register {
    V0,
    V1,
    V2,
    V3,
    V4,
    V5,
    V6,
    V7,
    V8,
    V9,
    VA,
    VB,
    VC,
    VD,
    VE,
    VF
};


// where all fonts are stored
#define FONTADDR 0x050
// where all instructions are stored
#define INSTADDR 0x200

#define MEMORYSIZ 4096
#define STACKSIZ 16
#define REGISTERSIZ 16
#define KEYPADSIZ 16
#define VIDEO_W 64
#define VIDEO_H 32
#define FONTSETSIZ 80

struct Chip8 {
    uint8_t memory[MEMORYSIZ];
    uint16_t stack[STACKSIZ];
    uint8_t registers[REGISTERSIZ];
    uint8_t keypad[KEYPADSIZ];
    uint32_t video[VIDEO_W * VIDEO_H];
    uint16_t index;
    uint16_t pc;
    uint8_t sound_timer;
    uint8_t delay_timer;
    uint8_t sp;
    // current instruction
    uint16_t inst; 
};

extern const uint8_t chip8_font_set[FONTSETSIZ];

struct Chip8 chip8_new(void);
void chip8_load_rom(struct Chip8 *chip8, const char *restrict filename);
void chip8_cycle(struct Chip8 *chip8);

#endif
