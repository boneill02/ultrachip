#ifndef DECODE_H
#define DECODE_H

#include <stdint.h>

uint16_t jump(uint16_t in);
char *decode_instruction(uint16_t in);

#endif
