#include "parse.h"

#include "instruction.h"
#include "symbol.h"
#include "util/util.h"
#include "util/defs.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int line_count(char *);
static void parse_line(char *, int, symbol_list_t *, label_list_t *);
static int parse_word(char *, char *, int, symbol_t *, label_list_t *);
static inline void put16(FILE *, uint16_t);
static int tokenize(char **, char *, const char *, int);
static char *trim_comma(char *);
static void write(FILE *, symbol_list_t *);

/**
 * @brief Parse the given string
 *
 * This is the main assembler function.
 *
 * This function generates bytecode from the given assembly code in s and writes
 * the output to f.
 *
 * @param s string containing assembly code
 * @param f file to write to
 */
void parse(char *s, FILE *f)
{
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

	s = trim(s);
	lineCount = tokenize(lines, s, "\n", lineCount); // lineCount updated to remove empty lines

	populate_labels(lines, lineCount, &labels);

	for (int i = 0; i < lineCount; i++)
	{
		parse_line(lines[i], i + 1, &symbols, &labels);
	}

	resolve_labels(&symbols, &labels);
	substitute_labels(&symbols, &labels);

	write(f, &symbols);

	free(symbols.s);
	free(labels.l);
	free(lines);
}

static int line_count(char *s)
{
	int ln = 0;
	while (*s)
	{
		if (*s == '\n')
		{
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
static void parse_line(char *s, int ln, symbol_list_t *symbols, label_list_t *labels)
{
	if (strlen(s) == 0 || strlen(trim_comment(s)) == 0)
	{
		return;
	}

	symbol_t *sym = &symbols->s[symbols->len];
	char *words[MAX_WORDS];
	int wc = tokenize(words, s, " ", MAX_WORDS);

	for (int i = 0; i < wc; i++)
	{
		if (i == wc - 1)
		{
			i += parse_word(words[i], NULL, ln, sym, labels);
		}
		else
		{
			i += parse_word(words[i], words[i + 1], ln, sym, labels);
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
static int parse_word(char *s, char *next, int ln, symbol_t *sym, label_list_t *labels)
{
	int value;
	sym->ln = ln;
	trim_comma(s);

	if (is_label_definition(s))
	{
		sym->type = SYM_LABEL_DEFINITION;
		for (int j = 0; j < labels->len; j++)
		{
			if (!strcmp(s, labels->l[j].identifier))
			{
				sym->value = j;
			}
		}
	}
	else if ((value = is_instruction(s)) != I_NULL)
	{
		sym->type = SYM_INSTRUCTION;
		sym->value = value;
	}
	else if (is_db(s))
	{
		sym->type = SYM_DB;
		sym->value = parse_int(next);
		return 1;
	}
	else if (is_dw(s))
	{
		sym->type = SYM_DW;
		sym->value = parse_int(next);
		return 1;
	}
	else if ((value = is_register(s)) != -1)
	{
		sym->type = SYM_V;
		sym->value = value;
	}
	else if ((value = is_reserved_identifier(s)) != -1)
	{
		sym->type = value;
	}
	else if ((value = parse_int(s)) != -1)
	{
		sym->type = SYM_INT;
		sym->value = value;
	}
	else if ((value = is_label(s, labels)) != -1)
	{
		sym->type = SYM_LABEL;
		sym->value = value;
	}
	else
	{
		fprintf(stderr, "Error (line %d): Unknown symbol \"%s\"\n", ln, s);
	}

	return 0;
}

/**
 * @brief Write 16 bit int to f
 *
 * @param f file to write to
 * @param n int to write
 */
static inline void put16(FILE *f, uint16_t n)
{
	fputc((n >> 8) & 0xFF, f);
	fputc(n & 0xFF, f);
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
static int tokenize(char **tok, char *s, const char *delim, int maxTokens)
{
	if (maxTokens <= 0 || !s || !tok)
	{
		return 0;
	}

	int tokenCount = 0;
	char *token = strtok(s, delim);
	while (token && tokenCount < maxTokens)
	{
		tok[tokenCount++] = token;
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
static char *trim_comma(char *s)
{
	trim(s);
	if (s[strlen(s) - 1] == ',')
	{
		s[strlen(s) - 1] = '\0';
	}

	return s;
}

/**
 * @brief Trim and remove comment from line if exists
 *
 * @param s string to trim
 * @return trimmed string
 */
char *trim_comment(char *s)
{
	trim(s);
	for (int i = 0; i < strlen(s); i++)
	{
		if (s[i] == ';')
			s[i] = '\0';
	}

	return s;
}

/**
 * @brief Convert symbols to bytes and write to output
 *
 * @param output output file
 * @param symbols symbol list
 */
static void write(FILE *output, symbol_list_t *symbols)
{
	int ret;
	instruction_t ins;

	for (int i = 0; i < symbols->len; i++)
	{
		switch (symbols->s[i].type)
		{
		case SYM_INSTRUCTION:
			ret = build_instruction(&ins, symbols, i);
			if (ret)
			{
				put16(output, ret);
				i += ins.pcount;
			}
			else
			{
				fprintf(stderr, "Error (line %d): Invalid instruction\n", symbols->s[i].ln);
			}
			break;
		case SYM_DB:
			printf("DB\n");
			if (symbols->s[i].value > UINT8_MAX)
			{
				fprintf(stderr, "Error (line %d): DB value too big\n", symbols->s[i].ln);
			}
			else
			{
				fputc(symbols->s[i].value, output);
			}
			break;
		case SYM_DW:
			if (symbols->s[i].value > UINT16_MAX)
			{
				fprintf(stderr, "Error (line %d): DW value too big\n", symbols->s[i].ln);
			}
			else
			{
				put16(output, symbols->s[i].value);
			}
			break;
		default:
			break;
		}
	}

	fclose(output);
}