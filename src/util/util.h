#ifndef CHIP8_UTIL_H
#define CHIP8_UTIL_H

#include "util/defs.h"

#include <stdio.h>

int hex_to_int(char);
int parse_int(const char *);
void print_version(const char *);
void *safe_calloc(size_t, size_t);
void safe_free(void *);
void safe_fclose(FILE *);
FILE *safe_fopen(const char *, const char *);
void *safe_malloc(size_t);
void *safe_realloc(void *, size_t);
void safe_exit(int);
char *trim(char *);

extern char exception[EXCEPTION_MESSAGE_SIZE];

#endif
