/**
 * @file exception.c
 *
 * Stuff for safe handling of files and mallocs, and debug-friendly error codes
 * and messages when something goes wrong.
 */

#include "exception.h"

#include "../defs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char c8_exception[EXCEPTION_MESSAGE_SIZE];
char exception_messages[19][EXCEPTION_MESSAGE_SIZE] = {
	UNKNOWN_EXCEPTION_MESSAGE,
	NULL_ARGUMENT_EXCEPTION_MESSAGE,
	INVALID_INSTRUCTION_EXCEPTION_MESSAGE,
	TOO_MANY_LABELS_EXCEPTION_MESSAGE,
	STACK_OVERFLOW_EXCEPTION_MESSAGE,
	INVALID_ARGUMENT_EXCEPTION_MESSAGE,
	INVALID_ARGUMENT_EXCEPTION_INTERNAL_MESSAGE,
	DUPLICATE_LABEL_EXCEPTION_MESSAGE,
	INVALID_SYMBOL_EXCEPTION_MESSAGE,
	TOO_MANY_MALLOCS_EXCEPTION_MESSAGE,
	UNKNOWN_EXCEPTION_MESSAGE,
	TOO_MANY_SYMBOLS_EXCEPTION_MESSAGE,
	LOAD_FILE_FAILURE_EXCEPTION_MESSAGE,
	FILE_TOO_BIG_EXCEPTION_MESSAGE,
	INVALID_COLOR_PALETTE_EXCEPTION_MESSAGE,
	INVALID_QUIRK_EXCEPTION_MESSAGE,
	FAILED_GRAPHICS_INITIALIZATION_EXCEPTION_MESSAGE,
	INVALID_FONT_EXCEPTION_MESSAGE,
};

int c8_exit_on_exception = 1;

void handle_exception(int code) {
	if (code < -3 && code >= -19) {
		fprintf(stderr, "libc8: %s\n", exception_messages[(code * -1) - 2]);
	}

	if (strlen(c8_exception)) {
		fprintf(stderr, "libc8: %s\n", c8_exception);
	}
	exit(code);
}
