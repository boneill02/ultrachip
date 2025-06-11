#include "unity.h"
#include "util/decode.h"
#include "util/defs.h"

#include <stdint.h>

#define FORMAT_A(a) ((a << 12) & 0xF000)
#define FORMAT_X(x) ((x << 8) & 0x0F00)
#define FORMAT_Y(y) ((y << 4) & 0x00F0)
#define FORMAT_B(b) (b & 0x000F)
#define FORMAT_KK(kk) (kk & 0x00FF)
#define FORMAT_NNN(nnn) (nnn & 0x0FFF)

#define BUILD_INSTRUCTION_AXYB(a, x, y, b) \
	(FORMAT_A(a) | FORMAT_X(x) | FORMAT_Y(y) | FORMAT_B(b))

#define BUILD_INSTRUCTION_AXKK(a, x, kk) \
	(FORMAT_A(a) | FORMAT_X(x) | FORMAT_KK(kk))

#define BUILD_INSTRUCTION_ANNN(a, nnn) \
	(FORMAT_A(a) | FORMAT_NNN(nnn))

uint8_t label_map[MEMSIZE];

void setUp(void) {
}

void tearDown(void) {
}

void test_decode_instruction_should_parse_(char *expected, uint16_t ins) {
	TEST_ASSERT_EQUAL_STRING(expected, decode_instruction(ins, label_map));
}

void test_decode_instruction_should_parse_CLS(void) {
	test_decode_instruction_should_parse_("CLS", 0x00E0);
}

void test_decode_instruction_should_parse_RET(void) {
	test_decode_instruction_should_parse_("RET", 0x00EE);
}

void test_decode_instruction_should_parse_SCD(void) {
	uint8_t b = 0x5;
	uint16_t ins = 0x00C0 | b;
	test_decode_instruction_should_parse_("SCD 5", ins);
}

void test_decode_instruction_should_parse_SCR(void) {
	test_decode_instruction_should_parse_("SCR", 0x00FB);
}

void test_decode_instruction_should_parse_SCL(void) {
	test_decode_instruction_should_parse_("SCL", 0x00FC);
}

void test_decode_instruction_should_parse_EXIT(void) {
	test_decode_instruction_should_parse_("EXIT", 0x00FD);
}

void test_decode_instruction_should_parse_LOW(void) {
	test_decode_instruction_should_parse_("LOW", 0x00FE);
}

void test_decode_instruction_should_parse_HIGH(void) {
	test_decode_instruction_should_parse_("HIGH", 0x00FF);
}

void test_decode_instruction_should_parse_JP(void) {
	int nnn = 0x432;
	int ins = BUILD_INSTRUCTION_ANNN(1, nnn);

	label_map[nnn] = 0;
	test_decode_instruction_should_parse_("JP $432", ins);
}

void test_decode_instruction_should_parse_JP_with_label(void) {
	int nnn = 0x432;
	int ins = BUILD_INSTRUCTION_ANNN(1, nnn);

	label_map[nnn] = 1;
	test_decode_instruction_should_parse_("JP label1", ins);
}

void test_decode_instruction_should_parse_CALL(void) {
	int nnn = 0x500;
	int ins = BUILD_INSTRUCTION_ANNN(2, nnn);

	label_map[nnn] = 0;
	test_decode_instruction_should_parse_("CALL $500", ins);
}

void test_decode_instruction_should_parse_CALL_with_label(void) {
	int nnn = 0x500;
	int ins = BUILD_INSTRUCTION_ANNN(2, nnn);

	label_map[nnn] = 1;
	test_decode_instruction_should_parse_("CALL label1", ins);
}

void test_decode_instruction_should_parse_SEVKK(void) {
	int x = 0x5;
	int kk = 0x85;
	int ins = BUILD_INSTRUCTION_AXKK(3, x, kk);

	test_decode_instruction_should_parse_("SE V5, $85", ins);
}

void test_decode_instruction_should_parse_SNEVKK(void) {
	int x = 0x5;
	int kk = 0x85;
	int ins = BUILD_INSTRUCTION_AXKK(4, x, kk);

	test_decode_instruction_should_parse_("SNE V5, $85", ins);
}

void test_decode_instruction_should_parse_SEVV(void) {
	int x = 0x5;
	int y = 0x8;
	int ins = BUILD_INSTRUCTION_AXYB(5, x, y, 0);

	test_decode_instruction_should_parse_("SE V5, V8", ins);
}

void test_decode_instruction_should_parse_LDVKK(void) {
	int x = 0x5;
	int kk = 0x10;
	int ins = BUILD_INSTRUCTION_AXKK(6, x, kk);

	test_decode_instruction_should_parse_("LD V5, $10", ins);
}

void test_decode_instruction_should_parse_ADDVKK(void) {
	int x = 0x1;
	int kk = 0x9F;
	int ins = BUILD_INSTRUCTION_AXKK(7, x, kk);

	test_decode_instruction_should_parse_("ADD V1, $9f", ins);
}

