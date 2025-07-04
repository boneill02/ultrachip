/**
 * @file c8/private/exception.h
 * @note NOT EXPORTED
 *
 * Stuff for handling exceptions.
 */

#ifndef LIBC8_EXCEPTION_H
#define LIBC8_EXCEPTION_H

#include <stdio.h>

#define EXCEPTION_MESSAGE_SIZE BUFSIZ

#define C8_EXCEPTION(code, ...) { \
    snprintf(c8_exception, EXCEPTION_MESSAGE_SIZE, __VA_ARGS__); \
    handle_exception(code); \
}

typedef enum {
    INVALID_INSTRUCTION_EXCEPTION = -3,
    TOO_MANY_LABELS_EXCEPTION = -4,
    STACK_OVERFLOW_EXCEPTION = -5,
    INVALID_ARGUMENT_EXCEPTION = -6,
    DUPLICATE_LABEL_EXCEPTION = -7,
    INVALID_SYMBOL_EXCEPTION = -8,
    MEMORY_ALLOCATION_EXCEPTION = -9,
    UNKNOWN_EXCEPTION = -10,
    TOO_MANY_SYMBOLS_EXCEPTION = -11,
    LOAD_FILE_FAILURE_EXCEPTION = -12,
    FILE_TOO_BIG_EXCEPTION = -13,
    INVALID_COLOR_PALETTE_EXCEPTION = -14,
    INVALID_QUIRK_EXCEPTION = -15,
    FAILED_GRAPHICS_INITIALIZATION_EXCEPTION = -16,
    INVALID_FONT_EXCEPTION = -17,
    INVALID_CLOCK_SPEED_EXCEPTION = -18,
    STACK_UNDERFLOW_EXCEPTION = -19
} exception_code_t;


/**
  * Message to print when calling `handle_exception` with a non-zero code
  */
extern char c8_exception[EXCEPTION_MESSAGE_SIZE];

void handle_exception(int);

#endif