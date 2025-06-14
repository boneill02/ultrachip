#include "unity.h"
#include "symbol.h"
#include "parse.h"
#include "util/defs.h"
#include "util/util.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINE_COUNT 10
#define MAX_LINE_LEN 32
#define CLEAR_LINES memset(lines[0], 0, MAX_LINE_LEN * MAX_LINE_COUNT)
#define CLEAR_LABELS \
	memset(labels.l, 0, LABEL_CEILING * sizeof(label_t)); \
	labels.len = 0; \
	labels.ceil = LABEL_CEILING;

char buf[64];
instruction_t ins;
symbol_list_t symbols;
label_list_t labels;
const char *empty = "\0";
int fc = 0;
char **lines;

void setUp(void) {
	srand(time(NULL));

	symbols.s = calloc(SYMBOL_CEILING, sizeof(symbol_t));
	symbols.ceil = SYMBOL_CEILING;
	labels.l = calloc(LABEL_CEILING, sizeof(label_t));
	labels.ceil = LABEL_CEILING;

	for(fc = 0; formats[fc].cmd != I_NULL; fc++);

	lines = malloc(MAX_LINE_COUNT * sizeof(char *));
	char *line0 = calloc(MAX_LINE_LEN, MAX_LINE_COUNT);
	for (int i = 0; i < MAX_LINE_COUNT; i++) {
		lines[i] = line0;
		line0 += MAX_LINE_LEN;
	}

}

void tearDown(void) {
	free(symbols.s);
	free(labels.l);
	
	free(lines[0]);
	free(lines);
}

void generate_valid_instruction_symbols(int idx, int ln) {
	instruction_format_t *fmt = &formats[rand() % fc];

	symbols.s[idx].ln = ln;
	symbols.s[idx].type = SYM_INSTRUCTION;
	symbols.s[idx].value = fmt->cmd;

	idx++;
	symbols.len = idx + fmt->pcount;
	for (int i = 0; i < fmt->pcount; i++) {
		symbols.s[idx+i].ln = ln;
		symbols.s[idx+i].type = fmt->ptype[i];
		switch (fmt->ptype[i]) {
			case SYM_R:
			case SYM_V:
			case SYM_INT:
				symbols.s[idx+i].value = rand() % 16;
				break;
			default:
				symbols.s[idx+i].value = 0;
		}
	}
}

void generate_invalid_instruction_symbols(int idx, int ln) {
	instruction_format_t *fmt = &formats[rand() % fc];

	while (fmt->pcount == 0) {
		fmt = &formats[rand() % fc];
	}

	symbols.s[idx].ln = ln;
	symbols.s[idx].type = SYM_INSTRUCTION;
	symbols.s[idx].value = fmt->cmd;

	int count = fmt->pcount == 0 ? 1 : 0;

	idx++;
	symbols.len = idx + fmt->pcount;
	for (int i = 0; i < count; i++) {
		symbols.s[idx+i].ln = ln;
		switch (fmt->ptype[i]) {
			case SYM_V:
				symbols.s[idx+i].type = SYM_INT;
				symbols.s[idx+i].value = rand() % 0x100;
				break;
			default:
				symbols.s[idx+i].type = SYM_V;
				symbols.s[idx+i].value = rand() % 0x10;
		}
	}
}

void test_build_instruction_valid(void) {
	int idx = rand() % SYMBOL_CEILING - 5;
	generate_valid_instruction_symbols(idx, rand());

	TEST_ASSERT_NOT_EQUAL_INT(0, build_instruction(&ins, &symbols, idx));
}

void test_build_instruction_invalid(void) {
	int idx = rand() % SYMBOL_CEILING - 5;
	generate_invalid_instruction_symbols(idx, rand());

	TEST_ASSERT_EQUAL_INT(0, build_instruction(&ins, &symbols, idx));
}

void test_build_instruction_valid_null_symbol_table(void) {
	generate_invalid_instruction_symbols(0, 0);
	int idx = rand() % SYMBOL_CEILING - 5;

	TEST_ASSERT_EQUAL_INT(0, build_instruction(&ins, NULL, idx));
}

void test_build_instruction_valid_negative_idx(void) {
	generate_valid_instruction_symbols(0, 0);
	int idx = (rand() % SYMBOL_CEILING) * -1;
	symbol_list_t symbols[SYMBOL_CEILING];	

	TEST_ASSERT_EQUAL_INT(0, build_instruction(&ins, symbols, idx));
}

