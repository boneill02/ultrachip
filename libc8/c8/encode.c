/**
 * @file c8/encode.c
 *
 * Base assembler code
 */

#include "encode.h"

#include "internal/symbol.h"
#include "defs.h"
#include "internal/exception.h"
#include "internal/util.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERBOSE_PRINT(s) if (args & ARG_VERBOSE) { printf("%s\n", s); }

static int line_count(const char *);
static int parse_line(char *, int, symbol_list_t *, label_list_t *);
static int parse_word(char *, char *, int, symbol_t *, label_list_t *);
static inline void put16(uint8_t *, uint16_t, int);
static int tokenize(char **, char *, const char *, int);
static int to_upper(char *);
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
 * @param out pointer to write bytecode to
 * @param args command line arguments
 *
 * @return length of resulting bytecode.
 */
int c8_encode(const char *s, uint8_t *out, int args) {
	NULLCHECK2(s, out);

	char *scpy;
	int len = strlen(s);
	int count = 0;
	int lineCount = line_count(s);
	int ret;
	char **lines;

	symbol_list_t symbols;
	symbols.s = (symbol_t *) calloc(sizeof(symbol_t), SYMBOL_CEILING);
	symbols.len = 0;
	symbols.ceil = SYMBOL_CEILING;

	label_list_t labels;
	labels.l = (label_t *) calloc(sizeof(label_t), LABEL_CEILING);
	labels.len = 0;
	labels.ceil = LABEL_CEILING;

	/* copy string and ensure newline at end */
	if (s[len - 1] == '\n') {
		scpy = (char *) malloc(len + 1);
		scpy[len] = '\0';
	} else {
		scpy = (char *) malloc(len + 2);
		scpy[len] = '\n';
		scpy[len + 1] = '\0';
	}

	for (int i = 0; i < len; i++) {
		scpy[i] = s[i];
	}

	lines = (char **) malloc(lineCount * sizeof(char *));
	if ((lineCount = tokenize(lines, scpy, "\n", lineCount)) < 0) {
		handle_exception(lineCount);
	}


	VERBOSE_PRINT("Populating identifiers in label map");
	if ((ret = populate_labels(lines, lineCount, &labels)) < 1) {
		handle_exception(ret);
	}

	VERBOSE_PRINT("Building symbol table");
	for (int i = 0; i < lineCount; i++) {
		ret = parse_line(lines[i], i+1, &symbols, &labels);
		if (ret < 1) {
			return ret;
		}
	}

	VERBOSE_PRINT("Resolving label addresses");
	if ((ret = resolve_labels(&symbols, &labels)) < 1) {
		handle_exception(ret);
	}

	VERBOSE_PRINT("Substituting label addresses in symbol table");
	if ((ret = substitute_labels(&symbols, &labels)) < 1) {
		handle_exception(ret);
	}

	VERBOSE_PRINT("Writing output");
	count = write(out, &symbols, args);

	free(scpy);
	free(symbols.s);
	free(labels.l);
	free(lines);
	return count;
}

/**
 * @brief Remove comment from string if exists
 *
 * @param s string to remove comment from
 * @return string without comment
 */
char *remove_comment(char *s) {
	NULLCHECK1(s);

	if (s[0] == ';') {
		s[0] = '\0';
		return s;
	}

	for (int i = 1; i < strlen(s); i++) {
		if (s[i] == ';' && isspace(s[i-1])) {
			s[i-1] = '\0';
		}
	}

	return s;
}

/**
 * @brief Get line count of s
 *
 * @param s string to count lines from
 *
 * @return line count
 */
