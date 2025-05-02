#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

#ifndef FONT_START
#define FONT_START 0x000
#endif

#ifndef PROG_START
#define PROG_START 0x200
#endif

#ifndef CLOCK_SPEED
#define CLOCK_SPEED 500
#endif

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 400
#define WINDOW_SCALE_X (DEFAULT_WINDOW_WIDTH / DISPLAY_WIDTH)
#define WINDOW_SCALE_Y (DEFAULT_WINDOW_HEIGHT / DISPLAY_HEIGHT)
#define FRAMERATE_CAP 60.0

uint8_t mem[0x1000], V[16];
uint8_t sp = 0, dt = 0, st = 0;
uint16_t stack[16];
uint16_t pc = 0x200, I = 0;
int key[0x10];
int keyRegister;
bool waitingForKey = false;

#endif