void test_is_comment_end_comment(void) {
	const char *s = "Hello ; This is a comment";

	TEST_ASSERT_EQUAL_INT(0, is_comment(s));
}

void test_is_comment_comment(void) {
	const char *s = "; This is a comment";

	TEST_ASSERT_EQUAL_INT(1, is_comment(s));
}

void test_is_comment_empty_string(void) {
	TEST_ASSERT_EQUAL_INT(0, is_comment(empty));
}

void test_is_comment_no_comment(void) {
	const char *s = "This is not a comment";

	TEST_ASSERT_EQUAL_INT(0, is_comment(s));
}

void test_is_db_db(void) {
	const char *s = "DB";

	TEST_ASSERT_EQUAL_INT(1, is_db(s));
}

void test_is_db_not_db(void) {
	const char *s = "DW";

	TEST_ASSERT_EQUAL_INT(0, is_db(s));
}

void test_is_db_contains_db(void) {
	const char *s = "Foo DB";
	TEST_ASSERT_EQUAL_INT(0, is_db(s));
}

void test_is_db_trailing_chars(void) {
	const char *s = "DB foo";
	TEST_ASSERT_EQUAL_INT(0, is_db(s));
}

void test_is_db_empty_string(void) {
	TEST_ASSERT_EQUAL_INT(0, is_db(empty));
}

void test_is_dw_dw(void) {
	const char *s = "DW";
	TEST_ASSERT_EQUAL_INT(1, is_dw(s));
}

void test_is_dw_not_dw(void) {
	const char *s = "DB";
	TEST_ASSERT_EQUAL_INT(0, is_dw(s));
}

void test_is_dw_contains_dw(void) {
	const char *s = "Foo DW";
	TEST_ASSERT_EQUAL_INT(0, is_db(s));
}

void test_is_dw_trailing_chars(void) {
	const char *s = "DW foo";
	TEST_ASSERT_EQUAL_INT(0, is_dw(s));
}

void test_is_dw_empty_string(void) {
	TEST_ASSERT_EQUAL_INT(0, is_dw(empty));
}

void test_is_instruction_instruction(void) {
	int ic = 0;
	for (ic = 0; instructionStrings[ic] != NULL; ic++);

	int ins = rand() % ic;

	char s[16];
	strcpy(s, instructionStrings[ins]);

	TEST_ASSERT_EQUAL_INT(ins, is_instruction(s));
}

void test_is_instruction_not_instruction(void) {
	const char *s = "Not an instruction";

	TEST_ASSERT_EQUAL_INT(-1, is_instruction(s));
}

void test_is_instruction_empty_string(void) {
	TEST_ASSERT_EQUAL_INT(-1, is_instruction(empty));
}

void test_is_label_definition_label_definition(void) {
	const char *s = "L:";

	TEST_ASSERT_EQUAL_INT(1, is_label_definition(s));
}

void test_is_label_definition_not_label_definition(void) {
	const char *s = "L";

	TEST_ASSERT_EQUAL_INT(0, is_label_definition(s));
}

void test_is_label_definition_empty_string(void) {
	TEST_ASSERT_EQUAL_INT(0, is_label_definition(empty));
}

void test_is_label_label(void) {
	const char *s = "L";

	labels.len = 3;
	labels.l[0].byte = rand();
	strcpy(labels.l[0].identifier, "LABEL");
	labels.l[1].byte = rand();
	strcpy(labels.l[1].identifier, "ANOTHERLABEL");
	labels.l[2].byte = rand();
	strcpy(labels.l[2].identifier, "L");

	TEST_ASSERT_EQUAL_INT(2, is_label(s, &labels));
}

void test_is_label_not_label(void) {
	const char *s = "L";

	labels.len = 3;
	labels.l[0].byte = rand();
	strcpy(labels.l[0].identifier, "LABEL");
	labels.l[1].byte = rand();
	strcpy(labels.l[1].identifier, "ANOTHERLABEL");
	labels.l[2].byte = rand();
	strcpy(labels.l[2].identifier, "L");

	TEST_ASSERT_EQUAL_INT(-1, is_label("LABEL3", &labels));
}

void test_is_label_empty_string(void) {
	TEST_ASSERT_EQUAL_INT(-1, is_label(empty, &labels));
}

void test_is_label_null_labellist(void) {
	TEST_ASSERT_EQUAL_INT(NULL_ARGUMENT_EXCEPTION, is_label(empty, NULL));
}

void test_is_register_register_uppercase(void) {
	const char *s = "V1";
	TEST_ASSERT_EQUAL_INT(1, is_register(s));
}

