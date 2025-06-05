#include "encode.h"

#include "util/util.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define E_TOO_MANY_ARGUMENTS "Too many arguments"

static void *clean_malloc(size_t s);
static void clean_exit(int code);
static int get_line(char *, char *, int);
static int index_of(char *, int, char);
static int is_arithmetic(char);
static int is_comment(char *, int);
static int is_instruction(char *, int);
static int is_label(char *, int);
static int next_word(char *, int);
static void parse_parameters(instruction_t *, int, char *, int);
static void parse_line(char *, int);
static void error(char *, int);
static void reallocateInstructions(void);
static int shift(uint16_t);

static instruction_t *instructions;
int instructionCount = 0;
int instructionCeiling = INSTRUCTION_COUNT;

instruction_format_t formats[] = {
    { S_CLS,  0x00E0, 0, {},                    {} },
    { S_RET,  0x00EE, 0, {},                    {} },
    { S_CALL, 0x2000, 1, {P_IMM},              {0x0FFF} },
    { S_SE,   0x3000, 2, {P_V, P_IMM},        {0x0F00, 0x00FF} },
    { S_SNE,  0x4000, 2, {P_V, P_IMM},        {0x0F00, 0x00FF} },
    { S_SE,   0x5000, 2, {P_V, P_V},          {0x0F00, 0x00F0} },
    { S_LD,   0x6000, 2, {P_V, P_V},          {0x0F00, 0x00F0} },
    { S_ADD,  0x7000, 2, {P_V, P_IMM},        {0x0F00, 0x00FF} },
    { S_LD,   0x8000, 2, {P_V, P_V},          {0x0F00, 0x00F0} },
    { S_OR,   0x8001, 2, {P_V, P_V},          {0x0F00, 0x00F0} },
    { S_AND,  0x8002, 2, {P_V, P_V},          {0x0F00, 0x00F0} },
    { S_XOR,  0x8003, 2, {P_V, P_V},          {0x0F00, 0x00F0} },
    { S_ADD,  0x8004, 2, {P_V, P_V},          {0x0F00, 0x00F0} },
    { S_SUB,  0x8005, 2, {P_V, P_V},          {0x0F00, 0x00F0} },
    { S_SHR,  0x8006, 1, {P_V},               {0x0F00} },
    { S_SUBN, 0x8007, 2, {P_V, P_V},          {0x0F00, 0x00F0} },
    { S_SHL,  0x800E,  1, {P_V},              {0x0F00} },
    { S_SNE,  0x9000,  2, {P_V, P_V},         {0x0F00} },
    { S_LD,   0xA000,  2, {P_I, P_IMM},       {0x0000, 0x0FFF} },
    { S_JP,   0xB000,  2, {P_V, P_IMM},       {0x0000, 0x0FFF} },
    { S_RND,  0xC000,  2, {P_V, P_IMM},       {0x0F00, 0x00FF} },
    { S_DRW,  0xD000,  3, {P_V, P_V, P_IMM},  {0x0F00, 0x00FF} },
    { S_SKP,  0xE09E,  1, {P_V},              {0x0F00} },
    { S_SKNP, 0xE0A1, 1,  {P_V},              {0x0F00} },
    { S_LD,   0xF007, 2,  {P_V, P_DT},        {0x0F00, 0x0000} },
    { S_LD,   0xF00A, 2,  {P_V, P_K},         {0x0F00, 0x0000} },
    { S_LD,   0xF015, 2,  {P_DT, P_V},        {0x0000, 0x0F00} },
    { S_LD,   0xF018, 2,  {P_ST, P_V},        {0x0000, 0x0F00} },
    { S_ADD,  0xF01E, 2,  {P_I, P_V},         {0x0000, 0x0F00} },
    { S_LD,  0xF029, 2,  {P_F, P_V},          {0x0000, 0x0F00} },
    { S_LD,  0xF033, 2,  {P_B, P_V},          {0x0000, 0x0F00} },
    { S_LD,  0xF055, 2,  {P_IP, P_V},         {0x0000, 0x0F00} },
    { S_LD,  0xF065, 2,  {P_V, P_IP},         {0x0F00, 0x0000} },
    { NULL,  0,      0,  {},                  {} },
};

uint16_t encode_instruction(instruction_t *ins) {
    uint16_t result = ins->format->base;

    for (int i = 0; i < ins->pcount; i++) {
        result |= ins->p[i].i << shift(ins->format->pmask[i]);
    }

    return result;
}