static int line_count(const char *s) {
	NULLCHECK1(s);

	int ln = 1;
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
 *
 * @return 1 if success, exception code otherwise
 */
static int parse_line(char *s, int ln, symbol_list_t *symbols, label_list_t *labels) {
	NULLCHECK3(s, symbols, labels);

	if (strlen(s) == 0 || strlen(remove_comment(s)) == 0 ||
		strlen(trim(s)) == 0) {
		return 1;
	}

	symbol_t *sym = next_symbol(symbols);
	char *words[C8_ENCODE_MAX_WORDS];
	int wc = tokenize(words, s, " ", C8_ENCODE_MAX_WORDS);
	int ret = 0;

	for (int i = 0; i < wc; i++) {
		if (i == wc - 1) {
			ret = parse_word(words[i], NULL, ln, sym, labels);
			if (ret < 0) {
				return ret;
			}
			i += ret;
		} else {
			ret = parse_word(words[i], words[i+1], ln, sym, labels);
			if (ret < 0) {
				return ret;
			}
			i += ret;
		}
		sym = next_symbol(symbols);
	}

	return 1;
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
	NULLCHECK3(s, sym, labels);

	int value;
	sym->ln = ln;
	s = remove_comma(s);
	s = trim(s);
	to_upper(s);

	if (is_label_definition(s) == 1) {
		sym->type = SYM_LABEL_DEFINITION;
		for (int j = 0; j < labels->len; j++) {
			if (!strcmp(s, labels->l[j].identifier)) {
				sym->value = j;
			}
		}
		return 0;
	} else if ((value = is_instruction(s)) >= 0) {
		sym->type = SYM_INSTRUCTION;
		sym->value = value;
		return 0;
	} else if (is_db(s) && next) {
		sym->type = SYM_DB;
		sym->value = parse_int(next);
		return 1;
	} else if (is_dw(s) && next) {
		sym->type = SYM_DW;
		sym->value = parse_int(next);
		return 1;
	} else if ((value = is_register(s)) >= 0) {
		sym->type = SYM_V;
		sym->value = value;
		return 0;
	} else if ((value = is_reserved_identifier(s)) >= 0) {
		sym->type = (Symbol) value;
		return 0;
	} else if ((value = parse_int(s)) > -1) {
		if (value < 0x10) {
			sym->type = SYM_INT4;
		} else if (value < 0x100) {
			sym->type = SYM_INT8;
		} else if (value < 0x1000) {
			sym->type = SYM_INT12;
		} else {
			sym->type = SYM_INT;
		}
		sym->value = value;
		return 0;
	} else if ((value = is_label(s, labels)) >= 0) {
		sym->type = SYM_LABEL;
		sym->value = value;
		return 0;
	}

	sprintf(c8_exception, "(line %d): %s\n", ln, s);
	return INVALID_SYMBOL_EXCEPTION;
}

/**
 * @brief Write 16 bit int to f
 *
 * @param output where to write
 * @param n index to write to
 */
static inline void put16(uint8_t *output, uint16_t n, int idx) {
	NULLCHECK1(output);

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
	NULLCHECK3(tok, s, delim);

	if (maxTokens <= 0) {
		return INVALID_ARGUMENT_EXCEPTION_INTERNAL;
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
 * @brief Trim and remove comma from s if exists
 *
 * @param s string to remove comma from
 * @return string without comma
 */
static char *remove_comma(char *s) {
	NULLCHECK1(s);

	trim(s);
	if (s[strlen(s) - 1] == ',') {
		s[strlen(s) - 1] = '\0';
	}

	return s;
}

/**
 * @brief Convert all characters in null-terminated string s to uppercase
 *
 * @param s string to convert
 */
static int to_upper(char *s) {
	NULLCHECK1(s);

	while (*s) {
		*s = toupper(*s);
		s++;
	}
	return 1;
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
	NULLCHECK2(output, symbols);

	int ret;
	instruction_t ins;
	int byte = 0;

	for (int i = 0; i < symbols->len; i++) {
		if (byte >= C8_MEMSIZE - C8_PROG_START) {
			return TOO_MANY_SYMBOLS_EXCEPTION;
		}

		switch(symbols->s[i].type) {
			case SYM_INSTRUCTION:
				ret = build_instruction(&ins, symbols, i);
				if (ret > 0) {
					put16(output, ret, byte);
					i += ins.pcount;
					if (args & ARG_VERBOSE) {
						printf("%03x: %04x\n", byte + C8_PROG_START, ret);
					}
					byte += 2;
				} else {
					sprintf(c8_exception, "(line %d)\n", ins.line);
					return ret;
				}
				break;
			case SYM_DB:
				if (symbols->s[i].value > UINT8_MAX) {
					sprintf(c8_exception, "(line %d): DB value too big\n", symbols->s[i].ln);
					return INVALID_ARGUMENT_EXCEPTION;
				} else {
					output[byte] = symbols->s[i].value;

					if (args & ARG_VERBOSE) {
						printf("%03x: %04x\n", byte + C8_PROG_START, symbols->s[i].value);
					}

					byte++;
				}
				break;
			case SYM_DW:
				if (symbols->s[i].value > UINT16_MAX) {
					fprintf(stderr, "(line %d): DW value too big\n", symbols->s[i].ln);
					return INVALID_ARGUMENT_EXCEPTION;
				} else {
					put16(output, symbols->s[i].value, byte);
					if (args & ARG_VERBOSE) {
						printf("%03x: %04x\n", byte + C8_PROG_START, symbols->s[i].value);
					}
					byte += 2;
				}
				break;
			default:
				break;
		}
	}

	return byte;
}
