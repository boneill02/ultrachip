#ifndef CHIP8_H
#define CHIP8_H

#include <stdbool.h>
#include <stdint.h>

#ifndef FONT_START
#define FONT_START 0x000
#endif

#ifndef PROG_START
#define PROG_START 0x200
#endif

#ifndef CLOCK_SPEED
#define CLOCK_SPEED 500
#endif

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

void parse_instruction(uint16_t);
void print_debug(void);

#endif