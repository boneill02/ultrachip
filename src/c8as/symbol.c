#include "symbol.h"

#ifdef TEST
#ifndef TEST_INCLUDED
#define TEST_INCLUDED
#include "util/util.c"
#include "parse.c"
#endif
#endif

#include "parse.h"
#include "util/defs.h"
#include "util/util.h"

#include <ctype.h>
#include <string.h>

static const char *identifierStrings[] = {
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
	S_HF,
	S_R,
	NULL,
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
	S_SCD,
	S_SCR,
	S_SCL,
	S_EXIT,
	S_LOW,
	S_HIGH,
	NULL,
};

instruction_format_t formats[] = {
	{ I_SCD,  0x00C0, 1, {SYM_INT},               {0x000F}},
	{ I_CLS,  0x00E0, 0, {SYM_NULL},              {0} },
	{ I_RET,  0x00EE, 0, {SYM_NULL},              {0} },
	{ I_SCR,  0x00FB, 0, {SYM_NULL},              {0}},
	{ I_SCL,  0x00FC, 0, {SYM_NULL},              {0}},
	{ I_EXIT, 0x00FD, 0, {SYM_NULL},              {0}},
	{ I_LOW,  0x00FE, 0, {SYM_NULL},              {0}},
	{ I_HIGH, 0x00FF, 0, {SYM_NULL},              {0}},
	{ I_JP,   0x1000, 1, {SYM_INT},               {0x0FFF} },
	{ I_CALL, 0x2000, 1, {SYM_INT},               {0x0FFF} },
	{ I_SE,   0x3000, 2, {SYM_V, SYM_INT},        {0x0F00, 0x00FF} },
	{ I_SNE,  0x4000, 2, {SYM_V, SYM_INT},        {0x0F00, 0x00FF} },
	{ I_SE,   0x5000, 2, {SYM_V, SYM_V},          {0x0F00, 0x00F0} },
	{ I_LD,   0x6000, 2, {SYM_V, SYM_INT},        {0x0F00, 0x00FF} },
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
	{ I_LD,   0xF030, 2, {SYM_HF, SYM_V},         {0x0000, 0x0F00} },
	{ I_LD,   0xF033, 2, {SYM_B, SYM_V},          {0x0000, 0x0F00} },
	{ I_LD,   0xF055, 2, {SYM_IP, SYM_V},         {0x0000, 0x0F00} },
	{ I_LD,   0xF065, 2, {SYM_V, SYM_IP},         {0x0F00, 0x0000} },
	{ I_LD,   0xF075, 2, {SYM_R, SYM_V},          {0x0000, 0x0F00} },
	{ I_LD,   0xF085, 2, {SYM_V, SYM_R},          {0x0F00, 0x0000} },
	{ I_NULL, 0,      0, {SYM_NULL},              {0} },
};

static uint16_t parse_instruction(instruction_t *);
static void reallocate_symbols(symbol_list_t *symbols);
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
	if (symbols == NULL || idx < 0) {
		return 0;
	}

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
			case SYM_HF:
			case SYM_R:
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
 * @brief Check if the given string is a comment
 * 
 * @param s the string to check
 * @return 1 if true, 0 if false
 */
int is_comment(const char *s) {
	if (strlen(s) == 0) {
		return 0;
	}
	return s[0] == ';';
}

/**
 * @brief Check if given string is a DB identifier
 * 
 * @return 1 if true, 0 if false
 */
int is_db(const char *s) {
	return !strcmp(s, S_DB);
}

/**
 * @brief Check if given string is a DW identifier
 * 
 * @return 1 if true, 0 if false
 */
int is_dw(const char *s) {
	return !strcmp(s, S_DW);
}

/**
 * @brief Check if the given string is an instruction
 * 
 * @param s the string to check
 * @return instruction enumerator if true, -1 if false
 */
int is_instruction(const char *s) {
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
 * @return 1 if true, 0 if false
 */
int is_label_definition(const char *s) {
	int len = strlen(s);
	if (len < 2) {
		return 0;
	}

	return s[len-1] == ':';
}

/**
 * @brief Check if given string is a label reference
 * 
 * @param s string to check
 * @return label index if true, -1 otherwise
 */
int is_label(const char *s, label_list_t *labels) {
	for (int i = 0; i < labels->len; i++) {
		if (!strcmp(s, labels->l[i].identifier)) {
			return i;
		}
	}

	return -1;
}

/**
 * @brief Check if the given string represents a V register
 * 
 * @param s string to check
 * @return V register number if true, -1 otherwise
 */
int is_register(const char *s) {
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
int is_reserved_identifier(const char *s) {
	for (int i = 0; identifierStrings[i]; i++) {
		if (!strcmp(s, identifierStrings[i])) {
			return i;
		}
	}

	return -1;
}

/**
 * @brief Get the next symbol
 * 
 * @return first empty symbol in symbol table
 */
symbol_t *next_symbol(symbol_list_t *symbols) {
	symbols->len++;
	if (symbols->len == symbols->ceil) {
		reallocate_symbols(symbols);
	}

	return &symbols->s[symbols->len];
}

/**
 * @brief Populate label list from lines
 * 
 * @param lines lines to search
 * @param lineCount number of lines to search
 * @param labels label list to populate
 * 
 * @return 1 if success, 0 if error caught
 */
int populate_labels(char **lines, int lineCount, label_list_t *labels) {
	for (int i = 0; i < lineCount; i++) {
		if (labels->len == labels->ceil) {
			return 0;
		}

		if (strlen(lines[i]) == 0) {
			continue;
		}
		lines[i] = trim(lines[i]);
		lines[i] = remove_comment(lines[i]);
		if (strlen(remove_comment(lines[i])) == 0) {
			continue;
		}

		if (is_label_definition(lines[i])) {
			int l = strlen(lines[i]) - 1;
			strncpy(labels->l[labels->len].identifier, lines[i], LABEL_IDENTIFIER_SIZE);
			labels->l[labels->len].identifier[l] = '\0';
			labels->len++;
		}
	}

	return 1;
}

/**
 * @brief Get byte indexes of label definitions from completed symbol table
 * 
 * @param symbols list of symbols
 * @param labels list of labels
 */
void resolve_labels(symbol_list_t *symbols, label_list_t *labels) {
	int byte = PROG_START;
	int labelIdx = 0;
	for (int i = 0; i < symbols->len; i++) {
		if (labelIdx == labels->len) {
			return;
		}

		switch (symbols->s[i].type) {
			case SYM_LABEL_DEFINITION:
				labels->l[labelIdx++].byte = byte;
				break;
			case SYM_DB:
				byte++;
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
 * @brief Substitute label symbols with their corresponding int value
 * 
 * @param symbols symbols to search
 * @param labels labels to search
 */
void substitute_labels(symbol_list_t *symbols, label_list_t *labels) {
	for (int i = 0; i < symbols->len; i++) {
		if (symbols->s[i].type == SYM_LABEL) {
			symbols->s[i].type = SYM_INT;
			symbols->s[i].value = labels->l[symbols->s[i].value].byte;
		}
	}
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

/**
 * @brief Expand symbol list
 * 
 * @param symbols symbol list
 */
static void reallocate_symbols(symbol_list_t *symbols) {
	int newCeiling = symbols->ceil + SYMBOL_CEILING;
	symbol_t *oldsym = symbols->s;
	symbols->s = (symbol_t *) malloc(sizeof(symbol_t) * newCeiling);
	memcpy(symbols->s, oldsym, symbols->ceil * sizeof(symbol_t));
	symbols->ceil = newCeiling;
	free(oldsym);
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
