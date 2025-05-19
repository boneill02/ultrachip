#ifndef CHIP8_UTIL_H
#define CHIP8_UTIL_H

bool *get_pixel(bool *, int, int);
int hex_to_int(char);
int parse_int(char *);
char *trim(char *);

#endif