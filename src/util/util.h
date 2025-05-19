#ifndef CHIP8_UTIL_H
#define CHIP8_UTIL_H

#include <stdbool.h>

bool *get_pixel(bool *, int, int);
int hex_to_int(char);
int parse_int(char *);
void print_version(const char *argv0);
char *trim(char *);

#endif