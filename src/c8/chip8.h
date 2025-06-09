#ifndef CHIP8_H
#define CHIP8_H

#include "graphics.h"
#include "util/defs.h"

#include <stdint.h>

#define FLAG_DEBUG 0x1
#define FLAG_VERBOSE 0x2

typedef struct {
	uint8_t mem[MEMSIZE];
	uint8_t R[8];
	uint8_t V[16];
	uint8_t sp;
	uint8_t dt;
	uint8_t st;
	uint16_t stack[STACK_SIZE];
	uint16_t pc;
	uint16_t I;
	int key[0x10];
	int VK;
	int cs;
	int waitingForKey;
	int running;
	display_t display;
	int flags;
} chip8_t;

void deinit_chip8(chip8_t *);
int *get_pixel(display_t *, int, int);
chip8_t *init_chip8(int, int, const char *);
void simulate(chip8_t *);

#endif