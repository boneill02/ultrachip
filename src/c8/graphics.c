/**
 * @file graphics.c
 *
 * Backend-agnostic graphics-related functions
 */

#include "graphics.h"

/**
 * @brief Get the value of (x,y) from `display`
 *
 * @param display `display_t` to get pixel from
 * @param x the x value
 * @param y the y value
 *
 * @return pointer to value of (x,y) in `display`
 */
int *get_pixel(display_t *display, int x, int y) {
	if (display->mode == DISPLAY_EXTENDED) {
		x += display->x;
		y += display->y;
	}
    return &display->p[y * STANDARD_DISPLAY_WIDTH + x];
}
