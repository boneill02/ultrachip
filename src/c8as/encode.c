#include "encode.h"

#include "util/util.h"
#include "util/defs.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 100
#define MAX_WORDS 100
#define MAX_LINES 100
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
    I_XOR,
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
    SYM_DT = 1,
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
 * @struct instruction_format_t
 * @brief Represents a valid instruction format
 * 
 * instruction_t's are checked against instruction_format_t's to verify
 * that they will produce valid instructions.
 */
typedef struct instruction_format_s {
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
typedef struct instruction_s {
    int line;
    Instruction cmd;
    int pcount;
    Symbol ptype[3];
    int p[3];
    instruction_format_t *format;
} instruction_t;

/**
 * @struct label_t
 * @brief Represents a label
 * 
 * Represents a label with a corresponding byte value
 */
typedef struct {
    char identifier[20];
    int byte;
} label_t;

static int build_instruction(int idx);
static void error(char *, int);
static int is_comment(char *, int);
static inline int is_db(char *);
static inline int is_dw(char *);
static int is_instruction(char *);
static int is_label_definition(char *, int);
static int is_register(char *);
static int is_reserved_identifier(char *s);
static symbol_t *next_symbol(void);
static void parse_line(char *, int);
static void reallocate_symbols(void);
static void resolve_labels(void);
static int shift(uint16_t);
static int tokenize(char **, char *, const char *, int);
static void trim_comment(char *, int);
static void write(FILE *);

instruction_format_t formats[] = {
    { I_CLS,  0x00E0, 0, {0},                     {0} },
    { I_RET,  0x00EE, 0, {0},                     {0} },
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
    { I_LD,  0xF029,  2, {SYM_F, SYM_V},          {0x0000, 0x0F00} },
    { I_LD,  0xF033,  2, {SYM_B, SYM_V},          {0x0000, 0x0F00} },
    { I_LD,  0xF055,  2, {SYM_IP, SYM_V},         {0x0000, 0x0F00} },
    { I_LD,  0xF065,  2, {SYM_V, SYM_IP},         {0x0F00, 0x0000} },
    { -1,    0,       0, {0},                     {0} },
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
    "",
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

instruction_t ins;
symbol_t *symbols;
int symbolCount = 0;
int symbolCeiling = SYMBOL_COUNT;

label_t labels[LABEL_COUNT];
int labelCount = 0;

static int build_instruction(int idx) {
    instruction_format_t f;
    int j;

    ins.cmd = symbols[idx].value;
    ins.line = symbols[idx].ln;
    ins.pcount = 0;

    idx++;
    for (int i = 0; i < symbolCount - idx; i++) {
        switch (symbols[idx + i].type) {
            case SYM_V:
            case SYM_INT:
                ins.p[i] = symbols[idx+i].value;
            case SYM_B:
            case SYM_DT:
            case SYM_F:
            case SYM_I:
            case SYM_IP:
            case SYM_K:
            case SYM_ST:
                ins.ptype[i] = symbols[idx+i].type;
                ins.pcount++;
                break;
            default:
                i = symbolCount;
                break;
        }
    }

    int match;
    for (int i = 0; formats[i].cmd != -1; i++) {
        f = formats[i];
        if (ins.pcount == f.pcount && ins.cmd == f.cmd) {
            match = 1;
            for (j = 0; j < ins.pcount; j++) {
                if (ins.ptype[j] != f.ptype[j]) {
                    match = 0;
                    break;
                }
            }

            if (match) {
                uint16_t result = f.base;
                for (int j = 0; j < ins.pcount; j++) {
                    if (f.pmask[j]) {
                        result |= ins.p[j] << shift(f.pmask[j]);
                    }
                }
                return result;
            }
        }
    }

    return 0;
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

static inline int is_db(char *s) {
    return !strcmp(s, S_DB);
}

static inline int is_dw(char *s) {
    return !strcmp(s, S_DW);
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
 * @brief Check if the given string is a label definition
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

/**
 * @brief Check if the given string represents a V register
 * 
 * @param s string to check
 * @return V register number if true, -1 otherwise
 */
static int is_register(char *s) {
    if (*s == 'V' || *s == 'v') {
        return parse_int(s);
    }

    return -1;
}

/**
 * @brief Check if given string is a reserved identifier
 * 
 * @param s string to check
 * @return type of identifier if true, -1 otherwise
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
 * @brief Parse the given string
 * 
 * This generates bytecode from the given assembly code in s and writes the output to f
 * 
 * @param s string containing assembly code
 * @param f file to write to
 */
void parse(char *s, FILE *f) {
    symbols = malloc(sizeof(symbol_t) * SYMBOL_COUNT);
    char **lines = malloc(MAX_LINES * sizeof(char *));

    s = trim(s);
    int lineCount = tokenize(lines, s, "\n", MAX_LINES);

    for (int i = 0; i < lineCount; i++) {
        if (is_label_definition(lines[i], strlen(lines[i]))) {
            labelCount++;
            strcpy(labels[labelCount].identifier, lines[i]);
        }
    }

    for (int i = 0; i < lineCount; i++) {
        parse_line(lines[i], i+1);
    }

    resolve_labels();
    write(f);

    free(symbols);
    free(lines);
}

/**
 * @brief Parse the given line
 * 
 * @param s line string
 * @param ln line number
 */
static void parse_line(char *s, int ln) {
    int len = strlen(s);
    if (is_comment(s, len)) return;

    char **words = malloc(MAX_WORDS * sizeof(char *));
    int wc;
    int value;
    
    symbol_t *sym = &symbols[symbolCount];

    trim_comment(s, len);
    wc = tokenize(words, s, " ", MAX_WORDS);

    for (int i = 0; i < wc; i++) {
        sym->ln = ln;
        if ((value = is_instruction(words[i])) != -1) {
            sym->type = SYM_INSTRUCTION;
            sym->value = value;
        } else if (is_db(s)) {
            sym->type = SYM_DB;
            i++;
            sym->value = parse_int(words[i]);
        } else if (is_dw(s)) {
            sym->type = SYM_DW;
            i++;
            sym->value = parse_int(words[i]);
        } else if (is_label_definition(words[i], strlen(s))) {
            labelCount++;
            strcpy(labels[labelCount].identifier, words[i]);
            sym->type = SYM_LABEL_DEFINITION;
        } else if ((value = is_register(words[i])) != -1) {
            sym->type = SYM_V;
            sym->value = value;
        } else if ((value = is_reserved_identifier(words[i]) != -1)) {
            sym->type = value;
        } else if ((value = parse_int(words[i]))) {
            sym->type = SYM_INT;
            sym->value = value;
        } else {
            sym->type = SYM_LABEL;
        }
        sym = next_symbol();
    }

    free(words);
}

/**
 * @brief Get the next symbol
 * 
 * @return first empty symbol in symbol table
 */
static symbol_t *next_symbol(void) {
    symbolCount++;
    if (symbolCount == symbolCeiling) {
        reallocate_symbols();
    }

    return &symbols[symbolCount];
}

/**
 * @brief Expand symbol array
 */
static void reallocate_symbols(void) {
    symbol_t *oldsym = symbols;
    symbols = malloc(sizeof(symbol_t) * symbolCeiling + SYMBOL_COUNT);
    memcpy(symbols, oldsym, symbolCeiling);
    symbolCeiling += SYMBOL_COUNT;
    free(oldsym);
}

/**
 * @brief Get byte values of labels from completed symbol table
 */
static void resolve_labels(void) {
    int byte = PROG_START;
    int labelIdx = 0;
    for (int i = 0; i < symbolCount; i++) {
        if (labelIdx == labelCount) {
            return;
        }

        switch (symbols[i].type) {
            case SYM_LABEL_DEFINITION:
                labels[labelIdx++].byte = byte;
            case SYM_DB:
                byte += 1;
                break;
            case SYM_INSTRUCTION:
            case SYM_DW:
                byte += 2;
            default:
                break;
        }
    }
}

/**
 * @brief Find the bits needed to shift to OR a parameter into an instruction
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
 * @brief split string into token array separated by delimeter
 * 
 * @param tok token array
 * @param s string to tokenize
 * @param delim delimeter to separate tokens
 * @param maxTokens maximum number of tokens
 * 
 * @return number of tokens
 */
static int tokenize(char **tok, char *s, const char *delim, int maxTokens) {
    if (maxTokens <= 0 || !s || !tok) {
        return 0;
    }

    int tokenCount = 0;
    char *token = strtok(s, delim);
    while (token && tokenCount < maxTokens) {
        tok[tokenCount++] = token;
        token = strtok(NULL, delim);
    }

    return tokenCount;
}

/**
 * @brief Trim and remove comment from line if exists
 * 
 * @param s string to trim
 * @param len length of string
 * @return trimmed string
 */
static void trim_comment(char *s, int len) {
    for (int i = 0; i < len; i++) {
        if (s[i] == ';') s[i] = '\0';
    }
}

/**
 * @brief Convert symbols to bytes and write to output
 * 
 * @param output output file
 */
static void write(FILE *output) {
    int ret;

    for (int i = 0; i < symbolCount; i++) {
        switch(symbols[i].type) {
            case SYM_INSTRUCTION:
                ret = build_instruction(i);
                if (ret) {
                    fputc((ret>>8) & 0xFF, output);
                    fputc(ret & 0xFF, output);
                    printf("%04x\n", ret);
                    i += ins.pcount;
                } else {
                    error("Invalid instruction", symbols[i].ln);
                }
                break;
            case SYM_DB:
                if (symbols[i].value > UINT8_MAX) {
                    error("Value too big", symbols[i].ln);
                } else {
                    fputc(symbols[i].value, output);
                    printf("%02x\n", symbols[i].value);
                }
                break;
            case SYM_DW:
                if (symbols[i].value > UINT16_MAX) {
                    error("Value too big", symbols[i].ln);
                } else {
                    fputc((symbols[i].value>>8) & 0xFF, output);
                    fputc(symbols[i].value & 0xFF, output);
                    printf("%04x\n", symbols[i].value);
                }
                break;
            default:
                break;
        }
    }

    fclose(output);
}
