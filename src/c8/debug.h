#ifndef DEBUG_H
#define DEBUG_H

#include "chip8.h"

#define DEBUG_QUIT 1
#define DEBUG_CONTINUE 2
#define DEBUG_STEP 3

#define DEBUG_HELP_STRING "Available commands:\n\
break [addr]: add breakpoint to PC or addr, if given\n\
rmbreak [addr]: Remove breakpoint at PC or addr, if given\n\
continue: Exit debug mode until next breakpoint or completion\n\
next: Step to the next instruction\n\
print [attribute]: Print current value of given attribute\n\
help: Print this help string\n\
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


int debug_repl(chip8_t *);
int has_breakpoint(uint16_t);

#endif
