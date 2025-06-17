#include "unity.h"

#include "util/util.c"
#include "parse.c"
#include "symbol.c"
#include "parse.h"
#include "util/defs.h"
#include "util/util.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINE_COUNT 10
#define MAX_LINE_LEN 32
#define CLEAR_LINES memset(lines[0],0,MAX_LINE_LEN*MAX_LINE_COUNT)
#define CLEAR_LABELS \
	memset(labels.l,0,LABEL_CEILING*sizeof(label_t)); \
	labels.len=0; \
	labels.ceil=LABEL_CEILING;
#define CLEAR_SYMBOLS \
	memset(labels.l, 0, SYMBOL_CEILING*sizeof(symbol_t)); \
	labels.len=0; \
	labels.ceil = LABEL_CEILING;
#define CLEAR_BUF for (int i=0;i<BUFSIZ;i++) {buf[i]='\0';}
#define RESET CLEAR_LINES; CLEAR_SYMBOLS; CLEAR_LABELS; CLEAR_BUF;

char buf[BUFSIZ];
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

void test_build_instruction_WhereInstructionIsValid(void) {
	RESET;

	int idx = rand() % SYMBOL_CEILING - 5;
	generate_valid_instruction_symbols(idx, rand());
	TEST_ASSERT_NOT_EQUAL_INT(0, build_instruction(&ins, &symbols, idx));
}

void test_build_instruction_WhereInstructionIsInvalid(void) {
	RESET;

	int idx = 10;
	generate_invalid_instruction_symbols(idx, rand());
	TEST_ASSERT_EQUAL_INT(INVALID_INSTRUCTION_EXCEPTION, build_instruction(&ins, &symbols, idx));
}

void test_build_instruction_WhereInstructioIsValid_WhereSymbolTableIsNull(void) {
	RESET;

	generate_invalid_instruction_symbols(0, 0);
	int idx = rand() % SYMBOL_CEILING - 5;
	TEST_ASSERT_EQUAL_INT(NULL_ARGUMENT_EXCEPTION, build_instruction(&ins, NULL, idx));
}

void test_build_instruction_WhereInstructionIsValid_WhereIdxIsNegative(void) {
	RESET;

	generate_valid_instruction_symbols(0, 0);
	int idx = (rand() % SYMBOL_CEILING) * -1;
	TEST_ASSERT_EQUAL_INT(INVALID_ARGUMENT_EXCEPTION_INTERNAL, build_instruction(&ins, &symbols, idx));
}

void test_is_comment_WhereCommentIsAtEndOfString(void) {
	RESET;

	const char *s = "Hello ; This is a comment";
	TEST_ASSERT_EQUAL_INT(0, is_comment(s));
}

void test_is_comment_WhereCommentIsEntireString(void) {
	RESET;

	const char *s = "; This is a comment";
	TEST_ASSERT_EQUAL_INT(1, is_comment(s));
}

void test_is_comment_WhereStringIsEmpty(void) {
	RESET;

	TEST_ASSERT_EQUAL_INT(0, is_comment(empty));
}

void test_is_comment_WhereNoCommentIsInString(void) {
	RESET;

	const char *s = "This is not a comment";
	TEST_ASSERT_EQUAL_INT(0, is_comment(s));
}

void test_is_db_WhereStringIsDB(void) {
	RESET;

	TEST_ASSERT_EQUAL_INT(1, is_db(S_DB));
}

void test_is_db_WhereStringIsNotDB(void) {
	RESET;

	TEST_ASSERT_EQUAL_INT(0, is_db(S_DW));
}

void test_is_db_WhereStringContainsDB(void) {
	RESET;

	const char *s = "Foo .DB";
	TEST_ASSERT_EQUAL_INT(0, is_db(s));
}

void test_is_db_WithTrailingChars(void) {
	RESET;

	const char *s = ".DB foo";
	TEST_ASSERT_EQUAL_INT(0, is_db(s));
}

void test_is_db_WhereStringIsEmpty(void) {
	RESET;

	TEST_ASSERT_EQUAL_INT(0, is_db(empty));
}

void test_is_db_WhereStringIsNull(void) {
	RESET;

	TEST_ASSERT_EQUAL_INT(NULL_ARGUMENT_EXCEPTION, is_db(NULL));
}

