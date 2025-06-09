#ifndef CHIP8_PARSE_H
#define CHIP8_PARSE_H

#include <stdio.h>

#define MAX_LINE_LENGTH 100
#define MAX_WORDS 100
#define MAX_LINES 100

void parse(char *, FILE *);
char *trim_comment(char *);

#endif