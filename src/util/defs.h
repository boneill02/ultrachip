/**
 * @file defs.h
 *
 * Base CHIP-8 definitions/macros
 */

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

#define PROG_START 0x200
#define MEMSIZE 0x1000

#endif
