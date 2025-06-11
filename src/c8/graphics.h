#ifndef CHIP8_GRAPHICS_H
#define CHIP8_GRAPHICS_H

#include "util/defs.h"

#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 400
#define WINDOW_SCALE_X (DEFAULT_WINDOW_WIDTH / EXTENDED_DISPLAY_WIDTH)
#define WINDOW_SCALE_Y (DEFAULT_WINDOW_HEIGHT / EXTENDED_DISPLAY_HEIGHT)

#define DISPLAY_STANDARD 0
#define DISPLAY_EXTENDED 1

typedef struct {
	int p[EXTENDED_DISPLAY_WIDTH * EXTENDED_DISPLAY_HEIGHT];
	int mode;
	unsigned int x, y;
} display_t;

void deinit_graphics(void);
int *get_pixel(display_t *, int, int);
int init_graphics(void);
void render(display_t *, int *);
int tick(int *, int);
#endif