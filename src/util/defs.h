#ifndef CHIP8_DEFS_H
#define CHIP8_DEFS_H

#define X(i) ((i & 0x0F00) >> 8)
#define Y(i) ((i & 0x00F0) >> 4)
#define NNN(i) (i & 0x0FFF)
#define A(i) ((i & 0xF000) >> 12)
#define B(i) (i & 0x000F)
#define KK(i) (i & 0x00FF)

#define EXPAND(i) \
	int x = X(i); \
	int y = Y(i); \
	int nnn = NNN(i); \
	int a = A(i); \
	int b = B(i); \
	int kk = KK(i);

#define FONT_START 0x000
#define PROG_START 0x200
#define MEMSIZE 0x1000
#define CLOCK_SPEED 500
#define STACK_SIZE 16
#define STANDARD_DISPLAY_WIDTH 64
#define STANDARD_DISPLAY_HEIGHT 32
#define EXTENDED_DISPLAY_WIDTH 128
#define EXTENDED_DISPLAY_HEIGHT 64

#define MAX_MALLOCS 16
#define MAX_FILES 3
#define EXCEPTION_MESSAGE_SIZE 64

#define NULL_ARGUMENT_EXCEPTION (-3)
#define INVALID_INSTRUCTION_EXCEPTION (-4)
#define TOO_MANY_LABELS_EXCEPTION (-5)
#define STACK_OVERFLOW_EXCEPTION (-6)
#define INVALID_ARGUMENT_EXCEPTION (-7)
#define INVALID_ARGUMENT_EXCEPTION_INTERNAL (-8)
#define DUPLICATE_LABEL_EXCEPTION (-9)
#define INVALID_SYMBOL_EXCEPTION (-10)
#define TOO_MANY_MALLOCS_EXCEPTION (-11)
#define UNKNOWN_EXCEPTION (-12)
#define TOO_MANY_SYMBOLS_EXCEPTION (-13)

#define NULLCHECK_ARGS1(a) if (!(a)) { return NULL_ARGUMENT_EXCEPTION; }
#define NULLCHECK_ARGS2(a,b) if (!(a) || !(b)) { return NULL_ARGUMENT_EXCEPTION; }
#define NULLCHECK_ARGS3(a,b,c) if (!(a) || !(b) || !(c)) { return NULL_ARGUMENT_EXCEPTION; }
#define VERBOSE_PRINT(s) if (args & ARG_VERBOSE) { printf("%s\n", s); }

#define UNKNOWN_EXCEPTION_MESSAGE "An unknown error has occurred."
#define NULL_ARGUMENT_EXCEPTION_MESSAGE "A NULL argument was passed to a function."
#define INVALID_INSTRUCTION_EXCEPTION_MESSAGE "An invalid instruction exists in the input file."
#define TOO_MANY_LABELS_EXCEPTION_MESSAGE "Too many labels are defined in the input file."
#define STACK_OVERFLOW_EXCEPTION_MESSAGE "A stack overflow occurred during execution."
#define INVALID_ARGUMENT_EXCEPTION_MESSAGE "An invalid instruction argument was given."
#define INVALID_ARGUMENT_EXCEPTION_INTERNAL_MESSAGE "An invalid argument was passed internally."
#define DUPLICATE_LABEL_EXCEPTION_MESSAGE "A label was defined multiple times."
#define INVALID_SYMBOL_EXCEPTION_MESSAGE "An invalid symbol exists in the input file."
#define TOO_MANY_MALLOCS_EXCEPTION_MESSAGE "Too many mallocs."

#endif
