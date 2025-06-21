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
int *c8_get_pixel(c8_display_t *display, int x, int y) {
	if (display->mode == C8_DISPLAYMODE_HIGH) {
		x += display->x;
		y += display->y;
	}
    return &display->p[y * C8_LOW_DISPLAY_WIDTH + x];
}
