/**
 * @file c8/defs.h
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
	uint8_t x = C8_X(i); \
	uint8_t y = C8_Y(i); \
	uint16_t nnn = C8_NNN(i); \
	uint8_t a = C8_A(i); \
	uint8_t b = C8_B(i); \
	uint8_t kk = C8_KK(i);

#define C8_PROG_START 0x200
#define C8_MEMSIZE 0x1000

#endif
