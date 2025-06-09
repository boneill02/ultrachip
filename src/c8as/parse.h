#ifndef CHIP8_PARSE_H
#define CHIP8_PARSE_H

#include <stdio.h>

#define ARG_VERBOSE 1

#define MAX_LINE_LENGTH 100
#define MAX_WORDS 100
#define MAX_LINES 100

void parse(char *, FILE *, int);
char *trim_comment(char *);

#endif