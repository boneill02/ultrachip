/**
 * @file dis.h
 *
 * Disassembler code.
 */

#ifndef CHIP8_DIS_H
#define CHIP8_DIS_H

#include <stdio.h>

#define ARG_PRINT_ADDRESSES 0x1
#define ARG_DEFINE_LABELS 0x2

void disassemble(FILE *, FILE *, int);

#endif
