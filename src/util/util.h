/**
 * @file util.h
 *
 * Miscellaneous useful functions.
 */

#ifndef CHIP8_UTIL_H
#define CHIP8_UTIL_H

#include "util/defs.h"

#include <stdio.h>

int hex_to_int(char);
int parse_int(const char *);
void print_version(const char *);
char *trim(char *);

#endif