static int setInstructionFormat(instruction_t *ins) {
    instruction_format_t f;
    int j;

    for (int i = 0; formats[i].s != NULL; i++) {
        f = formats[i];
        if (!strncmp(ins->s, f.s, strlen(f.s)) && ins->pcount == f.pcount) {
            for (j = 0; j < ins->pcount; j++) {
                if (ins->ptype[j] != f.ptype[j]) {
                    break;
                }
            }

            if (j == ins->pcount) {
                ins->format = &formats[i];
                return 1;
            }
        }
    }

    return 0;
}

static int shift(uint16_t mask) {
    int shift = 0;
    while ((mask & 1) == 0) {
        mask >>= 1;
        shift++;
    }
    return shift;
}

static void clean_exit(int code) {
    if (instructions) {
        free(instructions);
    }

    exit(code);
}

static void *clean_malloc(size_t s) {
    void *a;
    if (!(a = malloc(s))) {
        fprintf("Error: Failed to allocate memory\n");
        clean_exit(1);
    }
    return a;
}

void error(char *s, int ln) {
    if (ln) {
        fprintf(stderr, "Error (line %d): %s", ln, s);
    }
}

static int is_arithmetic(char s) {
    if (s == '+' || s == '-' || s == '*' || s == '/' || s == '%') {
        return 1;
    }
    return 0;
}

static int is_comment(char *s, int len) {
    int i = 0;
    for (; i < len; isspace(*s)) {
        s++;
        i++;
    }

    return i < len && *s == ';';
}

static int is_instruction(char *s, int len) {
    for (int i = 0; i < 18; i++) {
        if (!strncmp(s, instructionStrings[i], strlen(instructionStrings[i]))) {
            return i;
        }
    }

    return -1;
}

static int is_label(char *s, int len) {
    if (len < 2) {
        return 0;
    }
    return s[len-1] == ':';
}

static int get_line(char *dest, char *src, int len) {
    int c = 0;
    while (c < len && src[c] != '\n') {
        c++;
    }
    strncpy(dest, src, c);

    return c != len;
}

static int index_of(char *s, int len, char c) {
    int i = 0;
    while (i < len && s[i] != c) {
        s++;
    }

    return i == len ? -1 : i;
}

static int next_word(char *s, int len) {
    int i = 0;
    int j = 0;
    for (i = 0; i < len; i++) {
        if (j && !isspace(*s)) {
            break;
        } else if (!j && isspace(*s)) {
            j++;
        }
        s++;
    }

    return i == len ? -1 : i;
}

static void parse_parameters(instruction_t *ins, int pn, char *s, int len) {
    int intValue;
    if (*s == 'V') {
        ins->ptype[pn] = P_V;
        ins->p[pn].i = parse_int(s+1);
    } else if (*s == 'I') {
        ins->ptype[pn] = P_I;
    } else if (!strncmp(s, "DT", 2)) {
        // TODO check for len < 2
        ins->ptype[pn] = P_DT;
    } else if (!strncmp(s, "ST", 2)) {
        // TODO check for len < 2
        ins->ptype[pn] = P_ST;
    } else if ((intValue = parse_int(s))) {
        ins->ptype[pn] = P_IMM;
        // TODO throw error if immediate too big
        ins->p[pn].i = intValue;
    } else {
        ins->ptype[pn] = P_LABEL;
        // TODO implement, dynamically store labels?
    }
}

static void parse_line(char *s, int ln) {
    instruction_t *ins = &instructions[instructionCount];
    instruction_t *oldins;
    int idx = 0;
    int len = strlen(s);
    char buf[64];

    if (!is_comment(s, len)) {
        ins->line = ln;

        if ((ins->cmd = is_instruction(s, len)) == -1) {
            return;
        }

        /* get args */
        while ((idx = next_word(s, len)) != NULL) {
            if (ins->pcount > 2) {
                // TODO throw too many args error
                break;
            }
            len -= idx;
            s += idx;

            parse_arg(s, len, ins, ins->pcount);
            ins->pcount++;
        }
    }

    instructionCount++;

    if (instructionCount == instructionCeiling) {
        reallocateInstructions();
    }
}

static void reallocateInstructions(void) {
    instruction_t *oldins = instructions;
    instructions = clean_malloc(sizeof(instruction_t) * instructionCeiling + INSTRUCTION_COUNT);
    memcpy(instructions, oldins, instructionCeiling);
    instructionCeiling += INSTRUCTION_COUNT;
    free(oldins);
}


void parse(char *s) {
    int len = strlen(s);
    char line[MAX_LINE_LENGTH];
    int ln = 1;

    instructions = (instruction_t *) malloc(INSTRUCTION_COUNT * sizeof(instruction_t));

    while (get_line(line, s, len)) {
        parse_line(line, ln);
        ln++;
    }
}