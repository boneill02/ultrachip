/**
 * @file libc8/internal/exception.h
 * @note NOT EXPORTED
 * 
 * Stuff for handling exceptions.
 */

#ifndef LIBC8_EXCEPTION_H
#define LIBC8_EXCEPTION_H

#include <stdio.h>

#define EXCEPTION_MESSAGE_SIZE 64

#define NULLCHECK1(a) if (!(a)) { \
	sprintf(c8_exception, "At %s", __func__); \
	handle_exception(NULL_ARGUMENT_EXCEPTION); \
}

#define NULLCHECK2(a,b) if (!(a) || !(b)) { \
	sprintf(c8_exception, "At %s", __func__);\
	handle_exception(NULL_ARGUMENT_EXCEPTION); \
}

#define NULLCHECK3(a,b,c) if (!(a) || !(b) || !(c)) { \
	sprintf(c8_exception, "At %s", __func__); \
	handle_exception(NULL_ARGUMENT_EXCEPTION); \
}

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
#define LOAD_FILE_FAILURE_EXCEPTION (-14)
#define FILE_TOO_BIG_EXCEPTION (-15)
#define INVALID_COLOR_PALETTE_EXCEPTION (-16)
#define INVALID_QUIRK_EXCEPTION (-17)
#define FAILED_GRAPHICS_INITIALIZATION_EXCEPTION (-18)
#define INVALID_FONT_EXCEPTION (-19)

#define NULL_ARGUMENT_EXCEPTION_MESSAGE "A NULL argument was passed to a function."
#define INVALID_INSTRUCTION_EXCEPTION_MESSAGE "An invalid instruction exists in the input file."
#define TOO_MANY_LABELS_EXCEPTION_MESSAGE "Too many labels are defined in the input file."
#define STACK_OVERFLOW_EXCEPTION_MESSAGE "A stack overflow occurred during execution."
#define INVALID_ARGUMENT_EXCEPTION_MESSAGE "An invalid instruction argument was given."
#define INVALID_ARGUMENT_EXCEPTION_INTERNAL_MESSAGE "An invalid argument was passed internally."
#define DUPLICATE_LABEL_EXCEPTION_MESSAGE "A label was defined multiple times."
#define INVALID_SYMBOL_EXCEPTION_MESSAGE "An invalid symbol exists in the input file."
#define TOO_MANY_MALLOCS_EXCEPTION_MESSAGE "Too many mallocs."
#define UNKNOWN_EXCEPTION_MESSAGE "An unknown error has occurred."
#define TOO_MANY_SYMBOLS_EXCEPTION_MESSAGE "Too many symbols exist in the input file."
#define LOAD_FILE_FAILURE_EXCEPTION_MESSAGE "Failed to load file."
#define FILE_TOO_BIG_EXCEPTION_MESSAGE "The given file is too big."
#define INVALID_COLOR_PALETTE_EXCEPTION_MESSAGE "Invalid color palette."
#define INVALID_QUIRK_EXCEPTION_MESSAGE "Invalid quirk."
#define FAILED_GRAPHICS_INITIALIZATION_EXCEPTION_MESSAGE "Failed to initialize graphics."
#define INVALID_FONT_EXCEPTION_MESSAGE "Invalid font."

/**
 * Message to print when calling `handle_exception` with a non-zero code
 */
extern char c8_exception[EXCEPTION_MESSAGE_SIZE];

void handle_exception(int);

#endif