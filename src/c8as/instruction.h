#ifndef CHIP8_INSTRUCTION_H
#define CHIP8_INSTRUCTION_H

#include "symbol.h"

#include <stdint.h>

#define INSTRUCTION_COUNT 64

/**
 * @enum Instruction
 * @brief Represents instruction types
 * 
 * This enumeration defines all possible CHIP-8 instructions.
 */
typedef enum {
    I_NULL = -1,
    I_CLS,
    I_RET,
    I_JP,
    I_CALL,
    I_SE,
    I_SNE,
    I_LD,
    I_ADD,
    I_OR,
    I_AND,
    I_SUB,
    I_SHR,
    I_SUBN,
    I_SHL,
    I_RND,
    I_DRW,
    I_SKP,
    I_SKNP,
    I_XOR,
} Instruction;

/**
 * @struct instruction_format_t
 * @brief Represents a valid instruction format
 * 
 * instruction_t's are checked against instruction_format_t's to verify
 * that they will produce valid instructions.
 */
typedef struct {
    Instruction cmd;
    uint16_t base;
    int pcount;
    Symbol ptype[3];
    uint16_t pmask[3];
} instruction_format_t;

/**
 * @struct instruction_t
 * @brief Represents an instruction
 * 
 * During the second pass, this structure is used to verify the instruction's
 * validity and generate the bytecode.
 */
typedef struct {
    int line;
    Instruction cmd;
    int pcount;
    Symbol ptype[3];
    int p[3];
    instruction_format_t *format;
} instruction_t;

extern const char *instructionStrings[];
extern instruction_format_t formats[];

uint16_t build_instruction(instruction_t *, symbol_list_t *, int);

#endif