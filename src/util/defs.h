#ifndef CHIP8_DEFS_H
#define CHIP8_DEFS_H

#define X(i) ((i & 0x0F00) >> 8)
#define Y(i) ((i & 0x00F0) >> 4)
#define NNN(i) (i & 0x0FFF)
#define A(i) ((i & 0xF000) >> 12)
#define B(i) (i & 0x000F)
#define KK(i) (i & 0x00FF)

#define FONT_START 0x000
#define PROG_START 0x200
#define MEMSIZE 0x1000
#define CLOCK_SPEED 500
#define STACK_SIZE 16
#define STANDARD_DISPLAY_WIDTH 32
#define STANDARD_DISPLAY_HEIGHT 64
#define EXTENDED_DISPLAY_WIDTH 64
#define EXTENDED_DISPLAY_HEIGHT 128

#endif