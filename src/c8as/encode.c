#include "encode.h"

#include "util/util.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 100
#define INSTRUCTION_COUNT 64
#define SYMBOL_COUNT 64
#define LABEL_COUNT 64

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
#define S_K "K"
#define S_F "F"
#define S_B "B"
#define S_DT "DT"
#define S_ST "ST"
#define S_I "I"
#define S_IP "[I]"
#define S_DB "DB"
#define S_DW "DW"

/**
 * @enum Instruction
 * @brief Represents instruction types
 * 
 * This enumeration defines all possible CHIP-8 instructions.
 */
typedef enum {
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
} Instruction;

/**
 * @enum Symbol
 * @brief Represents symbol types
 * 
 * This enumeration defines all symbol types found during the first assembler
 * pass.
 * 
 * NOTE: values before label need to be kept in same order as identifierStrings
 */
typedef enum {
    SYM_DT = 0,
    SYM_ST,
    SYM_I,
    SYM_IP,
    SYM_K,
    SYM_F,
    SYM_B,
    SYM_DB,
    SYM_DW,
    SYM_LABEL,
    SYM_INT,
    SYM_STRING,
    SYM_V,
    SYM_INSTRUCTION,
    SYM_LABEL_DEFINITION,
} Symbol;

typedef struct {
    Symbol type;
    uint16_t value;
    int ln;
} symbol_t;

symbol_t *symbols;

/**
 * @enum Parameter
 * @brief Represents encoded parameter types
 * 
 * After the first pass, instruction_ts are generated for each instruction.
 * 
 * This enumeration represents all possible parameters that can be directly
 * encoded into the instruction. In the second pass, instruction_ts are generated
 * from the symbols, converting labels into their respective immediate values, etc.
 */
typedef enum {
    P_IMM,
    P_I,
    P_IP,
    P_V,
    P_DT,
    P_ST,
    P_K,
    P_B,
    P_F
} Parameter;

/**
 * @struct instruction_format_t
 * @brief Represents a valid instruction format
 * 
 * instruction_t's are checked against instruction_format_t's to verify
 * that they will produce valid instructions.
 */
typedef struct instruction_format_s {
    const char *s;
    uint16_t base;
    int pcount;
    Parameter ptype[3];
    uint16_t pmask[3];
} instruction_format_t;

/**
 * @struct instruction_t
 * @brief Represents an instruction
 * 
 * During the second pass, this structure is used to verify the instruction's
 * validity and generate the bytecode.
 */
typedef struct instruction_s {
    char *s;
    int line;
    Instruction cmd;
    int pcount;
    Parameter ptype[3];
    int p[3];
    instruction_format_t *format;
} instruction_t;

static uint16_t encode_instruction(instruction_t *);
static void error(char *, int);
static int find_label(char *);
static int get_line(char *, char *, int);
static int index_of(char *, int, char);
static int is_arithmetic(char);
static int is_comment(char *, int);
static int is_instruction(char *);
static int is_label_definition(char *, int);
static int is_register(char *);
static int is_reserved_identifier(char *s);
static symbol_t *next_symbol(void);
static char *next_word(char *, int);
static void parse_line(char *, int);
static void reallocate_instructions(void);
static void reallocate_symbols(void);
static int shift(uint16_t);
static char *trim_comment(char *, int);

