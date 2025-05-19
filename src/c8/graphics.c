#include "graphics.h"

#include "util.h"

#include <SDL2/SDL.h>

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Rect pix = {
	.x = 0,
	.y = 0,
	.w = WINDOW_SCALE_X,
	.h = WINDOW_SCALE_Y,
};
SDL_Rect winRect = {
	.x = 0,
	.y = 0,
	.w = DEFAULT_WINDOW_WIDTH,
	.h = DEFAULT_WINDOW_HEIGHT,
};

uint16_t keyMap[16][2] = {
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
};

int get_key(SDL_Keycode k);

/**
 * @brief Deinitialize the graphics library.
 */
void deinit_graphics(void) {
	SDL_DestroyWindow(window);
	SDL_Quit();
}

/**
 * @brief Convert the given SDL Keycode to a CHIP-8 keycode.
 * 
 * @param k the SDL_Keycode
 * 
 * @return the CHIP-8 keycode
 */
int get_key(SDL_Keycode k) {
	for (int i = 0; i < 16; i++) {
		if (keyMap[i][0] == k) return keyMap[i][1];
	}
	return -1;
}

/**
 * @brief Initialize the graphics library.
 * 
 * @return 1 if successful, 0 otherwise.
 */
int init_graphics(void) {
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("CHIP8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
	if (!window) return 0;
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	return renderer != NULL;
}

/**
 * Render the given display to the SDL2 window.
 * 
 * @param display pointer to a int arr of size `DISPLAY_WIDTH*DISPLAY_HEIGHT`
 */
void render(int *display) {
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(renderer, &winRect);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
	for (int i = 0; i < DISPLAY_WIDTH; i++) {
		for (int j = 0; j < DISPLAY_HEIGHT; j++) {
			if (*get_pixel(display, i, j)) {
				pix.x = i * WINDOW_SCALE_X;
				pix.y = j * WINDOW_SCALE_Y;
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
 * 
 * @return -2 if quitting, -1 if no key was pressed/released, else returns value
 * of key pressed/released.
 */
int tick(int *key, int clockSpeed) {
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

    return ret;
}
