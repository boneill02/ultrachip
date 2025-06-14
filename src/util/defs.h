#ifndef CHIP8_DEFS_H
#define CHIP8_DEFS_H

#define X(i) ((i & 0x0F00) >> 8)
#define Y(i) ((i & 0x00F0) >> 4)
#define NNN(i) (i & 0x0FFF)
#define A(i) ((i & 0xF000) >> 12)
#define B(i) (i & 0x000F)
#define KK(i) (i & 0x00FF)

#define EXPAND(i) \
	int x = X(i); \
	int y = Y(i); \
	int nnn = NNN(i); \
	int a = A(i); \
	int b = B(i); \
	int kk = KK(i);

#define FONT_START 0x000
#define PROG_START 0x200
#define MEMSIZE 0x1000
#define CLOCK_SPEED 500
#define STACK_SIZE 16
#define STANDARD_DISPLAY_WIDTH 64
#define STANDARD_DISPLAY_HEIGHT 32
#define EXTENDED_DISPLAY_WIDTH 128
#define EXTENDED_DISPLAY_HEIGHT 64

#define NULL_ARGUMENT_EXCEPTION (-3)
#define INVALID_INSTRUCTION_EXCEPTION (-4)
#define TOO_MANY_LABELS_EXCEPTION (-5)
#define STACK_OVERFLOW_EXCEPTION (-6)
#define INVALID_ARGUMENT_EXCEPTION (-7)
#define DUPLICATE_LABEL_EXCEPTION (-8)

#endif