/**
 * @file c8/private/util.h
 * @note NOT EXPORTED
 *
 * Miscellaneous useful functions.
 */

#ifndef C8_UTIL_H
#define C8_UTIL_H

#define VERBOSE_PRINT(a, ...) if (a & ARG_VERBOSE) { printf(__VA_ARGS__); }

int hex_to_int(char);
int parse_int(const char*);
char* trim(char*);

#endif
