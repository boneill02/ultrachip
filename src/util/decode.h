/**
 * @file decode.h
 *
 * Stuff for disassembling bytecode.
 */

#ifndef DECODE_H
#define DECODE_H

#include <stdint.h>

char *decode_instruction(uint16_t, uint8_t *);
uint16_t jump(uint16_t);

#endif
