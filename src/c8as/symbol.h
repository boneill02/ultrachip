#ifndef CHIP8_SYMBOL_H
#define CHIP8_SYMBOL_H

#include <stdint.h>
#include <stdlib.h>

#define INSTRUCTION_COUNT 64
#define LABEL_CEILING 64
#define LABEL_IDENTIFIER_SIZE 20
#define SYMBOL_CEILING 64

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
#define S_SCD "SCD"
#define S_SCR "SCR"
#define S_SCL "SCL"
#define S_EXIT "EXIT"
#define S_LOW "LOW"
#define S_HIGH "HIGH"
#define S_K "K"
#define S_F "F"
#define S_B "B"
#define S_DT "DT"
#define S_ST "ST"
#define S_I "I"
#define S_IP "[I]"
#define S_DB ".DB"
#define S_DW ".DW"
#define S_HF "HF"
#define S_R "R"

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
	I_SCD,
	I_SCR,
	I_SCL,
	I_EXIT,
	I_LOW,
	I_HIGH,
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
	SYM_NULL,
	SYM_DT,
	SYM_ST,
	SYM_I,
	SYM_IP,
	SYM_K,
	SYM_F,
	SYM_B,
	SYM_DB,
	SYM_DW,
	SYM_HF,
	SYM_R,
	SYM_LABEL,
	SYM_INT,
	SYM_INT4,
	SYM_INT8,
	SYM_INT12,
	SYM_STRING,
	SYM_V,
	SYM_INSTRUCTION,
	SYM_LABEL_DEFINITION,
} Symbol;

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

/**
 * @struct label_t
 * @brief Represents a label
 * 
 * Represents a label with an identifier and byte value
 * 
 * @var identifier string identifier
 * @var byte location of the label
 */
typedef struct {
	char identifier[LABEL_IDENTIFIER_SIZE];
	int byte;
} label_t;

/**
 * @struct label_list_t
 * @brief Represents a list of labels
 * 
 * @var l pointer to first label
 * @var len length of the list
 * @var ceil maximum length of the list
 */
typedef struct {
	label_t *l;
	int len;
	int ceil;
} label_list_t;

/**
 * @struct symbol_t
 * @brief Represents a symbol with a type, value, and line number
 */
typedef struct {
	Symbol type;
	uint16_t value;
	int ln;
} symbol_t;

/**
 * @struct symbol_list_t
 * @brief Represents a symbol with a type, value, and line number
 */
typedef struct {
	symbol_t *s;
	int len;
	int ceil;
} symbol_list_t;

extern const char *instructionStrings[];
extern const char *identifierStrings[];
extern instruction_format_t formats[];

int build_instruction(instruction_t *, symbol_list_t *, int);
int is_comment(const char *);
int is_db(const char *);
int is_dw(const char *);
int is_instruction(const char *);
int is_label_definition(const char *);
int is_label(const char *, label_list_t *);
int is_register(const char *);
int is_reserved_identifier(const char *);
symbol_t *next_symbol(symbol_list_t *);
int populate_labels(char **, int, label_list_t *);
int resolve_labels(symbol_list_t *, label_list_t *);
int substitute_labels(symbol_list_t *, label_list_t *);
#endif