void test_is_register_register_lowercase(void) {
	const char *s = "vf";
	TEST_ASSERT_EQUAL_INT(0xF, is_register(s));
}

void test_is_register_not_register(void) {
	const char *s = "x4";
	TEST_ASSERT_EQUAL_INT(-1, is_register(s));
}

void test_is_reserved_identifier_reserved_identifier(void) {
	int ic = 0;
	for (ic = 0; identifierStrings[ic] != NULL; ic++);

	int ident = rand() % ic;

	char s[16];
	strcpy(s, identifierStrings[ident]);

	TEST_ASSERT_EQUAL_INT(ident, is_reserved_identifier(s));
}

void test_is_reserved_identifier_not_reserved_identifier(void) {
	const char *s = "Not reserved";

	TEST_ASSERT_EQUAL_INT(-1, is_reserved_identifier(s));
}

void test_next_symbol_empty_symbollist(void) {
	memset(symbols.s, 0, SYMBOL_CEILING);
	symbols.len = 0;
	symbols.ceil = SYMBOL_CEILING;

	TEST_ASSERT_EQUAL_PTR(&symbols.s[0], next_symbol(&symbols));
}

void test_next_symbol_normal_symbollist(void) {
	memset(symbols.s, 0, SYMBOL_CEILING);

	symbols.len = 2;
	symbols.ceil = SYMBOL_CEILING;
	symbols.s[0].type = SYM_INSTRUCTION;
	symbols.s[1].type = SYM_IP;

	TEST_ASSERT_EQUAL_PTR(&symbols.s[3], next_symbol(&symbols));
}

void test_next_symbol_full_symbollist(void) {
	memset(symbols.s, SYM_INSTRUCTION, SYMBOL_CEILING);

	symbols.len = SYMBOL_CEILING - 1;
	symbols.ceil = SYMBOL_CEILING;

	symbol_t *symbol = next_symbol(&symbols);

	TEST_ASSERT_EQUAL_INT(SYMBOL_CEILING, symbols.len);
	TEST_ASSERT_EQUAL_INT(SYMBOL_CEILING * 2, symbols.ceil);
	TEST_ASSERT_EQUAL_PTR(&symbols.s[SYMBOL_CEILING], symbol);
}

void test_next_symbol_null_symbollist(void) {
	TEST_ASSERT_EQUAL_PTR(NULL, next_symbol(NULL));
}

void test_populate_labels_empty_lines(void) {
	CLEAR_LINES;
	CLEAR_LABELS;

	int r = populate_labels(lines, MAX_LINE_COUNT, &labels);

	TEST_ASSERT_EQUAL_INT(1, r);
	TEST_ASSERT_EQUAL_INT(0, labels.len);
	TEST_ASSERT_EQUAL_STRING("\0", labels.l[0].identifier);
}

void test_populate_labels_null_lines(void) {
	CLEAR_LABELS;
	int r = populate_labels(NULL, 5, &labels);

	TEST_ASSERT_EQUAL_INT(NULL_ARGUMENT_EXCEPTION, r);
	TEST_ASSERT_EQUAL_INT(0, labels.len);
	TEST_ASSERT_EQUAL_STRING("\0", labels.l[0].identifier);
}

void test_populate_labels_no_labels(void) {
	CLEAR_LABELS;
	CLEAR_LINES;

	sprintf(lines[0], "%s", "ADD V4, V5");
	sprintf(lines[1], "%s", "\0");
	sprintf(lines[2], "%s", "CLS");
	sprintf(lines[3], "%s", "RET");
	sprintf(lines[4], "%s", "SE V1, $55");

	int r = populate_labels(lines, 5, &labels);
	TEST_ASSERT_EQUAL_INT(1, r);
	TEST_ASSERT_EQUAL_INT(0, labels.len);
	TEST_ASSERT_EQUAL_STRING("\0", labels.l[0].identifier);
}

void test_populate_labels_null_labellist(void) {
	CLEAR_LINES;

	sprintf(lines[0], "%s", "ADD V4, V5");
	sprintf(lines[1], "%s", "\0");
	sprintf(lines[2], "%s", "CLS");
	sprintf(lines[3], "%s", "RET");
	sprintf(lines[4], "%s", "SE V1, $55");

	int r = populate_labels(lines, 5, NULL);

	TEST_ASSERT_EQUAL_INT(NULL_ARGUMENT_EXCEPTION, r);
}

