#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <stdbool.h>
#include <math.h>
#include "cpu.h"

#define WIN_H 400
#define WIN_W 800

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

// should only be modified by the input
_Atomic(bool) running = true;

void chip8_video_draw(struct Chip8 *const chip8) {
    uint32_t *video = chip8->video;

    SDL_Rect pixel = {
        .h = WIN_W / VIDEO_W,
        .w = WIN_H / VIDEO_H,
    };
    for (size_t y = 0; y < VIDEO_H; y++) {
        for (size_t x = 0; x < VIDEO_W;  x++) {
            pixel.x = x * pixel.w;
            pixel.y = y * pixel.h;

            uint32_t vid_pixel = video[y * VIDEO_W + x];
            if (vid_pixel == 0xFFFFFFFF) {
                SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            } else {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
            }

            SDL_RenderFillRect(renderer, &pixel);
        }
    }

    SDL_RenderPresent(renderer);
}

void chip8_quit_video(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    renderer = NULL;
    window = NULL;
}

void chip8_init_video(const struct Chip8 *chip8) {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Chip8 Emulator",  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_W, WIN_H, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!window) {
        fputs("Error: Couldn't create window.", stderr);
        exit(1);
    }

    if (!renderer) {
        fputs("Error: Couldn't create renderer.", stderr);
        exit(1);
    }
}

void chip8_capture_input(struct Chip8 *chip8) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
        }

        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_x:
                    chip8->keypad[0] = 1;
                break;

                case SDLK_1:
                    chip8->keypad[1] = 1;
                break;

                case SDLK_2:
                    chip8->keypad[2] = 1;
                break;

                case SDLK_3:
                    chip8->keypad[3] = 1;
                break;

                case SDLK_q:
                    chip8->keypad[4] = 1;
                break;

                case SDLK_w:
                    chip8->keypad[5] = 1;
                break;

                case SDLK_e:
                    chip8->keypad[6] = 1;
                break;

                case SDLK_a:
                    chip8->keypad[7] = 1;
                break;

                case SDLK_s:
                    chip8->keypad[8] = 1;
                break;

                case SDLK_d:
                    chip8->keypad[9] = 1;
                break;

                case SDLK_z:
                    chip8->keypad[0xA] = 1;
                break;

                case SDLK_c:
                    chip8->keypad[0xB] = 1;
                break;

                case SDLK_4:
                    chip8->keypad[0xC] = 1;
                break;

                case SDLK_r:
                    chip8->keypad[0xD] = 1;
                break;

                case SDLK_f:
                    chip8->keypad[0xE] = 1;
                break;

                case SDLK_v:
                    chip8->keypad[0xF] = 1;
                break;
            }
        }

        if (e.type == SDL_KEYUP) {
            switch (e.key.keysym.sym) {
                case SDLK_x:
                    chip8->keypad[0] = 0;
                break;

                case SDLK_1:
                    chip8->keypad[1] = 0;
                break;

                case SDLK_2:
                    chip8->keypad[2] = 0;
                break;

                case SDLK_3:
                    chip8->keypad[3] = 0;
                break;

                case SDLK_q:
                    chip8->keypad[4] = 0;
                break;

                case SDLK_w:
                    chip8->keypad[5] = 0;
                break;

                case SDLK_e:
                    chip8->keypad[6] = 0;
                break;

                case SDLK_a:
                    chip8->keypad[7] = 0;
                break;

                case SDLK_s:
                    chip8->keypad[8] = 0;
                break;

                case SDLK_d:
                    chip8->keypad[9] = 0;
                break;

                case SDLK_z:
                    chip8->keypad[0xA] = 0;
                break;

                case SDLK_c:
                    chip8->keypad[0xB] = 0;
                break;

                case SDLK_4:
                    chip8->keypad[0xC] = 0;
                break;

                case SDLK_r:
                    chip8->keypad[0xD] = 0;
                break;

                case SDLK_f:
                    chip8->keypad[0xE] = 0;
                break;

                case SDLK_v:
                    chip8->keypad[0xF] = 0;
                break;
            }
        }
    }
}

void chip8_init_input(struct Chip8 *chip8) {
    if (SDL_Init(SDL_INIT_EVENTS) < 0) {
        fputs("Error: Could not initialize SDL events.", stderr);
        exit(1);
    }
}

void chip8_play_audio(struct Chip8 *const chip8) {
    if (chip8->sound_timer > 0) {
        SDL_PauseAudio(0);
    } else {
        SDL_PauseAudio(1);
    }
}

#define AMPLITUDE 28000
#define SAMPLE_RATE 44100
#define M_PI 3.14159265358979323846

static void chip8_audio_callback(void *userdata, uint8_t *raw_buffer, int bytes) {
    int16_t *buffer = (int16_t *)raw_buffer;
    int len = bytes / 2;
    int sample_nr = 0;

    for (int i = 0; i < len; i++, sample_nr++) {
        double time = sample_nr / (double)SAMPLE_RATE;
        buffer[i] = AMPLITUDE * sin(2.0f * M_PI * 441.0f * time);
    }
}

void chip8_quit_audio(void) {
    SDL_CloseAudio();
}

void chip8_init_audio(const struct Chip8 *chip8) {
    if(SDL_Init(SDL_INIT_AUDIO) < 0) {
        fputs("Error: Could not initialize SDL audio.", stderr);
        exit(1);
    }

    SDL_AudioSpec desired = {
        .freq = SAMPLE_RATE,
        .format = AUDIO_S16SYS,
        .channels = 1,
        .samples = 2048,
        .userdata = NULL,
        .callback = chip8_audio_callback,
    };
    SDL_AudioSpec obtained;
    if (SDL_OpenAudio(&desired, &obtained) != 0) {
        fputs("Error: Could not open SDL audio.", stderr);
        exit(1);
    }

    if (obtained.format != desired.format) {
        fputs("Error: Didn't receive the correct audio format from SDL.", stderr);
        exit(1);
    }
}
