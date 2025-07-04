/**
 * @file c8/encode.h
 *
 * Base assembler code
 */

#ifndef LIBC8_PARSE_H
#define LIBC8_PARSE_H

#include <stdint.h>

#define ARG_VERBOSE 1

#define C8_ENCODE_MAX_LINE_LENGTH 100
#define C8_ENCODE_MAX_WORDS 100
#define C8_ENCODE_MAX_LINES 500

extern char **c8_lines;
extern char **c8_lines_unformatted;
extern int c8_line_count;

int c8_encode(const char*, uint8_t*, int);
char* remove_comment(char*);

#endif
