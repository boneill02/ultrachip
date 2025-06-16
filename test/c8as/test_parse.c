#include "unity.h"

#include "parse.c"
#include "util/defs.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BYTECODE_SIZE (MEMSIZE - PROG_START)
#define BUF_SIZE (BYTECODE_SIZE * MAX_LINE_LENGTH)

#define CLEAR_BYTECODE for(int i=0;i<BYTECODE_SIZE;i++){bytecode[i]=0;}
#define CLEAR_BUF for(int i=0;i<BUF_SIZE;i++){buf[i]='\0';}
#define CLEAR_EXCEPTION for(int i=0;i<EXCEPTION_MESSAGE_SIZE;i++){exception[i]='\0';}
#define CLEAR_LABELS \
	memset(labels.l,0,LABEL_CEILING*sizeof(label_t)); \
	labels.len=0; \
	labels.ceil=LABEL_CEILING;
#define CLEAR_SYMBOLS \
	memset(labels.l, 0, SYMBOL_CEILING*sizeof(symbol_t)); \
	labels.len=0; \
	labels.ceil = LABEL_CEILING;
#define RESET CLEAR_SYMBOLS; CLEAR_LABELS; CLEAR_BUF; CLEAR_BYTECODE; CLEAR_EXCEPTION;

char buf[BUF_SIZE];
uint8_t *bytecode;
extern char exception[EXCEPTION_MESSAGE_SIZE];
int fmtCount;
int insCount;

symbol_list_t symbols;
label_list_t labels;

void setUp(void) {
	srand(time(NULL));
	bytecode = calloc(BYTECODE_SIZE, 1);
	for (fmtCount = 0; formats[fmtCount].cmd != I_NULL; fmtCount++);
	for (insCount = 0; instructionStrings[insCount] != NULL; insCount++);
	symbols.s = calloc(SYMBOL_CEILING, sizeof(symbol_t));
	symbols.ceil = SYMBOL_CEILING;
	labels.l = calloc(LABEL_CEILING, sizeof(label_t));
	labels.ceil = LABEL_CEILING;
}

void tearDown(void) {
	free(bytecode);
	free(symbols.s);
	free(labels.l);
}

void generate_valid_instruction_string(void) {
	instruction_format_t *f;

	f = &formats[rand() % fmtCount];

	sprintf(buf, "%s", instructionStrings[f->cmd]);
	for (int i = 0; i < f->pcount; i++) {
		switch (f->ptype[i]) {
			case SYM_INT4:
				sprintf(buf, " $%01x", rand() % 0x10);
				break;
			case SYM_INT8:
				sprintf(buf, " $%02x", rand() % 0x100);
				break;
			case SYM_INT12:
				sprintf(buf, " $%03x", rand() % 0x1000);
				break;
			case SYM_V:
				sprintf(buf, " V%01x", rand() % 0x10);
				break;
			default:
				sprintf(buf, " %s", identifierStrings[f->ptype[i]]);
		}
	}
	sprintf(buf, "\n");
}

void generate_invalid_instruction_string(void) {
	instruction_format_t *f;

	f = &formats[rand() % fmtCount];

	sprintf(buf, "%s", instructionStrings[f->cmd]);
	if (f->pcount == 0) {
		sprintf(buf, " $10");
	}
	sprintf(buf, "\n");
}

void test_remove_comment_WhereStringHasNoComment(void) {
	RESET;

	const char *s = "String without a comment";
	sprintf(buf, "%s", s);
	TEST_ASSERT_EQUAL_STRING(s, remove_comment(buf));
}

void test_remove_comment_WhereStringHasCommentAtEnd(void) {
	RESET;

	const char *s = "String with a comment";
	sprintf(buf, "%s ; comment", s);
	TEST_ASSERT_EQUAL_STRING(s, remove_comment(buf));
}

void test_remove_comment_WhereStringIsOnlyComment(void) {
	RESET;

	const char *s = "; A comment";
	sprintf(buf, "%s", s);
	TEST_ASSERT_EQUAL_INT(0, strlen(remove_comment(buf)));
}

