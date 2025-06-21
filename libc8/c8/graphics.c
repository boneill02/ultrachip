/**
 * @file graphics.c
 *
 * Backend-agnostic graphics-related functions
 */

#include "graphics.h"

#ifndef SDL2

#include <stdio.h>

void __attribute__((weak)) c8_beep(void) { 
	fprintf(stderr, "c8_beep() not implemented.");
}

void __attribute__((weak)) c8_deinit_graphics(void) {
	fprintf(stderr, "c8_deinit_graphics() not implemented.");
}

int __attribute__((weak)) c8_init_graphics(void) {
	fprintf(stderr, "c8_init_graphics() not implemented.\n");
	return -1;
}

void __attribute__((weak)) c8_render(c8_display_t *display, int *colors) { 
	fprintf(stderr, "c8_render() not implemented.");
}

int tick(int *key, int clockSpeed) {
	fprintf(stderr, "tick() not implemented.\n");
	return -1;
}

#endif

/**
 * @brief Get the value of (x,y) from `display`
 *
 * @param display `display_t` to get pixel from
 * @param x the x value
 * @param y the y value
 *
 * @return pointer to value of (x,y) in `display`
 */
int *c8_get_pixel(c8_display_t *display, int x, int y) {
	if (display->mode == C8_DISPLAYMODE_HIGH) {
		x += display->x;
		y += display->y;
	}
    return &display->p[y * C8_LOW_DISPLAY_WIDTH + x];
}
