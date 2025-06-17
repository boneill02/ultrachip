#include "unity.h"
#include "util/decode.c"
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

void test_decode_instruction_WhereInstructionIsCLS(void) {
	test_decode_instruction_should_parse("CLS", 0x00E0);
}

void test_decode_instruction_WhereInstructionIsRET(void) {
	test_decode_instruction_should_parse("RET", 0x00EE);
}

void test_decode_instruction_WhereInstructionIsSCD(void) {
	GENERATE_RANDOMS;
	uint16_t ins = 0x00C0 | b;

	sprintf(buf, "SCD %d", b);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsSCR(void) {
	test_decode_instruction_should_parse("SCR", 0x00FB);
}

void test_decode_instruction_WhereInstructionIsSCL(void) {
	test_decode_instruction_should_parse("SCL", 0x00FC);
}

void test_decode_instruction_WhereInstructionIsEXIT(void) {
	test_decode_instruction_should_parse("EXIT", 0x00FD);
}

void test_decode_instruction_WhereInstructionIsLOW(void) {
	test_decode_instruction_should_parse("LOW", 0x00FE);
}

void test_decode_instruction_WhereInstructionIsHIGH(void) {
	test_decode_instruction_should_parse("HIGH", 0x00FF);
}

void test_decode_instruction_WhereInstructionIsJP(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_ANNN(1, nnn);
	label_map[nnn] = 0;

	sprintf(buf, "JP $%03x", nnn);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsJP_WithLabel(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_ANNN(1, nnn);
	label_map[nnn] = label;

	sprintf(buf, "JP label%d", label);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsCALL(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_ANNN(2, nnn);
	label_map[nnn] = 0;

	sprintf(buf, "CALL $%03x", nnn);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsCALL_WithLabel(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_ANNN(2, nnn);
	label_map[nnn] = label;

	sprintf(buf, "CALL label%d", label);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsSEVKK(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_AXKK(3, x, kk);

	sprintf(buf, "SE V%01x, $%02x", x, kk);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsSNEVKK(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_AXKK(4, x, kk);

	sprintf(buf, "SNE V%01x, $%02x", x, kk);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsSEVV(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_AXYB(5, x, y, 0);

	sprintf(buf, "SE V%01x, V%01x", x, y);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsLDVKK(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_AXKK(6, x, kk);

	sprintf(buf, "LD V%01x, $%02x", x, kk);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsADDVKK(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_AXKK(7, x, kk);

	sprintf(buf, "ADD V%01x, $%02x", x, kk);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsLDVV(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 0);

	sprintf(buf, "LD V%01x, V%01x", x, y);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsORVV(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 1);

	sprintf(buf, "OR V%01x, V%01x", x, y);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsANDVV(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 2);

	sprintf(buf, "AND V%01x, V%01x", x, y);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsXORVV(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 3);

	sprintf(buf, "XOR V%01x, V%01x", x, y);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsADDVV(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 4);

	sprintf(buf, "ADD V%01x, V%01x", x, y);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsSUBVV(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 5);

	sprintf(buf, "SUB V%01x, V%01x", x, y);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsSHRV(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, 0, 6);

	sprintf(buf, "SHR V%01x", x);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsSUBNVV(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 7);

	sprintf(buf, "SUBN V%01x, V%01x", x, y);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsSHLV(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_AXYB(8, x, 0, 0xE);

	sprintf(buf, "SHL V%01x", x);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsSNEVV(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_AXYB(9, x, y, 0);

	sprintf(buf, "SNE V%01x, V%01x", x, y);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsLDINNN(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_ANNN(0xA, nnn);
	label_map[nnn] = 0;

	sprintf(buf, "LD I, $%03x", nnn);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsLDINNN_WithLabel(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_ANNN(0xA, nnn);
	label_map[nnn] = label;

	sprintf(buf, "LD I, label%d", label);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsJPV0NNN(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_ANNN(0xB, nnn);
	label_map[nnn] = 0;

	sprintf(buf, "JP V0, $%03x", nnn);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsJPV0NNN_WithLabel(void) {
	GENERATE_RANDOMS;
	int ins = BUILD_INSTRUCTION_ANNN(0xB, nnn);
	label_map[nnn] = label;

	sprintf(buf, "JP V0, label%d", label);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsRNDVKK(void) {
	GENERATE_RANDOMS;
	uint16_t ins = BUILD_INSTRUCTION_AXKK(0xC, x, kk);

	sprintf(buf, "RND V%01x, $%02x", x, kk);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsDRWVVB(void) {
	GENERATE_RANDOMS;
	uint16_t ins = BUILD_INSTRUCTION_AXYB(0xD, x, y, b);

	sprintf(buf, "DRW V%01x, V%01x, $%01x", x, y, b);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsSKPV(void) {
	GENERATE_RANDOMS;
	uint16_t ins = BUILD_INSTRUCTION_AXKK(0xE, x, 0x9E);

	sprintf(buf, "SKP V%01x", x);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsSKNPV(void) {
	GENERATE_RANDOMS;
	uint16_t ins = BUILD_INSTRUCTION_AXKK(0xE, x, 0xA1);

	sprintf(buf, "SKNP V%01x", x);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsLDVDT(void) {
	GENERATE_RANDOMS;
	uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x07);

	sprintf(buf, "LD V%01x, DT", x);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsLDVK(void) {
	GENERATE_RANDOMS;
	uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x0A);

	sprintf(buf, "LD V%01x, K", x);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsLDDTV(void) {
	GENERATE_RANDOMS;
	uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x15);

	sprintf(buf, "LD DT, V%01x", x);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsLDSTV(void) {
	GENERATE_RANDOMS;
	uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x18);

	sprintf(buf, "LD ST, V%01x", x);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsADDIV(void) {
	GENERATE_RANDOMS;
	uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x1E);

	sprintf(buf, "ADD I, V%01x", x);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsLDFV(void) {
	GENERATE_RANDOMS;
	uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x29);

	sprintf(buf, "LD F, V%01x", x);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsLDHFV(void) {
	GENERATE_RANDOMS;
	uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x30);

	sprintf(buf, "LD HF, V%01x", x);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsLDBV(void) {
	GENERATE_RANDOMS;
	uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x33);

	sprintf(buf, "LD B, V%01x", x);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsLDIPV(void) {
	GENERATE_RANDOMS;
	uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x55);

	sprintf(buf, "LD [I], V%01x", x);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsLDVIP(void) {
	GENERATE_RANDOMS;
	uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x65);

	sprintf(buf, "LD V%01x, [I]", x);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsLDRV(void) {
	GENERATE_RANDOMS;
	uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x75);

	sprintf(buf, "LD R, V%01x", x);
	test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsLDVR(void) {
	GENERATE_RANDOMS;
	uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x85);

	sprintf(buf, "LD V%01x, R", x);
	test_decode_instruction_should_parse(buf, ins);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_decode_instruction_WhereInstructionIsCLS);
    RUN_TEST(test_decode_instruction_WhereInstructionIsRET);
    RUN_TEST(test_decode_instruction_WhereInstructionIsSCD);
    RUN_TEST(test_decode_instruction_WhereInstructionIsSCR);
    RUN_TEST(test_decode_instruction_WhereInstructionIsSCL);
    RUN_TEST(test_decode_instruction_WhereInstructionIsEXIT);
    RUN_TEST(test_decode_instruction_WhereInstructionIsLOW);
    RUN_TEST(test_decode_instruction_WhereInstructionIsHIGH);
    RUN_TEST(test_decode_instruction_WhereInstructionIsJP);
    RUN_TEST(test_decode_instruction_WhereInstructionIsJP_WithLabel);
    RUN_TEST(test_decode_instruction_WhereInstructionIsCALL);
    RUN_TEST(test_decode_instruction_WhereInstructionIsCALL_WithLabel);
    RUN_TEST(test_decode_instruction_WhereInstructionIsSEVKK);
    RUN_TEST(test_decode_instruction_WhereInstructionIsSNEVKK);
    RUN_TEST(test_decode_instruction_WhereInstructionIsSEVV);
    RUN_TEST(test_decode_instruction_WhereInstructionIsLDVKK);
    RUN_TEST(test_decode_instruction_WhereInstructionIsADDVKK);
    RUN_TEST(test_decode_instruction_WhereInstructionIsLDVV);
    RUN_TEST(test_decode_instruction_WhereInstructionIsORVV);
    RUN_TEST(test_decode_instruction_WhereInstructionIsANDVV);
    RUN_TEST(test_decode_instruction_WhereInstructionIsXORVV);
    RUN_TEST(test_decode_instruction_WhereInstructionIsADDVV);
    RUN_TEST(test_decode_instruction_WhereInstructionIsSUBVV);
    RUN_TEST(test_decode_instruction_WhereInstructionIsSHRV);
    RUN_TEST(test_decode_instruction_WhereInstructionIsSUBNVV);
    RUN_TEST(test_decode_instruction_WhereInstructionIsSHLV);
    RUN_TEST(test_decode_instruction_WhereInstructionIsSNEVV);
    RUN_TEST(test_decode_instruction_WhereInstructionIsLDINNN);
    RUN_TEST(test_decode_instruction_WhereInstructionIsLDINNN_WithLabel);
    RUN_TEST(test_decode_instruction_WhereInstructionIsJPV0NNN);
    RUN_TEST(test_decode_instruction_WhereInstructionIsJPV0NNN_WithLabel);
    RUN_TEST(test_decode_instruction_WhereInstructionIsRNDVKK);
    RUN_TEST(test_decode_instruction_WhereInstructionIsDRWVVB);
    RUN_TEST(test_decode_instruction_WhereInstructionIsSKPV);
    RUN_TEST(test_decode_instruction_WhereInstructionIsSKNPV);
    RUN_TEST(test_decode_instruction_WhereInstructionIsLDVDT);
    RUN_TEST(test_decode_instruction_WhereInstructionIsLDVK);
    RUN_TEST(test_decode_instruction_WhereInstructionIsLDDTV);
    RUN_TEST(test_decode_instruction_WhereInstructionIsLDSTV);
    RUN_TEST(test_decode_instruction_WhereInstructionIsADDIV);
    RUN_TEST(test_decode_instruction_WhereInstructionIsLDFV);
    RUN_TEST(test_decode_instruction_WhereInstructionIsLDHFV);
    RUN_TEST(test_decode_instruction_WhereInstructionIsLDBV);
    RUN_TEST(test_decode_instruction_WhereInstructionIsLDIPV);
    RUN_TEST(test_decode_instruction_WhereInstructionIsLDVIP);
    RUN_TEST(test_decode_instruction_WhereInstructionIsLDRV);
    RUN_TEST(test_decode_instruction_WhereInstructionIsLDVR);
    return UNITY_END();
}
