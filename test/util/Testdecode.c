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

void test_decode_instruction_should_parse(char *expected, uint16_t ins) {
	TEST_ASSERT_EQUAL_STRING(expected, decode_instruction(ins, label_map));
}

void test_decode_instruction_should_parseCLS(void) {
	test_decode_instruction_should_parse("CLS", 0x00E0);
}

void test_decode_instruction_should_parseRET(void) {
	test_decode_instruction_should_parse("RET", 0x00EE);
}

void test_decode_instruction_should_parseSCD(void) {
	uint8_t b = 0x5;
	uint16_t ins = 0x00C0 | b;
	test_decode_instruction_should_parse("SCD 5", ins);
}

void test_decode_instruction_should_parseSCR(void) {
	test_decode_instruction_should_parse("SCR", 0x00FB);
}

void test_decode_instruction_should_parseSCL(void) {
	test_decode_instruction_should_parse("SCL", 0x00FC);
}

void test_decode_instruction_should_parseEXIT(void) {
	test_decode_instruction_should_parse("EXIT", 0x00FD);
}

void test_decode_instruction_should_parseLOW(void) {
	test_decode_instruction_should_parse("LOW", 0x00FE);
}

void test_decode_instruction_should_parseHIGH(void) {
	test_decode_instruction_should_parse("HIGH", 0x00FF);
}

void test_decode_instruction_should_parseJP(void) {
	int nnn = 0x432;
	int ins = BUILD_INSTRUCTION_ANNN(1, nnn);

	label_map[nnn] = 0;
	test_decode_instruction_should_parse("JP $432", ins);
}

void test_decode_instruction_should_parseJPWithLabel(void) {
	int nnn = 0x432;
	int ins = BUILD_INSTRUCTION_ANNN(1, nnn);

	label_map[nnn] = 1;
	test_decode_instruction_should_parse("JP label1", ins);
}

void test_decode_instruction_should_parseCALL(void) {
	int nnn = 0x500;
	int ins = BUILD_INSTRUCTION_ANNN(2, nnn);

	label_map[nnn] = 0;
	test_decode_instruction_should_parse("CALL $500", ins);
}

void test_decode_instruction_should_parseCALLWithLabel(void) {
	int nnn = 0x500;
	int ins = BUILD_INSTRUCTION_ANNN(2, nnn);

	label_map[nnn] = 1;
	test_decode_instruction_should_parse("CALL label1", ins);
}

void test_decode_instruction_should_parseSEVKK(void) {
	int x = 0x5;
	int kk = 0x85;
	int ins = BUILD_INSTRUCTION_AXKK(3, x, kk);

	test_decode_instruction_should_parse("SE V5, $85", ins);
}

void test_decode_instruction_should_parseSNEVKK(void) {
	int x = 0x5;
	int kk = 0x85;
	int ins = BUILD_INSTRUCTION_AXKK(4, x, kk);

	test_decode_instruction_should_parse("SNE V5, $85", ins);
}

void test_decode_instruction_should_parseSEVV(void) {
	int x = 0x5;
	int y = 0x8;
	int ins = BUILD_INSTRUCTION_AXYB(5, x, y, 0);

	test_decode_instruction_should_parse("SE V5, V8", ins);
}

void test_decode_instruction_should_parseLDVKK(void) {
	int x = 0x5;
	int kk = 0x10;
	int ins = BUILD_INSTRUCTION_AXKK(6, x, kk);

	test_decode_instruction_should_parse("LD V5, $10", ins);
}

void test_decode_instruction_should_parseADDVKK(void) {
	int x = 0x1;
	int kk = 0x9F;
	int ins = BUILD_INSTRUCTION_AXKK(7, x, kk);

	test_decode_instruction_should_parse("ADD V1, $9f", ins);
}

void test_decode_instruction_should_parseLDVV(void) {
	int x = 0x0;
	int y = 0x5;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 0);

	test_decode_instruction_should_parse("LD V0, V5", ins);
}

void test_decode_instruction_should_parseORVV(void) {
	int x = 0x0;
	int y = 0x5;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 1);

	test_decode_instruction_should_parse("OR V0, V5", ins);
}

