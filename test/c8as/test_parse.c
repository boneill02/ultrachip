#include "unity.h"

#include "parse.h"
#include "util/defs.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BYTECODE_SIZE (MEMSIZE - PROG_START)
#define CLEAR_BYTECODE for(int i=0;i<BYTECODE_SIZE;i++){bytecode[i]=0;}
#define CLEAR_BUF for(int i=0;i<64;i++){buf[i]='\0';}
#define RESET CLEAR_BUF; CLEAR_BYTECODE;

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

	char *s = "; A comment\n";
	sprintf(buf, "%s", s);
	int r = parse(buf, bytecode, 0);
	TEST_ASSERT_EQUAL_INT(0, r);
	TEST_ASSERT_EQUAL_INT(0, bytecode[0]);
}

void test_parse_WhereOneValidInstructionExists(void) {
	RESET;

	char *s = "ADD V5, V3\n\n";
	sprintf(buf, "%s", s);
	int r = parse(buf, bytecode, 0);
	TEST_ASSERT_EQUAL_INT(2, r);
	TEST_ASSERT_EQUAL_INT(0x85, bytecode[0]);
	TEST_ASSERT_EQUAL_INT(0x34, bytecode[1]);
}

void test_parse_WhereMultipleValidInstructionsExist(void) {
	RESET;
	// TODO Implement
}

void test_parse_WhereInvalidInstructionsExist(void) {
	RESET;
	// TODO Implement
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