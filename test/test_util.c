#include "unity.h"
#include "c8/private/util.c"
#include "c8/private/exception.c"
#include "c8/defs.h"

#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define BUF_SIZE 64
#define CLEAR_BUF for(int i=0;i<BUF_SIZE;i++){buf[i]='\0';}
#define RESET CLEAR_BUF

char buf[BUF_SIZE];

void setUp(void) {
	srand(time(NULL));
}

void tearDown(void) {
}

void test_hex_to_int_WhereStringIsLowercase(void) {
	RESET;

	int n = (rand() % 6) + 10;
	char c = (n + 0x61) - 10;
	TEST_ASSERT_EQUAL_INT(n, hex_to_int(c));
}

void test_hex_to_int_WhereStringIsUppercase(void) {
	RESET;

	int n = (rand() % 6) + 10;
	char c = (n + 0x41) - 10;
	TEST_ASSERT_EQUAL_INT(n, hex_to_int(c));
}

void test_hex_to_int_WhereStringIsDecimal(void) {
	RESET;

	int n = rand() % 10;
	char c = n + 0x30;
	TEST_ASSERT_EQUAL_INT(n, hex_to_int(c));
}

void test_parse_int_WhereStringIsDecimal(void) {
	RESET;

	int n = rand();
	sprintf(buf, "%d", n);
	TEST_ASSERT_EQUAL_INT(n, parse_int(buf));
}

void test_parse_int_WhereStringIsHexWithDollarPrefix(void) {
	RESET;

	int n = rand();
	sprintf(buf, "$%x", n);
	TEST_ASSERT_EQUAL_INT(n, parse_int(buf));
}

void test_parse_int_WhereStringIsHexWithXPrefix(void) {
	RESET;

	int n = rand();
	sprintf(buf, "x%x", n);
	TEST_ASSERT_EQUAL_INT(n, parse_int(buf));
}

void test_parse_int_WhereStringIsHexWithVPrefix(void) {
	RESET;

	int n = rand();
	sprintf(buf, "V%x", n);
	TEST_ASSERT_EQUAL_INT(n, parse_int(buf));
}

void test_parse_int_WhereStringIsEmpty(void) {
	RESET;

	sprintf(buf, "\0");
	TEST_ASSERT_EQUAL_INT(-1, parse_int(buf));
}

void test_parse_int_WhereIntIsZero(void) {
	RESET;

	int n = 0;
	sprintf(buf, "%d", n);
	TEST_ASSERT_EQUAL_INT(n, parse_int(buf));
}

void test_parse_int_WhereStringDoesNotContainInt(void) {
	RESET;

	sprintf(buf, "A string without an integer");
	TEST_ASSERT_EQUAL_INT(-1, parse_int(buf));
}

void test_trim_WhereStringHasLeadingWhitespace(void) {
	RESET;

	const char *content = "Hello there";
	sprintf(buf, "        \t\t  %s", content);
	TEST_ASSERT_EQUAL_STRING(content, trim(buf));
}

void test_trim_WhereStringHasTrailingWhitespace(void) {
	RESET;

	const char *content = "Hello there";
	sprintf(buf, "%s        \t\t\t", content);
	TEST_ASSERT_EQUAL_STRING(content, trim(buf));
}

void test_trim_leading_WhereStringHasLeadingAndTrailingWhitespace(void) {
	RESET;

	const char *content = "Hello there";
	sprintf(buf, "         \t\t       %s   \t\t\t", content);
	TEST_ASSERT_EQUAL_STRING(content, trim(buf));
}

void test_trim_WhereStringHasNoWhitespace(void) {
	RESET;

	const char *content = "Hello there";
	sprintf(buf, "%s", content);
	TEST_ASSERT_EQUAL_STRING(content, trim(buf));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_hex_to_int_WhereStringIsDecimal);
    RUN_TEST(test_hex_to_int_WhereStringIsLowercase);
    RUN_TEST(test_hex_to_int_WhereStringIsUppercase);
	RUN_TEST(test_parse_int_WhereStringIsDecimal);
	RUN_TEST(test_parse_int_WhereStringIsHexWithDollarPrefix);
	RUN_TEST(test_parse_int_WhereStringIsHexWithVPrefix);
	RUN_TEST(test_parse_int_WhereStringIsHexWithXPrefix);
	RUN_TEST(test_parse_int_WhereStringIsEmpty);
	RUN_TEST(test_parse_int_WhereStringDoesNotContainInt);
	RUN_TEST(test_trim_WhereStringHasLeadingWhitespace);
	RUN_TEST(test_trim_WhereStringHasTrailingWhitespace);
	RUN_TEST(test_trim_leading_WhereStringHasLeadingAndTrailingWhitespace);
	RUN_TEST(test_trim_WhereStringHasNoWhitespace);
    return UNITY_END();
}