void test_decode_instruction_should_parseANDVV(void) {
	int x = 0x0;
	int y = 0x5;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 2);

	test_decode_instruction_should_parse("AND V0, V5", ins);
}

void test_decode_instruction_should_parseXORVV(void) {
	int x = 0x0;
	int y = 0x5;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 3);

	test_decode_instruction_should_parse("XOR V0, V5", ins);
}

void test_decode_instruction_should_parseADDVV(void) {
	int x = 0x0;
	int y = 0x5;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 4);

	test_decode_instruction_should_parse("ADD V0, V5", ins);
}

void test_decode_instruction_should_parseSUBVV(void) {
	int x = 0x0;
	int y = 0x5;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 5);

	test_decode_instruction_should_parse("SUB V0, V5", ins);
}

void test_decode_instruction_should_parseSHRV(void) {
	int x = 0x0;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, 0, 6);

	test_decode_instruction_should_parse("SHR V0", ins);
}

void test_decode_instruction_should_parseSUBNVV(void) {
	int x = 0x0;
	int y = 0x5;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 7);

	test_decode_instruction_should_parse("SUBN V0, V5", ins);
}

void test_decode_instruction_should_parseSHLV(void) {
	int x = 0x0;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, 0, 0xE);

	test_decode_instruction_should_parse("SHL V0", ins);
}

void test_decode_instruction_should_parseSNEVV(void) {
	int x = 0x0;
	int y = 0x5;
	int ins = BUILD_INSTRUCTION_AXYB(9, x, y, 0);

	test_decode_instruction_should_parse("SNE V0, V5", ins);
}

void test_decode_instruction_should_parseLDINNN(void) {
	int nnn = 0x504;
	int ins = BUILD_INSTRUCTION_ANNN(0xa, nnn);

	label_map[nnn] = 0;
	test_decode_instruction_should_parse("LD I, $504", ins);
}

void test_decode_instruction_should_parseLDINNNWithLabel(void) {
	int nnn = 0x504;
	int ins = BUILD_INSTRUCTION_ANNN(0xa, nnn);

	label_map[nnn] = 1;
	test_decode_instruction_should_parse("LD I, label1", ins);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_decode_instruction_should_parseCLS);
    RUN_TEST(test_decode_instruction_should_parseRET);
    RUN_TEST(test_decode_instruction_should_parseSCD);
    RUN_TEST(test_decode_instruction_should_parseSCR);
    RUN_TEST(test_decode_instruction_should_parseSCL);
    RUN_TEST(test_decode_instruction_should_parseEXIT);
    RUN_TEST(test_decode_instruction_should_parseLOW);
    RUN_TEST(test_decode_instruction_should_parseHIGH);
    RUN_TEST(test_decode_instruction_should_parseJP);
    RUN_TEST(test_decode_instruction_should_parseJPWithLabel);
    RUN_TEST(test_decode_instruction_should_parseCALL);
    RUN_TEST(test_decode_instruction_should_parseCALLWithLabel);
    RUN_TEST(test_decode_instruction_should_parseSEVKK);
    RUN_TEST(test_decode_instruction_should_parseSNEVKK);
    RUN_TEST(test_decode_instruction_should_parseSEVV);
    RUN_TEST(test_decode_instruction_should_parseLDVKK);
    RUN_TEST(test_decode_instruction_should_parseADDVKK);
    RUN_TEST(test_decode_instruction_should_parseLDVV);
    RUN_TEST(test_decode_instruction_should_parseORVV);
    RUN_TEST(test_decode_instruction_should_parseANDVV);
    RUN_TEST(test_decode_instruction_should_parseXORVV);
    RUN_TEST(test_decode_instruction_should_parseADDVV);
    RUN_TEST(test_decode_instruction_should_parseSUBVV);
    RUN_TEST(test_decode_instruction_should_parseSHRV);
    RUN_TEST(test_decode_instruction_should_parseSUBNVV);
    RUN_TEST(test_decode_instruction_should_parseSHLV);
    RUN_TEST(test_decode_instruction_should_parseSNEVV);
    RUN_TEST(test_decode_instruction_should_parseLDINNN);
    RUN_TEST(test_decode_instruction_should_parseLDINNNWithLabel);
    return UNITY_END();
}