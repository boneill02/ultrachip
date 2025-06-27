/**
 * @file c8/private/graphics_sdl2.c
 *
 * SDL2 graphics implementation. When `SDL2` is defined, overrides weak
 * graphics utility function definitions in `graphics.c`.
 */

#include "../graphics.h"

#include <SDL2/SDL.h>
#include <stdint.h>

#define RGB_R(i) ((i >> 16) & 0xFF)
#define RGB_G(i) ((i >> 8) & 0xFF)
#define RGB_B(i) (i & 0xFF)

static SDL_Window* window;
static SDL_Renderer* renderer;
static SDL_Rect pix = {
    .x = 0,
    .y = 0,
    .w = C8_WINDOW_SCALE_X,
    .h = C8_WINDOW_SCALE_Y,
};
static SDL_Rect winRect = {
    .x = 0,
    .y = 0,
    .w = C8_LOW_DISPLAY_WIDTH,
    .h = C8_LOW_DISPLAY_HEIGHT,
};

/**
 * Map of all keys to track.
 *
 * * `keyMap[x][0]` is `SDL_Keycode`
 * * `keyMap[x][]` is CHIP-8 keycode
 * * `keyMap[16]` enables debug mode / step,
 * * `keyMap[17]` disables debug mode
 */
static int keyMap[18][2] = {
    { SDLK_1, 0 },
    { SDLK_2, 1 },
    { SDLK_3, 2 },
    { SDLK_4, 3 },
    { SDLK_q, 4 },
    { SDLK_w, 5 },
    { SDLK_e, 6 },
    { SDLK_r, 7 },
    { SDLK_a, 8 },
    { SDLK_s, 9 },
    { SDLK_d, 10 },
    { SDLK_f, 11 },
    { SDLK_z, 12 },
    { SDLK_x, 13 },
    { SDLK_c, 14 },
    { SDLK_v, 15 },
    { SDLK_p, 16 }, // Enter debug mode
    { SDLK_m, 17 }, // Leave debug mode
};

static int get_key(SDL_Keycode k);

/**
 * @brief Deinitialize the graphics library.
 */
void c8_deinit_graphics(void) {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

/**
 * @brief Initialize the graphics library.
 *
 * @return 1 if successful, 0 otherwise.
 */
uint8_t c8_init_graphics(void) {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("CHIP8",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        C8_DEFAULT_WINDOW_WIDTH, C8_DEFAULT_WINDOW_HEIGHT,
        SDL_WINDOW_RESIZABLE);
    if (!window) {
        return 0;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    return renderer != NULL;
}

/**
 * Render the given display to the SDL2 window.
 *
 * @param display `display_t` to render
 * @param colors colors to render
 */
void c8_render(c8_display_t* display, int* colors) {
    int dx = 0;
    int dy = 0;

    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, RGB_R(colors[0]), RGB_G(colors[0]),
        RGB_B(colors[0]), SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &winRect);
    SDL_SetRenderDrawColor(renderer, RGB_R(colors[1]), RGB_G(colors[1]),
        RGB_B(colors[1]), SDL_ALPHA_OPAQUE);

    if (display->mode == C8_DISPLAYMODE_HIGH) {
        dx = display->x;
        dy = display->y;
    }


    for (int i = 0; i < C8_LOW_DISPLAY_WIDTH; i++) {
        for (int j = 0; j < C8_LOW_DISPLAY_HEIGHT; j++) {
            if (*c8_get_pixel(display, i + dx, j + dy)) {
                pix.x = i * C8_WINDOW_SCALE_X;
                pix.y = j * C8_WINDOW_SCALE_Y;
                SDL_RenderFillRect(renderer, &pix);
            }
        }
    }

    SDL_RenderPresent(renderer);
}

/**
 * @brief Process keypresses and delay based on clockspeed.
 *
 * If a relevant key is pressed or released (see `keyMap` in this file), this
 * function will update `key` accordingly.
 *
 * @param key pointer to int arr of keys
 * @param clockSpeed instructions per second
 *
 * @return -2 if quitting, -1 if no key was pressed/released, else returns value
 * of key pressed/released.
 */
int c8_tick(int* key, int clockSpeed) {
    SDL_Delay(1000 / clockSpeed);
    SDL_Event e;
    int ret = -1;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_QUIT:
            return -2;
        case SDL_KEYDOWN:
            if ((ret = get_key(e.key.keysym.sym)) != -1) {
                key[ret] = 1;
            }
            break;
        case SDL_KEYUP:
            if ((ret = get_key(e.key.keysym.sym)) != -1) {
                key[ret] = 0;
            }
            break;
        }
    }

    return ret > 15 ? -1 : ret;
}

/**
 * @brief Convert the given SDL Keycode to a CHIP-8 keycode.
 *
 * @param k the SDL_Keycode
 *
 * @return the CHIP-8 keycode
 */
static int get_key(SDL_Keycode k) {
    for (int i = 0; i < 18; i++) {
        if (keyMap[i][0] == k) {
            return keyMap[i][1];
        }
    }
    return -1;
}
