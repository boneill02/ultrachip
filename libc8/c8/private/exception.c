/**
 * @file c8/private/exception.c
 * @note NOT EXPORTED
 *
 * Stuff for handling exceptions.
 */

#include "exception.h"

#include "../defs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INVALID_INSTRUCTION_EXCEPTION_MESSAGE "An invalid instruction exists in the input file."
#define TOO_MANY_LABELS_EXCEPTION_MESSAGE "Too many labels are defined in the input file."
#define STACK_OVERFLOW_EXCEPTION_MESSAGE "A stack overflow occurred during execution."
#define INVALID_ARGUMENT_EXCEPTION_MESSAGE "An invalid instruction argument was given."
#define DUPLICATE_LABEL_EXCEPTION_MESSAGE "A label was defined multiple times."
#define INVALID_SYMBOL_EXCEPTION_MESSAGE "An invalid symbol exists in the input file."
#define MEMORY_ALLOCATION_EXCEPTION_MESSAGE "Failed to allocate memory."
#define UNKNOWN_EXCEPTION_MESSAGE "An unknown error has occurred."
#define TOO_MANY_SYMBOLS_EXCEPTION_MESSAGE "Too many symbols exist in the input file."
#define LOAD_FILE_FAILURE_EXCEPTION_MESSAGE "Failed to load file."
#define FILE_TOO_BIG_EXCEPTION_MESSAGE "The given file is too big."
#define INVALID_COLOR_PALETTE_EXCEPTION_MESSAGE "Invalid color palette."
#define INVALID_QUIRK_EXCEPTION_MESSAGE "Invalid quirk."
#define FAILED_GRAPHICS_INITIALIZATION_EXCEPTION_MESSAGE "Failed to initialize graphics."
#define INVALID_FONT_EXCEPTION_MESSAGE "Invalid font."
#define INVALID_CLOCK_SPEED_EXCEPTION_MESSAGE "Clock speed cannot be less than 1."
#define STACK_UNDERFLOW_EXCEPTION_MESSAGE "Stack underflow occurred during execution."

typedef struct {
    exception_code_t code;
    const char* message;
} exception_t;


exception_t exceptions[] = {
    { INVALID_INSTRUCTION_EXCEPTION, INVALID_INSTRUCTION_EXCEPTION_MESSAGE },
    { TOO_MANY_LABELS_EXCEPTION, TOO_MANY_LABELS_EXCEPTION_MESSAGE },
    { STACK_OVERFLOW_EXCEPTION, STACK_OVERFLOW_EXCEPTION_MESSAGE },
    { INVALID_ARGUMENT_EXCEPTION, INVALID_ARGUMENT_EXCEPTION_MESSAGE },
    { DUPLICATE_LABEL_EXCEPTION, DUPLICATE_LABEL_EXCEPTION_MESSAGE },
    { INVALID_SYMBOL_EXCEPTION, INVALID_SYMBOL_EXCEPTION_MESSAGE },
    { MEMORY_ALLOCATION_EXCEPTION, MEMORY_ALLOCATION_EXCEPTION_MESSAGE },
    { UNKNOWN_EXCEPTION, UNKNOWN_EXCEPTION_MESSAGE },
    { TOO_MANY_SYMBOLS_EXCEPTION, TOO_MANY_SYMBOLS_EXCEPTION_MESSAGE },
    { LOAD_FILE_FAILURE_EXCEPTION, LOAD_FILE_FAILURE_EXCEPTION_MESSAGE },
    { FILE_TOO_BIG_EXCEPTION, FILE_TOO_BIG_EXCEPTION_MESSAGE },
    { INVALID_COLOR_PALETTE_EXCEPTION, INVALID_COLOR_PALETTE_EXCEPTION_MESSAGE },
    { INVALID_QUIRK_EXCEPTION, INVALID_QUIRK_EXCEPTION_MESSAGE },
    { FAILED_GRAPHICS_INITIALIZATION_EXCEPTION, FAILED_GRAPHICS_INITIALIZATION_EXCEPTION_MESSAGE },
    { INVALID_FONT_EXCEPTION, INVALID_FONT_EXCEPTION_MESSAGE },
    { INVALID_CLOCK_SPEED_EXCEPTION, INVALID_CLOCK_SPEED_EXCEPTION_MESSAGE },
    { STACK_UNDERFLOW_EXCEPTION, STACK_UNDERFLOW_EXCEPTION_MESSAGE },
};

char c8_exception[EXCEPTION_MESSAGE_SIZE];

void handle_exception(int code) {
    for (size_t i = 0; i < sizeof(exceptions) / sizeof(exception_t); i++) {
        if (exceptions[i].code == code) {
            fprintf(stderr, "%s\n", exceptions[i].message);
        }
    }

    fprintf(stderr, "%s\n", c8_exception);

    #ifndef TEST
    exit(code);
    #endif
}
