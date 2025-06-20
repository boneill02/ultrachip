/**
 * @file graphics.h
 *
 * Function declarations for graphics display are here.
 *
 * Only `get_pixel` is defined in graphics.c. Declarations are library
 * agnostic so a different graphics backend can be used.
 */


#ifndef CHIP8_GRAPHICS_H
#define CHIP8_GRAPHICS_H

#include "util/defs.h"

#define STANDARD_DISPLAY_WIDTH 64
#define STANDARD_DISPLAY_HEIGHT 32
#define EXTENDED_DISPLAY_WIDTH 128
#define EXTENDED_DISPLAY_HEIGHT 64

#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 400
#define WINDOW_SCALE_X (DEFAULT_WINDOW_WIDTH / EXTENDED_DISPLAY_WIDTH)
#define WINDOW_SCALE_Y (DEFAULT_WINDOW_HEIGHT / EXTENDED_DISPLAY_HEIGHT)

#define DISPLAY_STANDARD 0
#define DISPLAY_EXTENDED 1

/**
 * @struct display_t
 *
 * @param p pixels (1D array)
 * @param mode display mode (`DISPLAY_STANDARD` or `DISPLAY_EXTENDED`)
 * @param x x offset (for `DISPLAY_EXTENDED`)
 * @param y y offset (for `DISPLAY_EXTENDED`)
 */
typedef struct {
	int p[EXTENDED_DISPLAY_WIDTH * EXTENDED_DISPLAY_HEIGHT];
	int mode;
	int x, y;
} display_t;

void deinit_graphics(void);
int *get_pixel(display_t *, int, int);
int init_graphics(void);
void render(display_t *, int *);
int tick(int *, int);
#endif