void test_populate_labels_existing_labels(void) {
	CLEAR_LABELS;
	CLEAR_LINES;

	sprintf(lines[0], "%s", "ADD V4, V5");
	sprintf(lines[1], "%s", "label:");
	sprintf(lines[2], "%s", "RET");
	sprintf(lines[3], "%s", "otherlabel:");
	sprintf(lines[4], "%s", "SE V1, $55");

	int r = populate_labels(lines, MAX_LINE_COUNT, &labels);

	TEST_ASSERT_EQUAL_INT(1, r);
	TEST_ASSERT_EQUAL_INT(2, labels.len);
	TEST_ASSERT_EQUAL_STRING("label", labels.l[0].identifier);
	TEST_ASSERT_EQUAL_STRING("otherlabel", labels.l[1].identifier);
}

void test_populate_labels_duplicate_labels(void) {
	CLEAR_LABELS;
	CLEAR_LINES;

	sprintf(lines[0], "%s", "ADD V4, V5");
	sprintf(lines[1], "%s", "label:");
	sprintf(lines[2], "%s", "RET");
	sprintf(lines[3], "%s", "label:");
	sprintf(lines[4], "%s", "SE V1, $55");

	int r = populate_labels(lines, MAX_LINE_COUNT, &labels);

	TEST_ASSERT_EQUAL_INT(DUPLICATE_LABEL_EXCEPTION, r);
}

void test_resolve_labels_one_label(void) { }

void test_resolve_labels_multiple_labels(void) { }

void test_resolve_labels_empty_symbollist(void) { }
void test_resolve_labels_null_symbollist(void) { }
void test_resolve_labels_empty_labellist(void) { }
void test_resolve_labels_null_labellist(void) { }

void test_substitute_labels_normal(void) { }
void test_substitute_labels_empty_symbollist(void) { }
void test_substitute_labels_null_symbollist(void) { }
void test_substitute_labels_empty_labellist(void) { }
void test_substitute_labels_null_labellist(void) { }

int main(void) {
    UNITY_BEGIN();

	RUN_TEST(test_build_instruction_valid);
	RUN_TEST(test_build_instruction_invalid);
	RUN_TEST(test_build_instruction_valid_negative_idx);
	RUN_TEST(test_build_instruction_valid_null_symbol_table);

	RUN_TEST(test_is_comment_comment);
	RUN_TEST(test_is_comment_end_comment);
	RUN_TEST(test_is_comment_no_comment);
	RUN_TEST(test_is_comment_empty_string);

	RUN_TEST(test_is_db_db);
	RUN_TEST(test_is_db_not_db);
	RUN_TEST(test_is_db_contains_db);
	RUN_TEST(test_is_db_trailing_chars);
	RUN_TEST(test_is_db_empty_string);

	RUN_TEST(test_is_dw_dw);
	RUN_TEST(test_is_dw_not_dw);
	RUN_TEST(test_is_dw_contains_dw);
	RUN_TEST(test_is_dw_trailing_chars);
	RUN_TEST(test_is_dw_empty_string);

	RUN_TEST(test_is_instruction_instruction);
	RUN_TEST(test_is_instruction_not_instruction);
	RUN_TEST(test_is_instruction_empty_string);

	RUN_TEST(test_is_label_definition_label_definition);
	RUN_TEST(test_is_label_definition_not_label_definition);
	RUN_TEST(test_is_label_definition_empty_string);

	RUN_TEST(test_is_label_label);
	RUN_TEST(test_is_label_not_label);
	RUN_TEST(test_is_label_empty_string);
	RUN_TEST(test_is_label_null_labellist);

	RUN_TEST(test_is_register_register_uppercase);
	RUN_TEST(test_is_register_register_lowercase);
	RUN_TEST(test_is_register_not_register);

	RUN_TEST(test_is_reserved_identifier_reserved_identifier);
	RUN_TEST(test_is_reserved_identifier_not_reserved_identifier);

	RUN_TEST(test_next_symbol_empty_symbollist);
	RUN_TEST(test_next_symbol_full_symbollist);
	RUN_TEST(test_next_symbol_normal_symbollist);
	RUN_TEST(test_next_symbol_null_symbollist);

	RUN_TEST(test_populate_labels_duplicate_labels);
	RUN_TEST(test_populate_labels_empty_lines);
	RUN_TEST(test_populate_labels_existing_labels);
	RUN_TEST(test_populate_labels_no_labels);
	RUN_TEST(test_populate_labels_null_labellist);
	RUN_TEST(test_populate_labels_null_lines);

    return UNITY_END();
}