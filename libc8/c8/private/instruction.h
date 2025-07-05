/**
 * @file c8/private/instruction.h
 * @note NOT EXPORTED
 *
 * This file contains the declaration of the instruction parsing and
 * execution functions for the CHIP-8 interpreter.
 */

#ifndef LIBC8_INSTRUCTION_H
#define LIBC8_INSTRUCTION_H

#include "c8/chip8.h"

int parse_instruction(c8_t* c8);

#endif