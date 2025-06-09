#include "instruction.h"

#include "symbol.h"

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
    NULL
};

instruction_format_t formats[] = {
    { I_CLS,  0x00E0, 0, {SYM_NULL},              {0} },
    { I_RET,  0x00EE, 0, {SYM_NULL},              {0} },
    { I_CALL, 0x2000, 1, {SYM_INT},               {0x0FFF} },
    { I_SE,   0x3000, 2, {SYM_V, SYM_INT},        {0x0F00, 0x00FF} },
    { I_SNE,  0x4000, 2, {SYM_V, SYM_INT},        {0x0F00, 0x00FF} },
    { I_SE,   0x5000, 2, {SYM_V, SYM_V},          {0x0F00, 0x00F0} },
    { I_LD,   0x6000, 2, {SYM_V, SYM_V},          {0x0F00, 0x00F0} },
    { I_ADD,  0x7000, 2, {SYM_V, SYM_INT},        {0x0F00, 0x00FF} },
    { I_LD,   0x8000, 2, {SYM_V, SYM_V},          {0x0F00, 0x00F0} },
    { I_OR,   0x8001, 2, {SYM_V, SYM_V},          {0x0F00, 0x00F0} },
    { I_AND,  0x8002, 2, {SYM_V, SYM_V},          {0x0F00, 0x00F0} },
    { I_XOR,  0x8003, 2, {SYM_V, SYM_V},          {0x0F00, 0x00F0} },
    { I_ADD,  0x8004, 2, {SYM_V, SYM_V},          {0x0F00, 0x00F0} },
    { I_SUB,  0x8005, 2, {SYM_V, SYM_V},          {0x0F00, 0x00F0} },
    { I_SHR,  0x8006, 1, {SYM_V},                 {0x0F00} },
    { I_SUBN, 0x8007, 2, {SYM_V, SYM_V},          {0x0F00, 0x00F0} },
    { I_SHL,  0x800E, 1, {SYM_V},                 {0x0F00} },
    { I_SNE,  0x9000, 2, {SYM_V, SYM_V},          {0x0F00, 0x00F0} },
    { I_LD,   0xA000, 2, {SYM_I, SYM_INT},        {0x0000, 0x0FFF} },
    { I_JP,   0xB000, 2, {SYM_V, SYM_INT},        {0x0000, 0x0FFF} },
    { I_RND,  0xC000, 2, {SYM_V, SYM_INT},        {0x0F00, 0x00FF} },
    { I_DRW,  0xD000, 3, {SYM_V, SYM_V, SYM_INT}, {0x0F00, 0x00F0, 0x000F} },
    { I_SKP,  0xE09E, 1, {SYM_V},                 {0x0F00} },
    { I_SKNP, 0xE0A1, 1, {SYM_V},                 {0x0F00} },
    { I_LD,   0xF007, 2, {SYM_V, SYM_DT},         {0x0F00, 0x0000} },
    { I_LD,   0xF00A, 2, {SYM_V, SYM_K},          {0x0F00, 0x0000} },
    { I_LD,   0xF015, 2, {SYM_DT, SYM_V},         {0x0000, 0x0F00} },
    { I_LD,   0xF018, 2, {SYM_ST, SYM_V},         {0x0000, 0x0F00} },
    { I_ADD,  0xF01E, 2, {SYM_I, SYM_V},          {0x0000, 0x0F00} },
    { I_LD,   0xF029, 2, {SYM_F, SYM_V},          {0x0000, 0x0F00} },
    { I_LD,   0xF033, 2, {SYM_B, SYM_V},          {0x0000, 0x0F00} },
    { I_LD,   0xF055, 2, {SYM_IP, SYM_V},         {0x0000, 0x0F00} },
    { I_LD,   0xF065, 2, {SYM_V, SYM_IP},         {0x0F00, 0x0000} },
    { I_NULL, 0,      0, {SYM_NULL},              {0} },
};

static uint16_t parse_instruction(instruction_t *);
static int shift(uint16_t fmt);
static int validate_instruction(instruction_t *);

/**
 * @brief Build an instruction from symbols beginning at idx
 * 
 * This function builds an instruction from a completely parsed set of symbols
 * (with labels expanded).
 * 
 * @param ins instruction_t to store instruction contents
 * @param symbols symbol list
 * @param idx symbols index of start of instruction
 * @return instruction bytecode
 */
uint16_t build_instruction(instruction_t *ins, symbol_list_t *symbols, int idx) {
    /* parse instruction command */
    ins->cmd = (Instruction) symbols->s[idx].value;
    ins->line = symbols->s[idx].ln;
    ins->pcount = 0;

    /* parse instruction args */
    idx++;
    for (int i = 0; i < symbols->len - idx; i++) {
        switch (symbols->s[idx + i].type) {
            case SYM_V:
            case SYM_INT:
                ins->p[i] = symbols->s[idx+i].value;
            case SYM_B:
            case SYM_DT:
            case SYM_F:
            case SYM_I:
            case SYM_IP:
            case SYM_K:
            case SYM_ST:
                ins->ptype[i] = symbols->s[idx+i].type;
                ins->pcount++;
                break;
            default:
                i = symbols->len;
                break;
        }
    }

    /* validate instruction */
    if (validate_instruction(ins)) {
        return parse_instruction(ins);
    }

    return 0;
}

/**
 * @brief Get bytecode value of instruction
 * 
 * @param ins instruction to get bytecode of
 * 
 * @return bytecode of instruction ins
 */
static uint16_t parse_instruction(instruction_t *ins) {
    uint16_t result = ins->format->base;
    for (int j = 0; j < ins->pcount; j++) {
        if (ins->format->pmask[j]) {
            result |= ins->p[j] << shift(ins->format->pmask[j]);
        }
    }
    return result;
}

/**
 * @brief Find the bits needed to shift to OR a parameter into an instruction
 * 
 * FIXME find a better way to do this without having to do this every
 * time an instruction is encoded
 * 
 * @return number of bits to shift
 */
static int shift(uint16_t fmt) {
    int shift = 0;
    while ((fmt & 1) == 0) {
        fmt >>= 1;
        shift++;
    }

    return shift;
}

/**
 * @brief Validate the given instruction against legal instruction formats
 * 
 * If successful, ins->format will be populated with the matching format
 * 
 * @param ins instruction to validate
 * 
 * @return 1 if success, 0 if failure
 */
static int validate_instruction(instruction_t *ins) {
    int match;

    for (int i = 0; formats[i].cmd != -1; i++) {
        instruction_format_t *f = &formats[i];
        if (ins->pcount == f->pcount && ins->cmd == f->cmd) {
            match = 1;
            for (int j = 0; j < ins->pcount; j++) {
                if (ins->ptype[j] != f->ptype[j]) {
                    match = 0;
                    break;
                }
            }

            if (match) {
                ins->format = f;
                return 1;
            }
        }
    }

    return 0;
}