void test_parse_WhereStringIsOnlyComment(void) {
	RESET;

	char *s = "; A comment";
	sprintf(buf, "%s\n", s);
	int r = parse(buf, bytecode, ARG_VERBOSE);
	TEST_ASSERT_EQUAL_INT(0, r);
	TEST_ASSERT_EQUAL_INT(0, bytecode[0]);
}

void test_parse_WhereOneValidInstructionExists(void) {
	RESET;

	/* Test specific instruction to match bytecode */
	char *s = "ADD V5, V3";
	sprintf(buf, "%s\n", s);
	int r = parse(buf, bytecode, 1);
	printf("%s\n", buf);
	TEST_ASSERT_EQUAL_INT(2, r);
	TEST_ASSERT_EQUAL_INT(0x85, bytecode[0]);
	TEST_ASSERT_EQUAL_INT(0x34, bytecode[1]);
	TEST_ASSERT_EQUAL_INT(0, strlen(exception));
}

void test_parse_WhereMultipleValidInstructionsExist(void) {
	RESET;

	char *s = "AND VF, $31\nOR V1, V9\nDRW V1, V9, $8\n";
	sprintf(buf, "%s", s);
	int r = parse(buf, bytecode, 1);
	TEST_ASSERT_EQUAL_INT(INVALID_INSTRUCTION_EXCEPTION, r);
}

void test_parse_WhereInvalidInstructionsExist(void) {
	RESET;

	char *s = "OR V1, V9\nAND $31\nDRW V1, V9, $8";
	sprintf(buf, "%s", s);
	int r = parse(buf, bytecode, 1);
	TEST_ASSERT_EQUAL_INT(INVALID_INSTRUCTION_EXCEPTION, r);
}

void test_parse_WhereInvalidSymbolsExist(void) {
	RESET;

	sprintf(buf, "invalid\n");
	int r = parse(buf, bytecode, 1);
	TEST_ASSERT_EQUAL_INT(INVALID_SYMBOL_EXCEPTION, r);
}

void test_parse_WhereResultingBytecodeIsTooBig(void) {
	RESET;

	const char *s = "AND V1, V9\n";
	int len = strlen(s);

	for (int i = 0; i < ((MEMSIZE - PROG_START) / 2) + 1; i++) {
		sprintf(buf + len, "%s", s);
	}
	int r = parse(buf, bytecode, 1);
	TEST_ASSERT_EQUAL_INT(TOO_MANY_SYMBOLS_EXCEPTION, r);
}

void test_parse_WhereTooManyLabelsAreDefined(void) {
	RESET;
	int len = 0;
	const char *s = "l";
	int slen = strlen(s);
	
	for (int i = 0; i < LABEL_CEILING; i++) {
		sprintf(&buf[len], "%s%02d:\n", s, i);
		len += slen + 4;
	}

	sprintf(buf + len, "ADD V1 V2\n");

	int r = parse(buf, bytecode, ARG_VERBOSE);
	TEST_ASSERT_EQUAL_INT(TOO_MANY_LABELS_EXCEPTION, r);
}

void test_parse_WhereStringIsEmpty(void) {
	RESET;

	int r = parse(buf, bytecode, ARG_VERBOSE);
	TEST_ASSERT_EQUAL_INT(0, r);
}

void test_parse_WhereStringIsNull(void) {
	RESET;

	int r = parse(NULL, bytecode, ARG_VERBOSE);
	TEST_ASSERT_EQUAL_INT(NULL_ARGUMENT_EXCEPTION, r);
}

void test_parse_WhereOutIsNull(void) {
	RESET;

	int r = parse(buf, NULL, ARG_VERBOSE);
	TEST_ASSERT_EQUAL_INT(NULL_ARGUMENT_EXCEPTION, r);
}

void line_count_WhereStringHasMultipleLines(void) {

	RESET;

	const char *s = "ABCD\nEFGH\nIJKL\n";
	TEST_ASSERT_EQUAL_INT(4, line_count(s));
}

