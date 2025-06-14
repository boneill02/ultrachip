#include "unity.h"

#include "parse.h"
#include "util/defs.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BYTECODE_SIZE (MEMSIZE - PROG_START)
char buf[BUFSIZ];
uint8_t *bytecode;

void setUp(void) {
	srand(time(NULL));
	bytecode = calloc(BYTECODE_SIZE, 1);
}

void tearDown(void) {
	free(bytecode);
}

void test_remove_comment_WhereStringHasNoComment(void) {
	const char *s = "String without a comment";
	sprintf(buf, "%s", s);
	TEST_ASSERT_EQUAL_STRING(s, remove_comment(buf));
}

void test_remove_comment_WhereStringHasCommentAtEnd(void) {
	const char *s = "String with a comment";

	sprintf(buf, "%s ; comment", s);

	TEST_ASSERT_EQUAL_STRING(s, remove_comment(buf));
}

void test_remove_comment_WhereStringIsOnlyComment(void) {
	const char *s = "; A comment";

	sprintf(buf, "%s", s);
	TEST_ASSERT_EQUAL_INT(0, strlen(remove_comment(buf)));
}

void test_parse_WhereStringIsOnlyComment(void) {
	char *s = "; A comment\n";
	memset(bytecode, 0, BYTECODE_SIZE);


	sprintf(buf, "%s", s);
	int r = parse(buf, bytecode, 0);
	TEST_ASSERT_EQUAL_INT(0, r);
	TEST_ASSERT_EQUAL_INT(0, bytecode[0]);
}

void test_parse_WhereOneValidInstructionExists(void) {
	char *s = "ADD V5, V3\n\n";
	memset(bytecode, 0, BYTECODE_SIZE);


	sprintf(buf, "%s", s);
	int r = parse(buf, bytecode, 0);
	TEST_ASSERT_EQUAL_INT(2, r);
	TEST_ASSERT_EQUAL_INT(0x85, bytecode[0]);
	TEST_ASSERT_EQUAL_INT(0x34, bytecode[1]);
}

void test_parse_WhereStringIsEmpty(void) { }
void test_parse_WhereStringIsNull(void) { }
void test_parse_WhereOutIsNull(void) { }
void test_parse_WhereMultipleValidInstructionsExist(void) { }
void test_parse_WhereInvalidInstructionsExist(void) { }
void test_parse_WhereInvalidSymbolsExist(void) { }
void test_parse_WhereResultingBytecodeIsTooBig(void) { }
void test_parse_WhereTooManyLabelsAreDefined(void) { }

int main(void) {
    UNITY_BEGIN();
	RUN_TEST(test_remove_comment_WhereStringHasNoComment);
	RUN_TEST(test_remove_comment_WhereStringHasCommentAtEnd);
	RUN_TEST(test_remove_comment_WhereStringIsOnlyComment);
	RUN_TEST(test_parse_WhereStringIsOnlyComment);
	RUN_TEST(test_parse_WhereOneValidInstructionExists);
    return UNITY_END();
}