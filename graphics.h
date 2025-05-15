#ifndef CHIP8_GRAPHICS_H
#define CHIP8_GRAPHICS_H

#include "chip8.h"

#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 400
#define WINDOW_SCALE_X (DEFAULT_WINDOW_WIDTH / DISPLAY_WIDTH)
#define WINDOW_SCALE_Y (DEFAULT_WINDOW_HEIGHT / DISPLAY_HEIGHT)

void deinit_graphics(void);
int init_graphics(void);
void render(int *);
int tick(int *, int);

#endif