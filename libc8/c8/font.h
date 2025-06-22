/**
 * @file c8/font.h
 *
 * Stuff related to loading fonts.
 */

#ifndef LIBC8_FONT_H
#define LIBC8_FONT_H

#include "chip8.h"

#define FONT_START 0x000
#define HIGH_FONT_START FONT_START + (0x10 * 5)

extern const char *c8_fontNames[2][5];

void print_fonts(c8_t *);
void c8_set_fonts(c8_t *, int, int);
int c8_set_fonts_s(c8_t *, char *);
int c8_set_small_font(c8_t *, const char *);
int c8_set_big_font(c8_t *, const char *);

#endif
