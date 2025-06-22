/**
 * @file libc8/internal/debug.h
 * @note NOT EXPORTED
 *
 * Stuff related to debug mode.
 */

#ifndef LIBC8_DEBUG_H
#define LIBC8_DEBUG_H

#include "../chip8.h"

#define DEBUG_QUIT 1
#define DEBUG_CONTINUE 2
#define DEBUG_STEP 3

#define DEBUG_HELP_STRING "Available commands:\n\
break [ADDRESS]: Add breakpoint to PC or ADDRESS, if given\n\
rmbreak [ADDRESS]: Remove breakpoint at PC or ADDRESS, if given\n\
continue: Exit debug mode until next breakpoint or completion\n\
help: Print this help string\n\
load PATH: Load program state from PATH\n\
next: Step to the next instruction\n\
print [ATTRIBUTE]: Print current value of ATTRIBUTE\n\
save PATH: Save program state to the given file\n\
set ATTRIBUTE VALUE: Set the given attribute to the given value\n\
quit: Terminate the program\n\
\n\
Available attributes to print:\n\
PC: Program counter\n\
SP: Stack pointer\n\
DT: Delay timer\n\
ST: Sound timer\n\
I:  I address\n\
K:  Register to store next keypress\n\
V[x]:  All register values or value of Vx, if given\n\
stack: All stack values\n\
$[address]: Value at given address\n\
\n\
If no argument is given to print, it will print all of the above attributes\n\
except for address values.\n"


int debug_repl(c8_t *);
int has_breakpoint(c8_t *, uint16_t);

#endif
