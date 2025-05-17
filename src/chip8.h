#ifndef CHIP8_H
#define CHIP8_H

#include <stdbool.h>
#include <stdint.h>

#define FONT_START 0x000
#define PROG_START 0x200
#define MEMSIZE 0x1000
#define CLOCK_SPEED 500
#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define STACK_SIZE 16

typedef struct chip8_s {
	uint8_t mem[MEMSIZE];
	uint8_t V[16];
	uint8_t sp;
	uint8_t dt;
	uint8_t st;
	uint16_t stack[STACK_SIZE];
	uint16_t pc;
	uint16_t I;
	int key[0x10];
	int VK;
	bool display[DISPLAY_WIDTH * DISPLAY_HEIGHT];
	int cs;
	int waitingForKey;
} chip8_t;

#endif