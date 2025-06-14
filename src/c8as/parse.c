#include "parse.h"

#ifdef TEST
#ifndef TEST_INCLUDED
#define TEST_INCLUDED
#include "util/util.c"
#include "symbol.c"
#endif
#endif

#include "symbol.h"

#include "util/defs.h"
#include "util/util.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int line_count(char *);
static void parse_line(char *, int, symbol_list_t *, label_list_t *);
static int parse_word(char *, char *, int, symbol_t *, label_list_t *);
static inline void put16(uint8_t *, uint16_t, int);
static int tokenize(char **, char *, const char *, int);
static char *to_upper(char *);
static char *remove_comma(char *);
static int write(uint8_t *, symbol_list_t *, int);

/**
 * @brief Parse the given string
 * 
 * This is the main assembler function.
 * 
 * This function generates bytecode from the given assembly code.
 * 
 * @param s string containing assembly code
 * @param f file to write to
 * @param a args
 * 
 * @return length of resulting bytecode.
 */
int parse(char *s, uint8_t *out, int args) {
	int bytes = 0;
	int lineCount = line_count(s);

	symbol_list_t symbols;
	symbols.s = malloc(sizeof(symbol_t) * SYMBOL_CEILING);
	symbols.len = 0;
	symbols.ceil = SYMBOL_CEILING;

	label_list_t labels;
	labels.l = malloc(sizeof(label_t) * LABEL_CEILING);
	labels.len = 0;
	labels.ceil = LABEL_CEILING;

	char **lines = malloc(lineCount * sizeof(char *));

	lineCount = tokenize(lines, s, "\n", lineCount); // lineCount updated to remove empty lines

	populate_labels(lines, lineCount, &labels);

	for (int i = 0; i < lineCount; i++) {
		parse_line(lines[i], i+1, &symbols, &labels);
	}

	resolve_labels(&symbols, &labels);
	substitute_labels(&symbols, &labels);

	bytes = write(out, &symbols, args);

	free(symbols.s);
	free(labels.l);
	free(lines);
	return bytes;
}

/**
 * @brief Trim and remove comment from line if exists
 * 
 * @param s string to trim
 * @return trimmed string
 */
char *remove_comment(char *s) {
	for (int i = 0; i < strlen(s); i++) {
		if (s[i] == ';') s[i] = '\0';
	}

	trim(s);
	return s;
}


static int line_count(char *s) {
	int ln = 0;
	while (*s) {
		if (*s == '\n') {
			ln++;
		}
		s++;
	}
	return ln;
}

/**
 * @brief Generate symbols for the given line
 * 
 * @param s line string
 * @param ln line number
 * @param symbols symbol list
 * @param labels label list
 */
static void parse_line(char *s, int ln, symbol_list_t *symbols, label_list_t *labels) {
	if (strlen(s) == 0 || strlen(remove_comment(s)) == 0) {
		return;
	}

	symbol_t *sym = &symbols->s[symbols->len];
	char *words[MAX_WORDS];
	int wc = tokenize(words, s, " ", MAX_WORDS);

	for (int i = 0; i < wc; i++) {
		if (i == wc-1) {
			i += parse_word(words[i], NULL, ln, sym, labels);
		} else {
			i += parse_word(words[i], words[i+1], ln, sym, labels);
		}
		sym = next_symbol(symbols);
	}
}

/**
 * @brief Generate symbol for the given word
 * 
 * @param s word string
 * @param next next word string
 * @param ln line number
 * @param sym symbol to populate
 * @param labels label list
 * 
 * @return number of words to skip
 */
static int parse_word(char *s, char *next, int ln, symbol_t *sym, label_list_t *labels) {
	int value;
	sym->ln = ln;
	s = remove_comma(s);

	if (is_label_definition(s)) {
		sym->type = SYM_LABEL_DEFINITION;
		for (int j = 0; j < labels->len; j++) {
			if (!strcmp(s, labels->l[j].identifier)) {
				sym->value = j;
			}
		}
	} else if ((value = is_instruction(to_upper(s))) != I_NULL) {
		sym->type = SYM_INSTRUCTION;
		sym->value = value;
	} else if (is_db(to_upper(s))) {
		sym->type = SYM_DB;
		sym->value = parse_int(next);
		return 1;
	} else if (is_dw(to_upper(s))) {
		sym->type = SYM_DW;
		sym->value = parse_int(next);
		return 1;
	} else if ((value = is_register(to_upper(s))) != -1) {
		sym->type = SYM_V;
		sym->value = value;
	} else if ((value = is_reserved_identifier(to_upper(s))) != -1) {
		sym->type = value;
	} else if ((value = parse_int(s)) != -1) {
		sym->type = SYM_INT;
		sym->value = value;
	} else if ((value = is_label(s, labels)) != -1) {
		sym->type = SYM_LABEL;
		sym->value = value;
	} else {
		fprintf(stderr, "Error (line %d): Unknown symbol \"%s\"\n", ln, s);
	}

	return 0;
}


/**
 * @brief Write 16 bit int to f
 * 
 * @param output where to write
 * @param n index to write to
 */
static inline void put16(uint8_t *output, uint16_t n, int idx) {
	output[idx] = (n >> 8) & 0xFF;
	output[idx+1] = n & 0xFF;
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
	token = trim(token);
	while (token && tokenCount < maxTokens) {
		tok[tokenCount++] = trim(token);
		token = strtok(NULL, delim);
	}

	return tokenCount;
}

/**
 * @brief Trim and remove comma from s if exists
 * 
 * @param s string to trim
 * @return trimmed string
 */
static char *remove_comma(char *s) {
	trim(s);
	if (s[strlen(s) - 1] == ',') {
		s[strlen(s) - 1] = '\0';
	}

	return s;
}

/**
 * @brief Convert all characters in s to uppercase
 * 
 * @param s string to convert
 */
static char *to_upper(char *s) {
	while (*s) {
		*s = toupper(*s);
		s++;
	}

	return s;
}

/**
 * @brief Convert symbols to bytes and write to output
 * 
 * @param output output array
 * @param symbols symbol list
 * @param args arguments given by user
 * 
 * @return length of bytecode
 */
static int write(uint8_t *output, symbol_list_t *symbols, int args) {
	int ret;
	instruction_t ins;
	int byte = 0;

	for (int i = 0; i < symbols->len; i++) {
		if (byte >= MEMSIZE - PROG_START) {
			return -1;
		}

		switch(symbols->s[i].type) {
			case SYM_INSTRUCTION:
				ret = build_instruction(&ins, symbols, i);
				if (ret) {
					put16(output, ret, byte);
					i += ins.pcount;
					if (args & ARG_VERBOSE) {
						printf("%03x: %04x\n", byte + PROG_START, ret);
					}
					byte += 2;
				} else {
					fprintf(stderr, "Error (line %d): Invalid instruction\n", symbols->s[i].ln);
				}
				break;
			case SYM_DB:
				if (symbols->s[i].value > UINT8_MAX) {
					fprintf(stderr, "Error (line %d): DB value too big\n", symbols->s[i].ln);
				} else {
					output[byte] = symbols->s[i].value;
					printf("%03x: %04x\n", byte + PROG_START, symbols->s[i].value);
					byte++;
				}
				break;
			case SYM_DW:
				if (symbols->s[i].value > UINT16_MAX) {
					fprintf(stderr, "Error (line %d): DW value too big\n", symbols->s[i].ln);
				} else {
					put16(output, symbols->s[i].value, byte);
					printf("%03x: %04x\n", byte + PROG_START, symbols->s[i].value);
					byte += 2;
				}
				break;
			default:
				break;
		}
	}

	return byte;
}