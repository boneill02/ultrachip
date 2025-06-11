#include "unity.h"
#include "util/decode.h"
#include "util/defs.h"

#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define FORMAT_A(a) ((a << 12) & 0xF000)
#define FORMAT_X(x) ((x << 8) & 0x0F00)
#define FORMAT_Y(y) ((y << 4) & 0x00F0)
#define FORMAT_B(b) (b & 0x000F)
#define FORMAT_KK(kk) (kk & 0x00FF)
#define FORMAT_NNN(nnn) (nnn & 0x0FFF)
#define GENERATE_RANDOMS \
	int x = rand() % 0xF; \
	int y = rand() % 0xF; \
	int kk = rand() % 0xFF; \
	int b = rand() % 0xF; \
	int nnn = rand() % 0xFFF; \
	int label = rand() % 64;

#define BUILD_INSTRUCTION_AXYB(a, x, y, b) \
	(FORMAT_A(a) | FORMAT_X(x) | FORMAT_Y(y) | FORMAT_B(b))

#define BUILD_INSTRUCTION_AXKK(a, x, kk) \
	(FORMAT_A(a) | FORMAT_X(x) | FORMAT_KK(kk))

#define BUILD_INSTRUCTION_ANNN(a, nnn) \
	(FORMAT_A(a) | FORMAT_NNN(nnn))

uint8_t label_map[MEMSIZE];
char buf[64];

void setUp(void) {
	srand(time(NULL));
}

void tearDown(void) {
}

void test_decode_instruction_should_parse(char *expected, uint16_t ins) {
	TEST_ASSERT_EQUAL_STRING(expected, decode_instruction(ins, label_map));
}

void test_decode_instruction_should_parse_CLS(void) {
	test_decode_instruction_should_parse("CLS", 0x00E0);
}

void test_decode_instruction_should_parse_RET(void) {
	test_decode_instruction_should_parse("RET", 0x00EE);
}