void test_is_dw_WhereStringIsDW(void) {
	RESET;
	TEST_ASSERT_EQUAL_INT(1, is_dw(S_DW));
}

void test_is_dw_WhereStringIsNotDW(void) {
	RESET;
	TEST_ASSERT_EQUAL_INT(0, is_dw(S_DB));
}

void test_is_dw_WhereStringContainsDW(void) {
	RESET;
	const char *s = "Foo .DW";
	TEST_ASSERT_EQUAL_INT(0, is_db(s));
}

void test_is_dw_WithTrailingChars(void) {
	RESET;
	const char *s = ".DW foo";
	TEST_ASSERT_EQUAL_INT(0, is_dw(s));
}

void test_is_dw_WhereStringIsEmpty(void) {
	RESET;
	TEST_ASSERT_EQUAL_INT(0, is_dw(empty));
}

void test_is_dw_WhereStringIsNull(void) {
	RESET;
	TEST_ASSERT_EQUAL_INT(NULL_ARGUMENT_EXCEPTION, is_dw(NULL));
}

void test_is_instruction_WhereStringIsInstruction(void) {
	RESET;
	int ic = 0;
	for (ic = 0; instructionStrings[ic] != NULL; ic++);

	int ins = rand() % ic;

	char s[16];
	strcpy(s, instructionStrings[ins]);

	TEST_ASSERT_EQUAL_INT(ins, is_instruction(s));
}

void test_is_instruction_WhereStringIsNotInstruction(void) {
	RESET;
	const char *s = "Not an instruction";

	TEST_ASSERT_EQUAL_INT(-1, is_instruction(s));
}

void test_is_instruction_WhereStringIsEmpty(void) {
	RESET;
	TEST_ASSERT_EQUAL_INT(-1, is_instruction(empty));
}

void test_is_instruction_WhereStringIsNull(void) {
	RESET;
	TEST_ASSERT_EQUAL_INT(NULL_ARGUMENT_EXCEPTION, is_instruction(NULL));
}

void test_is_label_definition_WhereStringIsLabelDefinition(void) {
	RESET;
	const char *s = "L:";

	TEST_ASSERT_EQUAL_INT(1, is_label_definition(s));
}

void test_is_label_definition_WhereStringIsNotLabelDefinition(void) {
	RESET;
	const char *s = "L";

	TEST_ASSERT_EQUAL_INT(0, is_label_definition(s));
}

void test_is_label_definition_WhereStringIsEmpty(void) {
	RESET;
	TEST_ASSERT_EQUAL_INT(0, is_label_definition(empty));
}

void test_is_label_definition_WhereStringIsNull(void) {
	RESET;
	TEST_ASSERT_EQUAL_INT(NULL_ARGUMENT_EXCEPTION, is_label_definition(NULL));
}