instruction_format_t formats[] = {
    { S_CLS,  0x00E0, 0, {},                 {} },
    { S_RET,  0x00EE, 0, {},                 {} },
    { S_CALL, 0x2000, 1,  {P_IMM},           {0x0FFF} },
    { S_SE,   0x3000, 2,  {P_V, P_IMM},      {0x0F00, 0x00FF} },
    { S_SNE,  0x4000, 2,  {P_V, P_IMM},      {0x0F00, 0x00FF} },
    { S_SE,   0x5000, 2,  {P_V, P_V},        {0x0F00, 0x00F0} },
    { S_LD,   0x6000, 2,  {P_V, P_V},        {0x0F00, 0x00F0} },
    { S_ADD,  0x7000, 2,  {P_V, P_IMM},      {0x0F00, 0x00FF} },
    { S_LD,   0x8000, 2,  {P_V, P_V},        {0x0F00, 0x00F0} },
    { S_OR,   0x8001, 2,  {P_V, P_V},        {0x0F00, 0x00F0} },
    { S_AND,  0x8002, 2,  {P_V, P_V},        {0x0F00, 0x00F0} },
    { S_XOR,  0x8003, 2,  {P_V, P_V},        {0x0F00, 0x00F0} },
    { S_ADD,  0x8004, 2,  {P_V, P_V},        {0x0F00, 0x00F0} },
    { S_SUB,  0x8005, 2,  {P_V, P_V},        {0x0F00, 0x00F0} },
    { S_SHR,  0x8006, 1,  {P_V},             {0x0F00} },
    { S_SUBN, 0x8007, 2,  {P_V, P_V},        {0x0F00, 0x00F0} },
    { S_SHL,  0x800E,  1, {P_V},             {0x0F00} },
    { S_SNE,  0x9000,  2, {P_V, P_V},        {0x0F00} },
    { S_LD,   0xA000,  2, {P_I, P_IMM},      {0x0000, 0x0FFF} },
    { S_JP,   0xB000,  2, {P_V, P_IMM},      {0x0000, 0x0FFF} },
    { S_RND,  0xC000,  2, {P_V, P_IMM},      {0x0F00, 0x00FF} },
    { S_DRW,  0xD000,  3, {P_V, P_V, P_IMM}, {0x0F00, 0x00FF} },
    { S_SKP,  0xE09E,  1, {P_V},             {0x0F00} },
    { S_SKNP, 0xE0A1, 1,  {P_V},             {0x0F00} },
    { S_LD,   0xF007, 2,  {P_V, P_DT},       {0x0F00, 0x0000} },
    { S_LD,   0xF00A, 2,  {P_V, P_K},        {0x0F00, 0x0000} },
    { S_LD,   0xF015, 2,  {P_DT, P_V},       {0x0000, 0x0F00} },
    { S_LD,   0xF018, 2,  {P_ST, P_V},       {0x0000, 0x0F00} },
    { S_ADD,  0xF01E, 2,  {P_I, P_V},        {0x0000, 0x0F00} },
    { S_LD,  0xF029, 2,   {P_F, P_V},        {0x0000, 0x0F00} },
    { S_LD,  0xF033, 2,   {P_B, P_V},        {0x0000, 0x0F00} },
    { S_LD,  0xF055, 2,   {P_IP, P_V},       {0x0000, 0x0F00} },
    { S_LD,  0xF065, 2,   {P_V, P_IP},       {0x0F00, 0x0000} },
    { NULL,  0,      0,   {},                {} },
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
    NULL
};

const char *identifierStrings[] = {
    S_DT,
    S_ST,
    S_I,
    S_IP,
    S_K,
    S_F,
    S_B,
    S_DB,
    S_DW,
    NULL,
};

instruction_t *instructions;
int instructionCount = 0;
int instructionCeiling = INSTRUCTION_COUNT;

symbol_t *symbols;
int symbolCount = 1;
int symbolCeiling = SYMBOL_COUNT;

const char *labels[LABEL_COUNT];

/**
 * @brief Returns the bytecode for the given instruction
 * 
 * This function assumes the instruction is complete and valid.
 * 
 * @param ins the instruction to encode
 * @return encoded instruction
 */
static uint16_t encode_instruction(instruction_t *ins) {
    uint16_t result = ins->format->base;

    for (int i = 0; i < ins->pcount; i++) {
        result |= ins->p[i] << shift(ins->format->pmask[i]);
    }

    return result;
}

/**
 * @brief Print error string
 * 
 * If ln>0, it will print the line number as well.
 * 
 * @param s the error message to print
 * @param ln the line number (pass 0 if irrelevant)
 */
void error(char *s, int ln) {
    if (ln) {
        fprintf(stderr, "Error (line %d): %s\n", ln, s);
    } else {
        fprintf(stderr, "Error: %s\n", s);
    }
}

