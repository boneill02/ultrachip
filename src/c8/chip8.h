/**
 * @file chip8.h
 *
 * Stuff for simulating and modifying `chip8_t`s.
 */

#ifndef CHIP8_H
#define CHIP8_H

#include "graphics.h"
#include "util/defs.h"

#include <stdint.h>

#define CLOCK_SPEED 500
#define STACK_SIZE 16

#define FLAG_DEBUG 0x1
#define FLAG_VERBOSE 0x2
#define FLAG_QUIRK_BITWISE 0x4
#define FLAG_QUIRK_DRAW 0x8
#define FLAG_QUIRK_LOADSTORE 0x10
#define FLAG_QUIRK_SHIFT 0x20
#define FLAG_QUIRK_JUMP 0x40

/**
 * @struct chip8_t
 * @brief Represents current state of the CHIP-8 interpreter
 *
 * @param mem CHIP-8 memory
 * @param R flag registers
 * @param V V (general purpose) registers
 * @param sp stack pointer
 * @param dt display timer
 * @param st sound timer
 * @param stack stack
 * @param pc program counter
 * @param I I (address) register
 * @param key key press states
 * @param VK V to store next keypress
 * @param cs instructions to execute per second
 * @param waitingForKey 1 or 0
 * @param running 1 or 0
 * @param display graphics display
 * @param flags CLI flags
 * @param breakpoints debug breakpoint map
 * @param colors 24 bit hex colors, background=[0] foreground=[1]
 * @param fonts font IDs (see font.c)
 * @param draw need to draw? (1 or 0)
 */
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
	int key[18];
	int VK;
	int cs;
	int waitingForKey;
	int running;
	display_t display;
	int flags;
	int breakpoints[MEMSIZE];
	int colors[2];
	int fonts[2];
	int draw;
} chip8_t;

void deinit_chip8(chip8_t *);
int *get_pixel(display_t *, int, int);
chip8_t *init_chip8(const char *, int);
int load_palette_arg(int *, char *);
int load_palette_file(int *, const char *);
void load_quirks(int *, const char *);
void simulate(chip8_t *);

#endif
