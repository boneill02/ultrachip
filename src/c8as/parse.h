#ifndef CHIP8_PARSE_H
#define CHIP8_PARSE_H

#include <stdint.h>
#include <stdio.h>

#define ARG_VERBOSE 1

#define MAX_LINE_LENGTH 100
#define MAX_WORDS 100
#define MAX_LINES 100

int parse(const char *, uint8_t *, int);
char *remove_comment(char *);

#endif