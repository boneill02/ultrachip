/**
 * @file defs.h
 *
 * Base CHIP-8 definitions/macros
 */

#ifndef LIBC8_DEFS_H
#define LIBC8_DEFS_H

#define C8_X(i) ((i & 0x0F00) >> 8)
#define C8_Y(i) ((i & 0x00F0) >> 4)
#define C8_NNN(i) (i & 0x0FFF)
#define C8_A(i) ((i & 0xF000) >> 12)
#define C8_B(i) (i & 0x000F)
#define C8_KK(i) (i & 0x00FF)
#define C8_EXPAND(i) \
	int x = C8_X(i); \
	int y = C8_Y(i); \
	int nnn = C8_NNN(i); \
	int a = C8_A(i); \
	int b = C8_B(i); \
	int kk = C8_KK(i);

#define C8_PROG_START 0x200
#define C8_MEMSIZE 0x1000


#endif