static int find_label(char *s) {
    for (int i = 0; i < LABEL_COUNT; i++) {
        if (labels[i]) {
            if (!strcmp(s, labels[i])) {
                return i;
            }
        } else {
            labels[i] = s;
            return i;
        }
    }

    return 0; // Unreachable
}

/**
 * @brief Copy line from src to dest
 * 
 * This will copy characters from src to dest until a newline is reached,
 * or len characters were copied.
 * 
 * @param dest string to copy to
 * @param src string to copy from
 * @param len maximum number of characters to copy
 * @return 1 if newline reached, 0 otherwise
 */
static int get_line(char *dest, char *src, int len) {
    int c = 0;
    while (c < len && src[c] != '\n') {
        c++;
    }
    strncpy(dest, src, c);

    return c != len;
}

/**
 * @brief Get index of c in s
 * 
 * Returns the first index of c in len. If c is not found, -1 is returned.
 * 
 * @param s string to search
 * @param len length to search
 * @param c char to search for
 * @return index if found, -1 if not found
 */
static int index_of(char *s, int len, char c) {
    int i = 0;
    while (i < len && s[i] != c) {
        s++;
    }

    return i == len ? -1 : i;
}

/**
 * @brief Check if given character is an arithmetic character
 * 
 * @param c character to check
 * @return 1 if true, 0 if false
 */
static int is_arithmetic(char c) {
    if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%') {
        return 1;
    }
    return 0;
}

/**
 * @brief Check if the given string is a comment
 * 
 * @param s the string to check
 * @param len the length of the string
 * @return 1 if true, 0 if false
 */
static int is_comment(char *s, int len) {
    int i = 0;
    for (; i < len; isspace(*s)) {
        s++;
        i++;
    }

    return i < len && *s == ';';
}

/**
 * @brief Check if the given string is an instruction
 * 
 * @param s the string to check
 * @param len length of the string
 * @return instruction enumerator if true, -1 if false
 */
static int is_instruction(char *s) {
    for (int i = 0; instructionStrings[i]; i++) {
        if (!strcmp(s, instructionStrings[i])) {
            return i;
        }
    }

    return -1;
}

/**
 * @brief Check if the given string is a label
 * 
 * @param s the string to check
 * @param len the length of the string
 * @return 1 if true, 0 if false
 */
static int is_label_definition(char *s, int len) {
    if (len < 2) {
        return 0;
    }
    return s[len-1] == ':';
}

static int is_register(char *s) {
    if (*s == 'V') {
        return parse_int(&s[1]);
    }

    return -1;
}

/**
 * @brief Check if given string is a reserved identifier
 */
static int is_reserved_identifier(char *s) {
    for (int i = 0; identifierStrings[i]; i++) {
        if (!strcmp(s, identifierStrings[i])) {
            return i;
        }
    }

    return -1;
}

/**
 * @brief Get the pointer to the next word in the given string
 * 
 * @param s the string to check
 * @param len the length of the string
 * @return NULL if no more words, pointer to next word otherwise
 */
static char *next_word(char *s, int len) {
    int i = 0;
    int j = 0;
    if (isspace(*s)) {
        for (i = 0; i < len; i++) {
            s++;
            if (!isspace(*s)) {
                return s;
            } else {
                *s = '\0';
            }
        }
        return NULL;
    }

    for (i = 0; i < len; i++) {
        s++;
        if (!j && isspace(*s)) {
            *s = '\0';
            j++;
        } else if (j && !isspace(*s)) {
            return s;
        }
    }
    return NULL;
}

/**
 * @brief Parse the given string
 */
void parse(char *s) {
    symbols = malloc(sizeof(symbol_t) * SYMBOL_COUNT);
    instructions = malloc(sizeof(instruction_t) * INSTRUCTION_COUNT);
    char **lines = malloc(500 * sizeof(char *)); // TODO fix
    int len = strlen(s);
    int lineCount = 1;
    int newline = 0;

    lines[0] = strtok(s, "\n");
    for (int i = 1; i < 500; i++) {
        if ((lines[lineCount] = strtok(NULL, "\n"))) {
            lineCount++;
        } else {
            break;
        }
    }

    /* First pass */
    for (int i = 0; i < lineCount; i++) {
        parse_line(lines[i], i+1);
    }

    for (int i = 0; i < symbolCount; i++) {
        printf("%d %d %d\n", symbols[i].ln, symbols[i].type, symbols[i].value);
    }

    /* Second pass */

    free(symbols);
    free(instructions);
    free(lines);
}