void line_count_WhereStringHasOneLine(void) {
	RESET;

	const char *s = "blablabla";
	TEST_ASSERT_EQUAL_INT(1, line_count(s));
}

void test_line_count_WhereStringIsEmpty(void) {
	RESET;
	
	TEST_ASSERT_EQUAL_INT(NULL_ARGUMENT_EXCEPTION, line_count(buf));
}

void test_line_count_WhereStringIsNull(void) {
	RESET;
	
	TEST_ASSERT_EQUAL_INT(NULL_ARGUMENT_EXCEPTION, line_count(NULL));
}

void test_parse_word_WhereWordIsLabelDefinition(void) {
	RESET;

	const char *s = "ldef";

	sprintf(buf, "%s:", s);
	labels.len = 1;
	sprintf(labels.l[0].identifier, "%s", s);
	int r = parse_word(buf, NULL, 1, &symbols.s[0], &labels);

	TEST_ASSERT_EQUAL_INT(0, r);
	TEST_ASSERT_EQUAL_INT(SYM_LABEL_DEFINITION, symbols.s[0].type);
	TEST_ASSERT_EQUAL_INT(0, symbols.s[0].value);
}

void test_parse_word_WhereWordIsInstruction(void) {
	RESET;

	int ins = rand() % insCount;

	sprintf(buf, "%s", instructionStrings[ins]);
	int r = parse_word(buf, NULL, 1, &symbols.s[0], &labels);

	TEST_ASSERT_EQUAL_INT(0, r);
	TEST_ASSERT_EQUAL_INT(SYM_INSTRUCTION, symbols.s[0].type);
	TEST_ASSERT_EQUAL_INT(ins, symbols.s[0].value);
}

void test_parse_word_WhereWordIsDB(void) {
	RESET;

	int v = rand() % UINT8_MAX;
	sprintf(buf, "%s", S_DB, v);
	sprintf(buf+10, "%d", v);
	int r = parse_word(buf, buf + 10, 1, &symbols.s[0], &labels);

	TEST_ASSERT_EQUAL_INT(1, r);
	TEST_ASSERT_EQUAL_INT(SYM_DB, symbols.s[0].type);
	TEST_ASSERT_EQUAL_INT(v, symbols.s[0].value);
}

void test_parse_word_WhereWordIsDW(void) {
	RESET;

	int v = rand() % UINT8_MAX;
	sprintf(buf, "%s", S_DW, v);
	sprintf(buf+10, "%d", v, v);
	int r = parse_word(buf, buf + 10, 1, &symbols.s[0], &labels);

	TEST_ASSERT_EQUAL_INT(1, r);
	TEST_ASSERT_EQUAL_INT(SYM_DW, symbols.s[0].type);
	TEST_ASSERT_EQUAL_INT(v, symbols.s[0].value);
}

void test_parse_word_WhereWordIsRegister(void) {
	RESET;

	int v = rand() % 0x10;
	sprintf(buf, "V%01x", v);
	int r = parse_word(buf, NULL, 1, &symbols.s[0], &labels);

	TEST_ASSERT_EQUAL_INT(0, r);
	TEST_ASSERT_EQUAL_INT(SYM_V, symbols.s[0].type);
	TEST_ASSERT_EQUAL_INT(v, symbols.s[0].value);
}

void test_parse_word_WhereWordIsReservedIdentifier(void) {
	RESET;

	sprintf(buf, "%s", S_HF);
	int r = parse_word(buf, NULL, 1, &symbols.s[0], &labels);

	TEST_ASSERT_EQUAL_INT(0, r);
	TEST_ASSERT_EQUAL_INT(SYM_HF, symbols.s[0].type);
	TEST_ASSERT_EQUAL_INT(0, symbols.s[0].value);
	
	RESET;
	sprintf(buf, "%s", S_IP);
	r = parse_word(buf, NULL, 1, &symbols.s[0], &labels);

	TEST_ASSERT_EQUAL_INT(0, r);
	TEST_ASSERT_EQUAL_INT(SYM_IP, symbols.s[0].type);
	TEST_ASSERT_EQUAL_INT(0, symbols.s[0].value);
}

