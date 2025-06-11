#include "unity.h"
#include "util/util.h"
#include "util/defs.h"

#include <stdint.h>
#include <stdlib.h>
#include <time.h>

char buf[64];

void setUp(void) {
	srand(time(NULL));
}

void tearDown(void) {
}

void test_hex_to_int_lowercase(void) {
	int n = (rand() % 6) + 10;
	char c = (n + 0x61) - 10;

	TEST_ASSERT_EQUAL_INT(n, hex_to_int(c));
}

void test_hex_to_int_uppercase(void) {
	int n = (rand() % 6) + 10;
	char c = (n + 0x41) - 10;

	TEST_ASSERT_EQUAL_INT(n, hex_to_int(c));
}

void test_hex_to_int_decimal(void) {
	int n = rand() % 10;
	char c = n + 0x30;

	TEST_ASSERT_EQUAL_INT(n, hex_to_int(c));
}

void test_parse_int_decimal(void) {
	int n = rand();

	sprintf(buf, "%d", n);
	TEST_ASSERT_EQUAL_INT(n, parse_int(buf));
}

void test_parse_int_hex_dollar(void) {
	int n = rand();

	sprintf(buf, "$%d", n);
	TEST_ASSERT_EQUAL_INT(n, parse_int(buf));
}

void test_parse_int_hex_x(void) {
	int n = rand();

	sprintf(buf, "x%d", n);
	TEST_ASSERT_EQUAL_INT(n, parse_int(buf));
}

void test_parse_int_hex_V(void) {
	int n = rand();

	sprintf(buf, "V%d", n);
	TEST_ASSERT_EQUAL_INT(n, parse_int(buf));
}

void test_parse_int_hex_v(void) {
	int n = rand();

	sprintf(buf, "V%d", n);
	TEST_ASSERT_EQUAL_INT(n, parse_int(buf));
}

void test_parse_int_empty(void) {
	sprintf("\0", buf);
	TEST_ASSERT_EQUAL_INT(-1, parse_int(buf));
}

void test_parse_int_zero(void) {
	int n = 0;

	sprintf(buf, "%d", n);
	TEST_ASSERT_EQUAL_INT(n, parse_int(buf));
}

void test_parse_int_invalid(void) {
	sprintf(buf, "A string without an integer");
	TEST_ASSERT_EQUAL_INT(-1, parse_int(buf));
}

void test_trim_leading_whitespace(void) {
	const char *content = "Hello there";

	sprintf(buf, "        \t\t  %s", content);
	TEST_ASSERT_EQUAL_STRING(content, trim(buf));
}

void test_trim_trailing_whitespace(void) {
	const char *content = "Hello there";

	sprintf(buf, "%s        \t\t\t", content);
	TEST_ASSERT_EQUAL_STRING(content, trim(buf));
}

void test_trim_leading_trailing_whitespace(void) {
	const char *content = "Hello there";

	sprintf(buf, "         \t\t       %s   \t\t\t", content);
	TEST_ASSERT_EQUAL_STRING(content, trim(buf));
}

void test_trim_no_whitespace(void) {
	const char *content = "Hello there";

	sprintf(buf, "%s", content);
	TEST_ASSERT_EQUAL_STRING(content, trim(buf));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_hex_to_int_decimal);
    RUN_TEST(test_hex_to_int_lowercase);
    RUN_TEST(test_hex_to_int_uppercase);
	RUN_TEST(test_parse_int_decimal);
	RUN_TEST(test_parse_int_hex_dollar);
	RUN_TEST(test_parse_int_hex_V);
	RUN_TEST(test_parse_int_hex_v);
	RUN_TEST(test_parse_int_hex_x);
	RUN_TEST(test_parse_int_empty);
	RUN_TEST(test_parse_int_invalid);
	RUN_TEST(test_trim_leading_whitespace);
	RUN_TEST(test_trim_trailing_whitespace);
	RUN_TEST(test_trim_leading_trailing_whitespace);
	RUN_TEST(test_trim_no_whitespace);
    return UNITY_END();
}