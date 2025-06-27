/**
 * @file c8/chip8.h
 *
 * Stuff for simulating and modifying `c8_t`s.
 */

#ifndef LIBC8_CHIP8_H
#define LIBC8_CHIP8_H

#include "graphics.h"
#include "defs.h"

#include <stdint.h>

#define C8_CLOCK_SPEED 1000
#define C8_STACK_SIZE 16

#define C8_FLAG_DEBUG 0x1
#define C8_FLAG_VERBOSE 0x2
#define C8_FLAG_QUIRK_BITWISE 0x4
#define C8_FLAG_QUIRK_DRAW 0x8
#define C8_FLAG_QUIRK_LOADSTORE 0x10
#define C8_FLAG_QUIRK_SHIFT 0x20
#define C8_FLAG_QUIRK_JUMP 0x40

 /**
  * @struct c8_t
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
    uint8_t mem[C8_MEMSIZE];
    uint8_t R[8];
    uint8_t V[16];
    uint8_t sp;
    uint8_t dt;
    uint8_t st;
    uint16_t stack[C8_STACK_SIZE];
    uint16_t pc;
    uint16_t I;
    int key[18];
    int VK;
    int cs;
    int waitingForKey;
    int running;
    c8_display_t display;
    int flags;
    int breakpoints[C8_MEMSIZE];
    int colors[2];
    int fonts[2];
    int draw;
} c8_t;

void c8_deinit(c8_t*);
c8_t* c8_init(const char*, int);
int c8_load_palette_s(c8_t*, char*);
int c8_load_palette_f(c8_t*, const char*);
void c8_load_quirks(c8_t*, const char*);
void c8_simulate(c8_t*);

#endif