void test_is_label_WhereStringIsLabel(void) {
	RESET;
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

void test_is_label_WhereStringIsNotLabel(void) {
	RESET;
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

void test_is_label_WhereStringIsEmpty(void) {
	RESET;
	TEST_ASSERT_EQUAL_INT(-1, is_label(empty, &labels));
}

void test_is_label_WhereStringIsNull(void) {
	RESET;
	RESET;

	labels.len = 3;
	labels.l[0].byte = rand();
	strcpy(labels.l[0].identifier, "LABEL");
	labels.l[1].byte = rand();
	strcpy(labels.l[1].identifier, "ANOTHERLABEL");
	labels.l[2].byte = rand();
	strcpy(labels.l[2].identifier, "L");

	TEST_ASSERT_EQUAL_INT(NULL_ARGUMENT_EXCEPTION, is_label(NULL, &labels));
}

void test_is_label_WhereLabelListIsNull(void) {
	RESET;
	TEST_ASSERT_EQUAL_INT(NULL_ARGUMENT_EXCEPTION, is_label(empty, NULL));
}

void test_is_register_WhereStringIsRegister_WhereRegisterIsUppercase(void) {
	RESET;
	const char *s = "V1";
	TEST_ASSERT_EQUAL_INT(1, is_register(s));
}

void test_is_register_WhereStringIsRegister_RegisterIsLowercase(void) {
	RESET;
	const char *s = "vf";
	TEST_ASSERT_EQUAL_INT(0xF, is_register(s));
}

void test_is_register_WhereStringIsNotRegister(void) {
	RESET;
	const char *s = "x4";
	TEST_ASSERT_EQUAL_INT(-1, is_register(s));
}

void test_is_register_WhereStringIsEmpty(void) {
	RESET;
	TEST_ASSERT_EQUAL_INT(-1, is_register(empty));
}

void test_is_register_WhereStringIsNull(void) {
	RESET;
	TEST_ASSERT_EQUAL_INT(NULL_ARGUMENT_EXCEPTION, is_register(NULL));
}

void test_is_reserved_identifier_WhereStringIsReservedIdentifier(void) {
	RESET;
	int ic = 0;
	for (ic = 0; identifierStrings[ic] != NULL; ic++);

	int ident = rand() % ic;

	char s[16];
	strcpy(s, identifierStrings[ident]);

	TEST_ASSERT_EQUAL_INT(ident, is_reserved_identifier(s));
}

void test_is_reserved_identifier_WhereStringIsNotReservedIdentifier(void) {
	RESET;
	const char *s = "Not reserved";

	TEST_ASSERT_EQUAL_INT(-1, is_reserved_identifier(s));
}

void test_is_reserved_identifier_WhereStringIsEmpty(void) {
	RESET;
	TEST_ASSERT_EQUAL_INT(-1, is_reserved_identifier(empty));
}

void test_is_reserved_identifier_WhereStringIsNull(void) {
	RESET;
	TEST_ASSERT_EQUAL_INT(-1, is_reserved_identifier(NULL));
}

void test_next_symbol_WhereSymbolListIsEmpty(void) {
	RESET;
	memset(symbols.s, 0, SYMBOL_CEILING);
	symbols.len = 0;
	symbols.ceil = SYMBOL_CEILING;

	TEST_ASSERT_EQUAL_PTR(&symbols.s[0], next_symbol(&symbols));
}

void test_next_symbol_WhereSymbolListIsNotEmptyOrFull(void) {
	RESET;
	memset(symbols.s, 0, SYMBOL_CEILING);

	symbols.len = 2;
	symbols.ceil = SYMBOL_CEILING;
	symbols.s[0].type = SYM_INSTRUCTION;
	symbols.s[1].type = SYM_IP;

	TEST_ASSERT_EQUAL_PTR(&symbols.s[2], next_symbol(&symbols));
}

void test_next_symbol_WhereSymbolListIsFull(void) {
	RESET;
	memset(symbols.s, SYM_INSTRUCTION, SYMBOL_CEILING);

	symbols.len = SYMBOL_CEILING;
	symbols.ceil = SYMBOL_CEILING;

	symbol_t *symbol = next_symbol(&symbols);

	TEST_ASSERT_EQUAL_INT(SYMBOL_CEILING + 1, symbols.len);
	TEST_ASSERT_EQUAL_INT(SYMBOL_CEILING * 2, symbols.ceil);
	TEST_ASSERT_EQUAL_PTR(&symbols.s[SYMBOL_CEILING], symbol);
}

void test_next_symbol_WhereSymbolListIsNull(void) {
	RESET;
	TEST_ASSERT_EQUAL_PTR(NULL, next_symbol(NULL));
}

void test_populate_labels_WhereLinesIsEmpty(void) {
	RESET;
	CLEAR_LINES;
	CLEAR_LABELS;

	int r = populate_labels(lines, MAX_LINE_COUNT, &labels);

	TEST_ASSERT_EQUAL_INT(1, r);
	TEST_ASSERT_EQUAL_INT(0, labels.len);
	TEST_ASSERT_EQUAL_STRING("\0", labels.l[0].identifier);
}

void test_populate_labels_WhereLinesIsNull(void) {
	RESET;
	CLEAR_LABELS;
	int r = populate_labels(NULL, 5, &labels);

	TEST_ASSERT_EQUAL_INT(NULL_ARGUMENT_EXCEPTION, r);
	TEST_ASSERT_EQUAL_INT(0, labels.len);
	TEST_ASSERT_EQUAL_STRING("\0", labels.l[0].identifier);
}

void test_populate_labels_WhereLabelListIsEmpty(void) {
	RESET;
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

void test_populate_labels_WhereLabelListIsNull(void) {
	RESET;
	CLEAR_LINES;

	sprintf(lines[0], "%s", "ADD V4, V5");
	sprintf(lines[1], "%s", "\0");
	sprintf(lines[2], "%s", "CLS");
	sprintf(lines[3], "%s", "RET");
	sprintf(lines[4], "%s", "SE V1, $55");

	int r = populate_labels(lines, 5, NULL);

	TEST_ASSERT_EQUAL_INT(NULL_ARGUMENT_EXCEPTION, r);
}

void test_populate_labels_WhereLinesHasMultipleLabelDefinitions(void) {
	RESET;
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

void test_populate_labels_WhereLinesHasDuplicateLabelDefinitions(void) {
	RESET;
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

void test_resolve_labels_WhereLabelListHasOneLabel_WhereSymbolListHasLabelDefinition(void) {
	RESET;

	symbols.len = 3;
	symbols.s[0].type = SYM_INSTRUCTION;
	symbols.s[1].type = SYM_LABEL;
	symbols.s[1].value = 0;
	symbols.s[2].type = SYM_LABEL_DEFINITION;
	symbols.s[2].value = 0;
	symbols.s[3].type = SYM_INSTRUCTION;

	labels.len = 1;
	sprintf(labels.l[0].identifier, "%s", "LABEL");

	int r = resolve_labels(&symbols, &labels);
	TEST_ASSERT_EQUAL_INT(1, r);
	TEST_ASSERT_EQUAL_INT(0x202, labels.l[0].byte);
}

void test_resolve_labels_WhereLabelListHasOneLabel_WhereSymbolListDoesNotHaveLabelDefinition(void) {
	RESET;

	symbols.len = 2;
	symbols.s[0].type = SYM_INSTRUCTION;
	symbols.s[1].type = SYM_LABEL;
	symbols.s[1].value = 0;

	labels.len = 1;
	sprintf(labels.l[0].identifier, "%s", "LABEL");

	int r = resolve_labels(&symbols, &labels);
	TEST_ASSERT_EQUAL_INT(1, r);
	TEST_ASSERT_EQUAL_INT(0, labels.l[0].byte);
}

void test_resolve_labels_WhereLabelListHasMultipleLabels_WhereSymbolListHasLabelDefinitions(void) {
	RESET;

	symbols.len = 5;
	symbols.s[0].type = SYM_INSTRUCTION;
	symbols.s[1].type = SYM_LABEL_DEFINITION;
	symbols.s[1].value = 0;
	symbols.s[2].type = SYM_INSTRUCTION;
	symbols.s[3].type = SYM_LABEL_DEFINITION;
	symbols.s[3].value = 1;
	symbols.s[4].type = SYM_INSTRUCTION;

	labels.len = 2;
	sprintf(labels.l[0].identifier, "%s", "LABEL");
	sprintf(labels.l[1].identifier, "%s", "OTHERLABEL");

	int r = resolve_labels(&symbols, &labels);
	TEST_ASSERT_EQUAL_INT(1, r);
	TEST_ASSERT_EQUAL_INT(0x202, labels.l[0].byte);
	TEST_ASSERT_EQUAL_INT(0x204, labels.l[1].byte);
}

void test_resolve_labels_WhereSymbolListIsEmpty(void) {
	RESET;

	labels.len = 1;
	sprintf(labels.l[0].identifier, "%s", "LABEL");

	TEST_ASSERT_EQUAL_INT(0, resolve_labels(&symbols, &labels));
}

void test_resolve_labels_WhereSymbolListIsNull(void) {
	RESET;

	labels.len = 1;
	sprintf(labels.l[0].identifier, "%s", "LABEL");

	TEST_ASSERT_EQUAL_INT(NULL_ARGUMENT_EXCEPTION, resolve_labels(NULL, &labels));
}

void test_resolve_labels_WhereLabelListIsEmpty(void) {
	RESET;

	symbols.len = 1;
	symbols.s[0].type = SYM_DB;

	TEST_ASSERT_EQUAL_INT(1, resolve_labels(&symbols, &labels));
}

void test_resolve_labels_WhereLabelListIsNull(void) {
	RESET;

	symbols.len = 1;
	symbols.s[0].type = SYM_DB;

	TEST_ASSERT_EQUAL_INT(NULL_ARGUMENT_EXCEPTION, resolve_labels(&symbols, NULL));
}

void test_substitute_labels_WhereLabelListContainsAllLabels(void) {
	RESET;

	symbols.len = 7;
	symbols.s[0].type = SYM_INSTRUCTION;
	symbols.s[1].type = SYM_LABEL;
	symbols.s[1].value = 1;
	symbols.s[2].type = SYM_LABEL_DEFINITION;
	symbols.s[2].value = 0;
	symbols.s[3].type = SYM_INSTRUCTION;
	symbols.s[4].type = SYM_LABEL_DEFINITION;
	symbols.s[4].value = 1;
	symbols.s[5].type = SYM_INSTRUCTION;
	symbols.s[6].type = SYM_LABEL;
	symbols.s[6].value = 0;

	labels.len = 2;
	labels.l[0].byte = 0x202;
	labels.l[1].byte = 0x204;

	int r = substitute_labels(&symbols, &labels);

	TEST_ASSERT_EQUAL_INT(1, r);
	TEST_ASSERT_EQUAL_INT(SYM_INT12, symbols.s[1].type);
	TEST_ASSERT_EQUAL_INT(0x204, symbols.s[1].value);
	TEST_ASSERT_EQUAL_INT(SYM_INT12, symbols.s[6].type);
	TEST_ASSERT_EQUAL_INT(0x202, symbols.s[6].value);
}

void test_substitute_labels_WhereLabelListIsMissingLabels(void) {
	RESET;

	symbols.len = 7;
	symbols.s[0].type = SYM_INSTRUCTION;
	symbols.s[1].type = SYM_LABEL;
	symbols.s[1].value = 1;
	symbols.s[2].type = SYM_LABEL_DEFINITION;
	symbols.s[2].value = 0;
	symbols.s[3].type = SYM_INSTRUCTION;
	symbols.s[4].type = SYM_LABEL_DEFINITION;
	symbols.s[4].value = 1;
	symbols.s[5].type = SYM_INSTRUCTION;
	symbols.s[6].type = SYM_LABEL;
	symbols.s[6].value = 0;

	labels.len = 1;
	labels.l[0].byte = 0x202;

	int r = substitute_labels(&symbols, &labels);

	TEST_ASSERT_EQUAL_INT(INVALID_SYMBOL_EXCEPTION, r);
}

void test_substitute_labels_WhereSymbolListIsEmpty(void) {
	RESET;

	labels.len = 1;
	labels.l[0].byte = 0x202;

	int r = substitute_labels(&symbols, &labels);

	TEST_ASSERT_EQUAL_INT(1, r);
}

void test_substitute_labels_WhereSymbolListIsNull(void) {
	RESET;

	int r = substitute_labels(NULL, &labels);

	TEST_ASSERT_EQUAL_INT(NULL_ARGUMENT_EXCEPTION, r);
}

void test_substitute_labels_WhereSymbolListContainsNoLabels_WhereLabelListIsEmpty(void) {
	RESET;

	symbols.len = 4;
	symbols.s[0].type = SYM_INSTRUCTION;
	symbols.s[1].type = SYM_INSTRUCTION;
	symbols.s[2].type = SYM_INSTRUCTION;
	symbols.s[3].type = SYM_DB;

	int r = substitute_labels(&symbols, &labels);

	TEST_ASSERT_EQUAL_INT(1, r);
}

void test_substitute_labels_WhereLabelListIsNull(void) {
	RESET;

	int r = substitute_labels(&symbols, NULL);

	TEST_ASSERT_EQUAL_INT(NULL_ARGUMENT_EXCEPTION, r);
}

int main(void) {
	UNITY_BEGIN();

	RUN_TEST(test_build_instruction_WhereInstructionIsValid);
	RUN_TEST(test_build_instruction_WhereInstructionIsInvalid);
	RUN_TEST(test_build_instruction_WhereInstructionIsValid_WhereIdxIsNegative);
	RUN_TEST(test_build_instruction_WhereInstructioIsValid_WhereSymbolTableIsNull);

	RUN_TEST(test_is_comment_WhereCommentIsEntireString);
	RUN_TEST(test_is_comment_WhereCommentIsAtEndOfString);
	RUN_TEST(test_is_comment_WhereNoCommentIsInString);
	RUN_TEST(test_is_comment_WhereStringIsEmpty);

	RUN_TEST(test_is_db_WhereStringIsDB);
	RUN_TEST(test_is_db_WhereStringIsNotDB);
	RUN_TEST(test_is_db_WhereStringContainsDB);
	RUN_TEST(test_is_db_WithTrailingChars);
	RUN_TEST(test_is_db_WhereStringIsEmpty);
	RUN_TEST(test_is_db_WhereStringIsNull);

	RUN_TEST(test_is_dw_WhereStringIsDW);
	RUN_TEST(test_is_dw_WhereStringIsNotDW);
	RUN_TEST(test_is_dw_WhereStringContainsDW);
	RUN_TEST(test_is_dw_WithTrailingChars);
	RUN_TEST(test_is_dw_WhereStringIsEmpty);
	RUN_TEST(test_is_dw_WhereStringIsNull);

	RUN_TEST(test_is_instruction_WhereStringIsInstruction);
	RUN_TEST(test_is_instruction_WhereStringIsNotInstruction);
	RUN_TEST(test_is_instruction_WhereStringIsEmpty);
	RUN_TEST(test_is_instruction_WhereStringIsNull);

	RUN_TEST(test_is_label_definition_WhereStringIsLabelDefinition);
	RUN_TEST(test_is_label_definition_WhereStringIsNotLabelDefinition);
	RUN_TEST(test_is_label_definition_WhereStringIsEmpty);
	RUN_TEST(test_is_label_definition_WhereStringIsNull);

	RUN_TEST(test_is_label_WhereStringIsLabel);
	RUN_TEST(test_is_label_WhereStringIsNotLabel);
	RUN_TEST(test_is_label_WhereStringIsEmpty);
	RUN_TEST(test_is_label_WhereStringIsNull);
	RUN_TEST(test_is_label_WhereLabelListIsNull);

	RUN_TEST(test_is_register_WhereStringIsRegister_WhereRegisterIsUppercase);
	RUN_TEST(test_is_register_WhereStringIsRegister_RegisterIsLowercase);
	RUN_TEST(test_is_register_WhereStringIsNotRegister);
	RUN_TEST(test_is_register_WhereStringIsEmpty);
	RUN_TEST(test_is_register_WhereStringIsNull);

	RUN_TEST(test_is_reserved_identifier_WhereStringIsReservedIdentifier);
	RUN_TEST(test_is_reserved_identifier_WhereStringIsNotReservedIdentifier);
	RUN_TEST(test_is_register_WhereStringIsEmpty);
	RUN_TEST(test_is_register_WhereStringIsNull);

	RUN_TEST(test_next_symbol_WhereSymbolListIsEmpty);
	RUN_TEST(test_next_symbol_WhereSymbolListIsFull);
	RUN_TEST(test_next_symbol_WhereSymbolListIsNotEmptyOrFull);
	RUN_TEST(test_next_symbol_WhereSymbolListIsNull);

	RUN_TEST(test_populate_labels_WhereLinesHasDuplicateLabelDefinitions);
	RUN_TEST(test_populate_labels_WhereLinesIsEmpty);
	RUN_TEST(test_populate_labels_WhereLinesHasMultipleLabelDefinitions);
	RUN_TEST(test_populate_labels_WhereLabelListIsEmpty);
	RUN_TEST(test_populate_labels_WhereLabelListIsNull);
	RUN_TEST(test_populate_labels_WhereLinesIsNull);

	RUN_TEST(test_resolve_labels_WhereLabelListHasOneLabel_WhereSymbolListHasLabelDefinition);
	RUN_TEST(test_resolve_labels_WhereLabelListHasMultipleLabels_WhereSymbolListHasLabelDefinitions);
	RUN_TEST(test_resolve_labels_WhereSymbolListIsEmpty);
	RUN_TEST(test_resolve_labels_WhereSymbolListIsNull);
	RUN_TEST(test_resolve_labels_WhereLabelListIsEmpty);
	RUN_TEST(test_resolve_labels_WhereLabelListIsNull);

	RUN_TEST(test_substitute_labels_WhereLabelListContainsAllLabels);
	RUN_TEST(test_substitute_labels_WhereLabelListIsMissingLabels);
	RUN_TEST(test_substitute_labels_WhereSymbolListIsEmpty);
	RUN_TEST(test_substitute_labels_WhereSymbolListIsNull);
	RUN_TEST(test_substitute_labels_WhereSymbolListContainsNoLabels_WhereLabelListIsEmpty);
	RUN_TEST(test_substitute_labels_WhereLabelListIsNull);
	return UNITY_END();
}
