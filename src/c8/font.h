#ifndef CHIP8_FONT_H
#define CHIP8_FONT_H

#include "chip8.h"

#define FONT_START 0x000
#define HIGH_FONT_START FONT_START + (0x10 * 5)

extern const char *fontNames[2][5];

void print_fonts(chip8_t *);
void set_fonts(chip8_t *, int, int);
int set_fonts_s(chip8_t *, char *);
int set_small_font(chip8_t *, const char *);
int set_big_font(chip8_t *, const char *);

#endif
