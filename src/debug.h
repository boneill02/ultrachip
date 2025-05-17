#ifndef DEBUG_H
#define DEBUG_H

#include <stdbool.h>

#include "chip8.h"

#define DEBUG_QUIT 1
#define DEBUG_CONTINUE 2
#define DEBUG_STEP 3

int debug_repl(chip8_t *);
bool has_breakpoint(uint16_t);

#endif
