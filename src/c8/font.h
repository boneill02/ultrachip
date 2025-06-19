#ifndef CHIP8_FONT_H
#define CHIP8_FONT_H

#include "chip8.h"

#define FONT_START 0x000
#define HIGH_FONT_START FONT_START + (0x10 * 5)

void set_fonts(chip8_t *, int, int);
void set_fonts_s(chip8_t *, const char *);

#endif
