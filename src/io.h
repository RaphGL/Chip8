#ifndef CHIP8_IO
#define CHIP8_IO
#include "cpu.h"

extern _Atomic(bool) running;

void chip8_init_video(const struct Chip8 *chip8);
void chip8_video_draw(struct Chip8 *const chip8);
void chip8_quit_video(void);

void chip8_init_audio(const struct Chip8 *chip8);
void chip8_play_audio(struct Chip8 *const chip8);
void chip8_quit_audio(void);

void chip8_init_input(struct Chip8 *chip8);
void chip8_capture_input(struct Chip8 *chip8);
#endif
