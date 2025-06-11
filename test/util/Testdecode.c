#include "unity.h"
#include "util/decode.h"

#include <stdio.h>

uint8_t *label_map;

void setUp(void) {
	label_map = NULL;
}

void tearDown(void) {
    // clean stuff up here
}

void test_decode_instruction_should_parse(char *expected, uint16_t ins) {
	TEST_ASSERT_EQUAL_STRING(expected, decode_instruction(ins, label_map));
}

void test_decode_instruction_should_parseCLS(void) {
	test_decode_instruction_should_parse("CLS", 0x00E0);
}

void test_decode_instruction_should_parseRET(void) {
	test_decode_instruction_should_parse("RET", 0x00EE);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_decode_instruction_should_parseCLS);
    RUN_TEST(test_decode_instruction_should_parseRET);
    return UNITY_END();
}