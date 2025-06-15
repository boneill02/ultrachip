#include "unity.h"

#include "parse.h"
#include "symbol.h"
#include "util/defs.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BYTECODE_SIZE (MEMSIZE - PROG_START)
#define CLEAR_BYTECODE for(int i=0;i<BYTECODE_SIZE;i++){bytecode[i]=0;}
#define CLEAR_BUF for(int i=0;i<BUFSIZ;i++){buf[i]='\0';}
#define CLEAR_EXCEPTION for(int i=0;i<EXCEPTION_MESSAGE_SIZE;i++){exception[i]='\0';}

#define RESET CLEAR_BUF; CLEAR_BYTECODE; CLEAR_EXCEPTION;

char buf[BUFSIZ];
uint8_t *bytecode;
extern char exception[EXCEPTION_MESSAGE_SIZE];
int fmtCount;

void setUp(void) {
	srand(time(NULL));
	bytecode = calloc(BYTECODE_SIZE, 1);
	for (fmtCount = 0; formats[fmtCount].cmd != I_NULL; fmtCount++);
}

void tearDown(void) {
	free(bytecode);
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
	char *s = "ADD V5, V3\n";
	sprintf(buf, "%s", s);
	int r = parse(buf, bytecode, 1);
	TEST_ASSERT_EQUAL_INT(2, r);
	TEST_ASSERT_EQUAL_INT(0x85, bytecode[0]);
	TEST_ASSERT_EQUAL_INT(0x34, bytecode[1]);
	TEST_ASSERT_EQUAL_INT(0, strlen(exception));

	/* Test random instruction */
	RESET;
	generate_valid_instruction_string();
	r = parse(buf, bytecode, 1);
	TEST_ASSERT_EQUAL_INT(2, r); // FAILING
	int bc = bytecode[0] || bytecode[1];
	TEST_ASSERT_GREATER_THAN_INT(0, bc);
	TEST_ASSERT_EQUAL_INT(0, strlen(exception));
}

void test_parse_WhereMultipleValidInstructionsExist(void) {
	RESET;
	// TODO Implement
	generate_valid_instruction_string();
	generate_valid_instruction_string();
	generate_valid_instruction_string();
	generate_valid_instruction_string();
	generate_valid_instruction_string();
	char *s = "AND VF, $31\nOR V1, V9\nDRW V1, V9, $8";
	int r = parse(buf, bytecode, 1);
	TEST_ASSERT_EQUAL_INT(INVALID_INSTRUCTION_EXCEPTION, r); // FAILING
}

void test_parse_WhereInvalidInstructionsExist(void) {
	RESET;
	// TODO Implement
	generate_valid_instruction_string();
	generate_valid_instruction_string();
	generate_valid_instruction_string();
	generate_invalid_instruction_string();
	generate_valid_instruction_string();
	int r = parse(buf, bytecode, 1);
	TEST_ASSERT_EQUAL_INT(INVALID_INSTRUCTION_EXCEPTION, r); // FAILING
}

void test_parse_WhereInvalidSymbolsExist(void) {
	RESET;
	// TODO Implement
}

void test_parse_WhereResultingBytecodeIsTooBig(void) {
	RESET;
	// TODO Implement
}
void test_parse_WhereTooManyLabelsAreDefined(void) {
	RESET;
	// TODO Implement
}

void test_parse_WhereStringIsEmpty(void) {
	RESET;
	// TODO Implement
}

void test_parse_WhereStringIsNull(void) {
	RESET;
	// TODO Implement
}

void test_parse_WhereOutIsNull(void) {
	RESET;
	// TODO Implement
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
	return UNITY_END();
}