/**
 * @brief Parse the given line
 */
static void parse_line(char *s, int ln) {
    if (is_comment(s, strlen(s))) return;
    int len = strlen(s);
    char **words = malloc(64 * sizeof(char *));
    int wordCount;
    int value;
    
    symbol_t *sym = &symbols[symbolCount];

    s = trim_comment(s, len);

    words[0] = strtok(s, ", ");
    for (int i = 1; i < 64; i++) {
        if ((words[i] = strtok(NULL, ", "))) {
            wordCount++;
        } else {
            break;
        }
    }

    for (int i = 0; i < wordCount; i++) {
        printf("%s\n", words[i]);
        sym->ln = ln;
        if ((value = is_instruction(words[i])) != -1) {
            sym->type = SYM_INSTRUCTION;
            sym->value = value;
        } else if (is_label_definition(s, strlen(s))) {
            sym->type = SYM_LABEL_DEFINITION;
            sym->value = find_label(s);
        } else if (is_register(s)) {
            sym->type = SYM_V;
        } else if ((value = is_reserved_identifier(s) != -1)) {
            sym->type = value;
        } else if ((value = parse_int(s))) {
            sym->type = SYM_INT;
            sym->value = value;
        } else {
            sym->type = SYM_LABEL;
            sym->value = find_label(s);
            sym = next_symbol();
        }
        sym = next_symbol();
    }

    free(words);
}

static symbol_t *next_symbol(void) {
    symbolCount++;
    if (symbolCount == symbolCeiling) {
        reallocate_symbols();
    }

    return &symbols[symbolCount - 1];
}

/**
 * @brief Generate ptype and p for parameter pn in ins from s
 * 
 * @param ins instruction to populate parameter
 * @param pn parameter number to populate
 * @param s parameter string
 * @param len parameter string length
 */
static void parse_parameter(instruction_t *ins, int pn, char *s, int len) {
    int intValue = parse_int(s);;
    if (*s == 'V') {
        ins->ptype[pn] = P_V;
        ins->p[pn] = parse_int(s+1);
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
        ins->p[pn] = intValue;
    }
}

/**
 * @brief Expand instructions array
 */
static void reallocate_instructions(void) {
    instruction_t *oldins = instructions;
    instructions = malloc(sizeof(instruction_t) * instructionCeiling + INSTRUCTION_COUNT);
    memcpy(instructions, oldins, instructionCeiling);
    instructionCeiling += INSTRUCTION_COUNT;
    free(oldins);
}

static void reallocate_symbols(void) {
    symbol_t *oldsym = symbols;
    instructions = malloc(sizeof(symbol_t) * symbolCeiling + SYMBOL_COUNT);
    memcpy(symbols, oldsym, symbolCeiling);
    symbolCeiling += SYMBOL_COUNT;
    free(oldsym);
}

/**
 * @brief Validate the given instruction's format
 * 
 * Finds a matching instruction_format_t for the given instruction and assigns
 * it to the given instruction.
 * 
 * @param ins instruction to validate
 * @return 1 if valid, 0 otherwise
 */
static int set_instruction_format(instruction_t *ins) {
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

/**
 * @brief Find the bits needed to shift to OR something into mask
 * 
 * FIXME find a better way to do this without having to do this every
 * time an instruction is encoded
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
 * @brief Trim and remove comment from line if exists
 * 
 * @param s string to trim
 * @param len length of string
 * @return trimmed string
 */
static char *trim_comment(char *s, int len) {
    char *s2;
    s = trim(s);
    s2 = s;

    while (!is_comment(s2, len--) && len > 0) {
        s2++;
    }

    if (len == 0) {
        *s2 = '\0';
    }
    return s;
}