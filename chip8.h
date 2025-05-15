#ifndef CHIP8_H
#define CHIP8_H

#include <stdbool.h>
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

typedef struct chip8_s {
	uint8_t mem[0x1000];
	uint8_t V[16];
	uint8_t sp;
	uint8_t dt;
	uint8_t st;
	uint16_t stack[16];
	uint16_t pc;
	uint16_t I;
	int key[0x10];
	int VK;
	int display[DISPLAY_WIDTH * DISPLAY_HEIGHT];
	int cs;
	int waitingForKey;
} chip8_t;

#endif