void test_decode_instruction_should_parse_SCD(void) {
	GENERATE_RANDOMS;
	uint16_t ins = 0x00C0 | b;

	sprintf(buf, "SCD %d", b);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_SCR(void) {
	test_decode_instruction_should_parse("SCR", 0x00FB);
}

void test_decode_instruction_should_parse_SCL(void) {
	test_decode_instruction_should_parse("SCL", 0x00FC);
}

void test_decode_instruction_should_parse_EXIT(void) {
	test_decode_instruction_should_parse("EXIT", 0x00FD);
}

void test_decode_instruction_should_parse_LOW(void) {
	test_decode_instruction_should_parse("LOW", 0x00FE);
}

void test_decode_instruction_should_parse_HIGH(void) {
	test_decode_instruction_should_parse("HIGH", 0x00FF);
}

void test_decode_instruction_should_parse_JP(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_ANNN(1, nnn);
	label_map[nnn] = 0;

	sprintf(buf, "JP $%03x", nnn);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_JP_with_label(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_ANNN(1, nnn);
	label_map[nnn] = label;

	sprintf(buf, "JP label%d", label);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_CALL(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_ANNN(2, nnn);
	label_map[nnn] = 0;

	sprintf(buf, "CALL $%03x", nnn);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_CALL_with_label(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_ANNN(2, nnn);
	label_map[nnn] = label;

	sprintf(buf, "CALL label%d", label);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_SEVKK(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_AXKK(3, x, kk);

	sprintf(buf, "SE V%01x, $%02x", x, kk);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_SNEVKK(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_AXKK(4, x, kk);

	sprintf(buf, "SNE V%01x, $%02x", x, kk);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_SEVV(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_AXYB(5, x, y, 0);

	sprintf(buf, "SE V%01x, V%01x", x, y);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_LDVKK(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_AXKK(6, x, kk);

	sprintf(buf, "LD V%01x, $%02x", x, kk);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_ADDVKK(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_AXKK(7, x, kk);

	sprintf(buf, "ADD V%01x, $%02x", x, kk);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_LDVV(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 0);

	sprintf(buf, "LD V%01x, V%01x", x, y);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_ORVV(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 1);

	sprintf(buf, "OR V%01x, V%01x", x, y);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_ANDVV(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 2);

	sprintf(buf, "AND V%01x, V%01x", x, y);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_XORVV(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 3);

	sprintf(buf, "XOR V%01x, V%01x", x, y);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_ADDVV(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 4);

	sprintf(buf, "ADD V%01x, V%01x", x, y);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_SUBVV(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 5);

	sprintf(buf, "SUB V%01x, V%01x", x, y);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_SHRV(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, 0, 6);

	sprintf(buf, "SHR V%01x", x);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_SUBNVV(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 7);

	sprintf(buf, "SUBN V%01x, V%01x", x, y);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_SHLV(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, 0, 0xE);

	sprintf(buf, "SHL V%01x", x);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_SNEVV(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_AXYB(9, x, y, 0);

	sprintf(buf, "SNE V%01x, V%01x", x, y);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_LDINNN(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_ANNN(0xA, nnn);
	label_map[nnn] = 0;

	sprintf(buf, "LD I, $%03x", nnn);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_LDINNN_with_label(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_ANNN(0xA, nnn);
	label_map[nnn] = label;

	sprintf(buf, "LD I, label%d", label);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_JPV0NNN(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_ANNN(0xB, nnn);
	label_map[nnn] = 0;

	sprintf(buf, "JP V0, $%03x", nnn);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_JPV0NNN_with_label(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_ANNN(0xB, nnn);
	label_map[nnn] = label;

	sprintf(buf, "JP V0, label%d", label);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_RNDVKK(void) {
	GENERATE_RANDOMS;
	uint16_t ins = BUILD_INSTRUCTION_AXKK(0xC, x, kk);

	sprintf(buf, "RND V%01x, $%02x", x, kk);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_DRWVVB(void) {
	GENERATE_RANDOMS;
	uint16_t ins = BUILD_INSTRUCTION_AXYB(0xD, x, y, b);

	sprintf(buf, "DRW V%01x, V%01x, $%01x", x, y, b);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_SKPV(void) {
	GENERATE_RANDOMS;
	uint16_t ins = BUILD_INSTRUCTION_AXKK(0xE, x, 0x9E);

	sprintf(buf, "SKP V%01x", x);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_SKNPV(void) {
	GENERATE_RANDOMS;
	uint16_t ins = BUILD_INSTRUCTION_AXKK(0xE, x, 0xA1);

	sprintf(buf, "SKNP V%01x", x);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_LDVDT(void) {
	GENERATE_RANDOMS;
	uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x07);

	sprintf(buf, "LD V%01x, DT", x);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_LDVK(void) {
	GENERATE_RANDOMS;
	uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x0A);

	sprintf(buf, "LD V%01x, K", x);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_LDDTV(void) {
	GENERATE_RANDOMS;
	uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x15);

	sprintf(buf, "LD DT, V%01x", x);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_LDSTV(void) {
	GENERATE_RANDOMS;
	uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x18);

	sprintf(buf, "LD ST, V%01x", x);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_ADDIV(void) {
	GENERATE_RANDOMS;
	uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x1E);

	sprintf(buf, "ADD I, V%01x", x);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_LDFV(void) {
	GENERATE_RANDOMS;
	uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x29);

	sprintf(buf, "LD F, V%01x", x);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_LDHFV(void) {
	GENERATE_RANDOMS;
	uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x30);

	sprintf(buf, "LD HF, V%01x", x);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_LDBV(void) {
	GENERATE_RANDOMS;
	uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x33);

	sprintf(buf, "LD B, V%01x", x);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_LDIPV(void) {
	GENERATE_RANDOMS;
	uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x55);

	sprintf(buf, "LD [I], V%01x", x);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_LDVIP(void) {
	GENERATE_RANDOMS;
	uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x65);

	sprintf(buf, "LD V%01x, [I]", x);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_LDRV(void) {
	GENERATE_RANDOMS;
	uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x75);

	sprintf(buf, "LD R, V%01x", x);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_should_parse_LDVR(void) {
	GENERATE_RANDOMS;
	uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x85);

	sprintf(buf, "LD V%01x, R", x);
	test_decode_instruction_should_parse(buf, ins);
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
    RUN_TEST(test_decode_instruction_should_parse_JPV0NNN);
    RUN_TEST(test_decode_instruction_should_parse_JPV0NNN_with_label);
    RUN_TEST(test_decode_instruction_should_parse_RNDVKK);
    RUN_TEST(test_decode_instruction_should_parse_DRWVVB);
    RUN_TEST(test_decode_instruction_should_parse_SKPV);
    RUN_TEST(test_decode_instruction_should_parse_SKNPV);
    RUN_TEST(test_decode_instruction_should_parse_LDVDT);
    RUN_TEST(test_decode_instruction_should_parse_LDVK);
    RUN_TEST(test_decode_instruction_should_parse_LDDTV);
    RUN_TEST(test_decode_instruction_should_parse_LDSTV);
    RUN_TEST(test_decode_instruction_should_parse_ADDIV);
    RUN_TEST(test_decode_instruction_should_parse_LDFV);
    RUN_TEST(test_decode_instruction_should_parse_LDHFV);
    RUN_TEST(test_decode_instruction_should_parse_LDBV);
    RUN_TEST(test_decode_instruction_should_parse_LDIPV);
    RUN_TEST(test_decode_instruction_should_parse_LDVIP);
    RUN_TEST(test_decode_instruction_should_parse_LDRV);
    RUN_TEST(test_decode_instruction_should_parse_LDVR);
    return UNITY_END();
}