void test_parse_word_WhereWordIsInt(void) {
	RESET;
	int v4 = rand() % 0x10;
	int v8 = rand() % 0x100;
	int v12 = rand() % 0x1000;

	sprintf(buf, "$%x", v4);
	int r = parse_word(buf, NULL, 1, &symbols.s[0], &labels);
	TEST_ASSERT_EQUAL_INT(0, r);
	TEST_ASSERT_EQUAL_INT(SYM_INT4, symbols.s[0].type);
	TEST_ASSERT_EQUAL_INT(v4, symbols.s[0].value);

	RESET;

	sprintf(buf, "$%x", v8);
	r = parse_word(buf, NULL, 1, &symbols.s[0], &labels);
	TEST_ASSERT_EQUAL_INT(0, r);
	TEST_ASSERT_EQUAL_INT(SYM_INT8, symbols.s[0].type);
	TEST_ASSERT_EQUAL_INT(v8, symbols.s[0].value);

	RESET;

	sprintf(buf, "$%x", v12);
	r = parse_word(buf, NULL, 1, &symbols.s[0], &labels);
	TEST_ASSERT_EQUAL_INT(0, r);
	TEST_ASSERT_EQUAL_INT(SYM_INT12, symbols.s[0].type);
	TEST_ASSERT_EQUAL_INT(v12, symbols.s[0].value);
}

void test_parse_word_WhereWordIsLabel(void) {
	RESET;

	const char *l = "LABEL";
	labels.len = 2;
	sprintf(labels.l[0].identifier, "otherlabel");
	sprintf(labels.l[1].identifier, l);
	sprintf(buf, "%s", l);

	int r = parse_word(buf, NULL, 1, &symbols.s[0], &labels);
	TEST_ASSERT_EQUAL_INT(0, r);
	TEST_ASSERT_EQUAL_INT(SYM_LABEL, symbols.s[0].type);
	TEST_ASSERT_EQUAL_INT(1, symbols.s[0].value);
}

void test_parse_word_WhereWordIsInvalid(void) {
	RESET;

	const char *s = "Invalid";
	sprintf(buf, "%s", s);
	
	int r = parse_word(buf, NULL, 1, &symbols.s[0], &labels);
	TEST_ASSERT_EQUAL_INT(INVALID_SYMBOL_EXCEPTION, r);
	TEST_ASSERT_EQUAL_INT(0, symbols.len);
}

int main(void) {
    UNITY_BEGIN();
	RUN_TEST(test_remove_comment_WhereStringHasNoComment);
	RUN_TEST(test_remove_comment_WhereStringHasCommentAtEnd);
	RUN_TEST(test_remove_comment_WhereStringIsOnlyComment);
	RUN_TEST(test_parse_WhereStringIsOnlyComment);
	RUN_TEST(test_parse_WhereOneValidInstructionExists);
	RUN_TEST(test_parse_WhereMultipleValidInstructionsExist);
	RUN_TEST(test_parse_WhereInvalidInstructionsExist);
	RUN_TEST(test_parse_WhereInvalidSymbolsExist);
	RUN_TEST(test_parse_WhereResultingBytecodeIsTooBig);
	RUN_TEST(test_parse_WhereTooManyLabelsAreDefined);
	RUN_TEST(test_parse_WhereStringIsEmpty);
	RUN_TEST(test_parse_WhereStringIsNull);
	RUN_TEST(test_parse_WhereOutIsNull);
	RUN_TEST(test_parse_word_WhereWordIsDB);
	RUN_TEST(test_parse_word_WhereWordIsDW);
	RUN_TEST(test_parse_word_WhereWordIsInstruction);
	RUN_TEST(test_parse_word_WhereWordIsRegister);
	RUN_TEST(test_parse_word_WhereWordIsReservedIdentifier);
	RUN_TEST(test_parse_word_WhereWordIsInt);
	RUN_TEST(test_parse_word_WhereWordIsLabel);
	RUN_TEST(test_parse_word_WhereWordIsInvalid);
	return UNITY_END();
}