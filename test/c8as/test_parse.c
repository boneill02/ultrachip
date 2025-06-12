#include "unity.h"

#include "parse.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char buf[BUFSIZ];

void setUp(void) {
	srand(time(NULL));
}

void tearDown(void) {
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

int main(void) {
    UNITY_BEGIN();
	RUN_TEST(test_remove_comment_no_comment);
	RUN_TEST(test_remove_comment_end_comment);
	RUN_TEST(test_remove_comment_just_comment);
    return UNITY_END();
}