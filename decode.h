#ifndef DECODE_H
#define DECODE_H

#include <stdint.h>

uint16_t jump(uint16_t);
char *decode_instruction(uint16_t, uint8_t *);

#endif
