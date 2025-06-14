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

void test_remove_comment_no_comment(void) {
	const char *s = "String without a comment";
	sprintf(buf, "%s", s);
	TEST_ASSERT_EQUAL_STRING(s, remove_comment(buf));
}

void test_remove_comment_end_comment(void) {
	const char *s = "String with a comment";

	sprintf(buf, "%s ; comment", s);

	TEST_ASSERT_EQUAL_STRING(s, remove_comment(buf));
}

void test_remove_comment_just_comment(void) {
	const char *s = "; A comment";

	sprintf(buf, "%s", s);
	TEST_ASSERT_EQUAL_INT(0, strlen(remove_comment(buf)));
}

void test_parse_just_comment(void) {
	char *s = "; A comment\n";
	memset(bytecode, 0, BYTECODE_SIZE);


	sprintf(buf, "%s", s);
	int r = parse(buf, bytecode, 0);
	TEST_ASSERT_EQUAL_INT(0, r);
	TEST_ASSERT_EQUAL_INT(0, bytecode[0]);
}

void test_parse_valid_instruction(void) {
	char *s = "ADD V5, V3\n";
	memset(bytecode, 0, BYTECODE_SIZE);


	sprintf(buf, "%s", s);
	int r = parse(buf, bytecode, 0);
	TEST_ASSERT_EQUAL_INT(2, r);
	TEST_ASSERT_EQUAL_INT(0x85, bytecode[0]);
	TEST_ASSERT_EQUAL_INT(0x34, bytecode[1]);
}

void test_parse_empty_string(void) { }
void test_parse_null_string(void) { }
void test_parse_null_out(void) { }
void test_parse_multiple_lines(void) { }
void test_parse_invalid_instruction(void) { }
void test_parse_invalid_symbol(void) { }
void test_parse_too_many_bytes(void) { }
void test_parse_too_many_labels(void) { }

int main(void) {
    UNITY_BEGIN();
	RUN_TEST(test_remove_comment_no_comment);
	RUN_TEST(test_remove_comment_end_comment);
	RUN_TEST(test_remove_comment_just_comment);
	RUN_TEST(test_parse_just_comment);
	RUN_TEST(test_parse_valid_instruction);
    return UNITY_END();
}