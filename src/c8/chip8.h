#ifndef CHIP8_H
#define CHIP8_H

#include "defs.h"

#include <stdint.h>

#define FLAG_DEBUG 0x1
#define FLAG_VERBOSE 0x2

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
	int display[DISPLAY_WIDTH * DISPLAY_HEIGHT];
	int cs;
	int waitingForKey;
	int flags;
} chip8_t;

void deinit_chip8(chip8_t *);
chip8_t *init_chip8(int, int, const char *);
void simulate(chip8_t *);

#endif