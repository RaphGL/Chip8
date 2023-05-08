#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <stdbool.h>
#include "cpu.h"

#define WIN_H 400
#define WIN_W 800

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static SDL_Thread *video_thread = NULL;
static SDL_Thread *input_thread = NULL;

// should only be modified by the input thread
_Atomic(bool) running = true;

static int chip8_video_draw(void *video_buf) {
    uint32_t *video = video_buf;

    while (running) {
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

    return 0;
}

void chip8_quit_video(void) {
    SDL_WaitThread(video_thread, NULL);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void chip8_init_video(const struct Chip8 *chip8) {
    SDL_Init(SDL_INIT_VIDEO);
    if (SDL_CreateWindowAndRenderer(WIN_W, WIN_H, SDL_WINDOW_SHOWN, &window, &renderer) < 0) {
        fputs("Error: Couldn't create window.", stderr);
        exit(0);
    }
    SDL_SetWindowTitle(window, "Chip8 Emulator");

    video_thread = SDL_CreateThread(chip8_video_draw, "chip8_video_draw", (void *)chip8->video);
    if (!video_thread) {
        fputs("Error: Could not create video thread.", stderr);
        exit(0);
    }
}

int chip8_capture_input(void * arg) {
    struct Chip8 *chip8 = arg;
    SDL_Event e;

    while (running) {
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

    return 0;
}

void chip8_quit_input(void) {
    SDL_WaitThread(input_thread, NULL);
}

void chip8_init_input(struct Chip8 *chip8) {
    SDL_Init(SDL_INIT_EVENTS);
    input_thread = SDL_CreateThread(chip8_capture_input, "chip8_capture_input", chip8);
    if (!input_thread) {
        fputs("Error: Could not create input thread.", stderr);
        exit(0);
    }
}

void chip8_init_audio(const struct Chip8 *chip8) {
    SDL_Init(SDL_INIT_AUDIO);
}
