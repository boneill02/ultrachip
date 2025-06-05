#ifndef CHIP8_ENCODE_H
#define CHIP8_ENCODE_H

#include <stdint.h>

#define MAX_LINE_LENGTH 100
#define INSTRUCTION_COUNT 64

#define S_CLS "CLS"
#define S_RET "RET"
#define S_JP "JP"
#define S_CALL "CALL"
#define S_SE "SE"
#define S_SNE "SNE"
#define S_LD "LD"
#define S_ADD "ADD"
#define S_OR "OR"
#define S_AND "AND"
#define S_SUB "SUB"
#define S_SHR "SHR"
#define S_SUBN "SUBN"
#define S_SHL "SHL"
#define S_RND "RND"
#define S_DRW "DRW"
#define S_SKP "SKP"
#define S_SKNP "SKNP"
#define S_XOR "XOR"

enum Instruction {
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
};

enum Parameter {
    P_IMM,
    P_LABEL,
    P_STRING,
    P_INS,
    P_I,
    P_IP,
    P_V,
    P_DT,
    P_ST,
    P_K,
    P_B,
    P_F
};

const char *instructionStrings[] = {
    S_CLS,
    S_RET,
    S_JP,
    S_CALL,
    S_SE,
    S_SNE,
    S_LD,
    S_ADD,
    S_OR,
    S_AND,
    S_SUB,
    S_SHR,
    S_SUBN,
    S_SHL,
    S_RND,
    S_DRW,
    S_SKP,
    S_SKNP,
    S_XOR,
};

typedef struct instruction_format_s {
    const char *s;
    uint16_t base;
    int pcount;
    enum Parameter ptype[3];
    uint16_t pmask[3];
} instruction_format_t;

typedef struct instruction_s {
    char *s;
    int line;
    enum Instruction cmd;
    int pcount;
    enum Parameter ptype[3];
    union Param p[3];
    instruction_format_t *format;
} instruction_t;

uint16_t encode_instruction(instruction_t *);

#endif
