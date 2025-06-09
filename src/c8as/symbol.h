#ifndef CHIP8_SYMBOL_H
#define CHIP8_SYMBOL_H

#include <stdint.h>
#include <stdlib.h>

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
	SYM_LABEL,
	SYM_INT,
	SYM_STRING,
	SYM_V,
	SYM_INSTRUCTION,
	SYM_LABEL_DEFINITION,
} Symbol;

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

int is_comment(char *);
int is_db(char *);
int is_dw(char *);
int is_instruction(char *);
int is_label_definition(char *);
int is_label(char *, label_list_t *);
int is_register(char *);
int is_reserved_identifier(char *);
symbol_t *next_symbol(symbol_list_t *);
int populate_labels(char **, int, label_list_t *);
void resolve_labels(symbol_list_t *, label_list_t *);
void substitute_labels(symbol_list_t *, label_list_t *);

#endif