void test_decode_instruction_should_parse_LDVV(void) {
	int x = 0x0;
	int y = 0x5;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 0);

	test_decode_instruction_should_parse_("LD V0, V5", ins);
}

void test_decode_instruction_should_parse_ORVV(void) {
	int x = 0x0;
	int y = 0x5;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 1);

	test_decode_instruction_should_parse_("OR V0, V5", ins);
}

void test_decode_instruction_should_parse_ANDVV(void) {
	int x = 0x0;
	int y = 0x5;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 2);

	test_decode_instruction_should_parse_("AND V0, V5", ins);
}

void test_decode_instruction_should_parse_XORVV(void) {
	int x = 0x0;
	int y = 0x5;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 3);

	test_decode_instruction_should_parse_("XOR V0, V5", ins);
}

void test_decode_instruction_should_parse_ADDVV(void) {
	int x = 0x0;
	int y = 0x5;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 4);

	test_decode_instruction_should_parse_("ADD V0, V5", ins);
}

void test_decode_instruction_should_parse_SUBVV(void) {
	int x = 0x0;
	int y = 0x5;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 5);

	test_decode_instruction_should_parse_("SUB V0, V5", ins);
}

void test_decode_instruction_should_parse_SHRV(void) {
	int x = 0x0;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, 0, 6);

	test_decode_instruction_should_parse_("SHR V0", ins);
}

void test_decode_instruction_should_parse_SUBNVV(void) {
	int x = 0x0;
	int y = 0x5;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 7);

	test_decode_instruction_should_parse_("SUBN V0, V5", ins);
}

void test_decode_instruction_should_parse_SHLV(void) {
	int x = 0x0;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, 0, 0xE);

	test_decode_instruction_should_parse_("SHL V0", ins);
}

void test_decode_instruction_should_parse_SNEVV(void) {
	int x = 0x0;
	int y = 0x5;
	int ins = BUILD_INSTRUCTION_AXYB(9, x, y, 0);

	test_decode_instruction_should_parse_("SNE V0, V5", ins);
}

void test_decode_instruction_should_parse_LDINNN(void) {
	int nnn = 0x504;
	int ins = BUILD_INSTRUCTION_ANNN(0xa, nnn);

	label_map[nnn] = 0;
	test_decode_instruction_should_parse_("LD I, $504", ins);
}

void test_decode_instruction_should_parse_LDINNN_with_label(void) {
	int nnn = 0x504;
	int ins = BUILD_INSTRUCTION_ANNN(0xa, nnn);

	label_map[nnn] = 1;
	test_decode_instruction_should_parse_("LD I, label1", ins);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_decode_instruction_should_parse_CLS);
    RUN_TEST(test_decode_instruction_should_parse_RET);
    RUN_TEST(test_decode_instruction_should_parse_SCD);
    RUN_TEST(test_decode_instruction_should_parse_SCR);
    RUN_TEST(test_decode_instruction_should_parse_SCL);
    RUN_TEST(test_decode_instruction_should_parse_EXIT);
    RUN_TEST(test_decode_instruction_should_parse_LOW);
    RUN_TEST(test_decode_instruction_should_parse_HIGH);
    RUN_TEST(test_decode_instruction_should_parse_JP);
    RUN_TEST(test_decode_instruction_should_parse_JP_with_label);
    RUN_TEST(test_decode_instruction_should_parse_CALL);
    RUN_TEST(test_decode_instruction_should_parse_CALL_with_label);
    RUN_TEST(test_decode_instruction_should_parse_SEVKK);
    RUN_TEST(test_decode_instruction_should_parse_SNEVKK);
    RUN_TEST(test_decode_instruction_should_parse_SEVV);
    RUN_TEST(test_decode_instruction_should_parse_LDVKK);
    RUN_TEST(test_decode_instruction_should_parse_ADDVKK);
    RUN_TEST(test_decode_instruction_should_parse_LDVV);
    RUN_TEST(test_decode_instruction_should_parse_ORVV);
    RUN_TEST(test_decode_instruction_should_parse_ANDVV);
    RUN_TEST(test_decode_instruction_should_parse_XORVV);
    RUN_TEST(test_decode_instruction_should_parse_ADDVV);
    RUN_TEST(test_decode_instruction_should_parse_SUBVV);
    RUN_TEST(test_decode_instruction_should_parse_SHRV);
    RUN_TEST(test_decode_instruction_should_parse_SUBNVV);
    RUN_TEST(test_decode_instruction_should_parse_SHLV);
    RUN_TEST(test_decode_instruction_should_parse_SNEVV);
    RUN_TEST(test_decode_instruction_should_parse_LDINNN);
    RUN_TEST(test_decode_instruction_should_parse_LDINNN_with_label);
    return UNITY_END();
}