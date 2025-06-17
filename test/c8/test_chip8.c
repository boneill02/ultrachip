#include "unity.h"
#include "c8dummy.c"
#include "c8/chip8.c"
#include "debug.h"
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
	int nnn = rand() % 0xFFF;

#define BUILD_INSTRUCTION_AXYB(a, x, y, b) \
	(FORMAT_A(a) | FORMAT_X(x) | FORMAT_Y(y) | FORMAT_B(b))

#define BUILD_INSTRUCTION_AXKK(a, x, kk) \
	(FORMAT_A(a) | FORMAT_X(x) | FORMAT_KK(kk))

#define BUILD_INSTRUCTION_ANNN(a, nnn) \
	(FORMAT_A(a) | FORMAT_NNN(nnn))

#define INSERT_INSTRUCTION(pc, a) \
	c8.mem[pc] = (a >> 8) & 0xFF; \
	c8.mem[pc+1] = a & 0xFF;

#define RESET for (int i = 0; i < MEMSIZE; i++) {  }

chip8_t c8;

void setUp(void) {
	srand(time(NULL));
}

void tearDown(void) { }


void test_parse_instruction_WhereInstructionIsCLS(void) {
	RESET;

	c8.display.p[0] = 1;
	c8.display.p[10] = 1;

	INSERT_INSTRUCTION(0, 0x00E0);
	parse_instruction(&c8);

	TEST_ASSERT_EQUAL_INT(0, c8.display.p[0]);
	TEST_ASSERT_EQUAL_INT(10, c8.display.p[10]);
}

void test_parse_instruction_WhereInstructionIsRET(void) {
	RESET;
	c8.sp = 1;
	c8.pc = 0x206;
	c8.stack[0] = 0x200;
	INSERT_INSTRUCTION(c8.pc, 0x00EE)

	parse_instruction(&c8);

	TEST_ASSERT_EQUAL_INT(0x200, c8.pc);
}

void test_parse_instruction_WhereInstructionIsSCD(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsSCR(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsSCL(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsEXIT(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsLOW(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsHIGH(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsJP(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsCALL(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsSEVKK(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsSNEVKK(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsSEVV(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsLDVKK(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsADDVKK(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsLDVV(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsORVV(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsANDVV(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsXORVV(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsADDVV(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsSUBVV(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsSHRV(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsSUBNVV(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsSHLV(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsSNEVV(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsLDINNN(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsJPV0NNN(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsRNDVKK(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsDRWVVB(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsSKPV(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsSKNPV(void) {
	RESET;
	// TODO
}


void test_parse_instruction_WhereInstructionIsLDVDT(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsLDVK(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsLDDTV(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsLDSTV(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsADDIV(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsLDFV(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsLDHFV(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsLDBV(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsLDIPV(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsLDVIP(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsLDRV(void) {
	RESET;
}

void test_parse_instruction_WhereInstructionIsLDVR(void) {
	RESET;
	// TODO
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_parse_instruction_WhereInstructionIsCLS);
    RUN_TEST(test_parse_instruction_WhereInstructionIsRET);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSCD);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSCR);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSCL);
    RUN_TEST(test_parse_instruction_WhereInstructionIsEXIT);
    RUN_TEST(test_parse_instruction_WhereInstructionIsLOW);
    RUN_TEST(test_parse_instruction_WhereInstructionIsHIGH);
    RUN_TEST(test_parse_instruction_WhereInstructionIsJP);
    RUN_TEST(test_parse_instruction_WhereInstructionIsCALL);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSEVKK);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSNEVKK);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSEVV);
    RUN_TEST(test_parse_instruction_WhereInstructionIsLDVKK);
    RUN_TEST(test_parse_instruction_WhereInstructionIsADDVKK);
    RUN_TEST(test_parse_instruction_WhereInstructionIsLDVV);
    RUN_TEST(test_parse_instruction_WhereInstructionIsORVV);
    RUN_TEST(test_parse_instruction_WhereInstructionIsANDVV);
    RUN_TEST(test_parse_instruction_WhereInstructionIsXORVV);
    RUN_TEST(test_parse_instruction_WhereInstructionIsADDVV);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSUBVV);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSHRV);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSUBNVV);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSHLV);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSNEVV);
    RUN_TEST(test_parse_instruction_WhereInstructionIsLDINNN);
    RUN_TEST(test_parse_instruction_WhereInstructionIsJPV0NNN);
    RUN_TEST(test_parse_instruction_WhereInstructionIsRNDVKK);
    RUN_TEST(test_parse_instruction_WhereInstructionIsDRWVVB);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSKPV);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSKNPV);
    RUN_TEST(test_parse_instruction_WhereInstructionIsLDVDT);
    RUN_TEST(test_parse_instruction_WhereInstructionIsLDVK);
    RUN_TEST(test_parse_instruction_WhereInstructionIsLDDTV);
    RUN_TEST(test_parse_instruction_WhereInstructionIsLDSTV);
    RUN_TEST(test_parse_instruction_WhereInstructionIsADDIV);
    RUN_TEST(test_parse_instruction_WhereInstructionIsLDFV);
    RUN_TEST(test_parse_instruction_WhereInstructionIsLDHFV);
    RUN_TEST(test_parse_instruction_WhereInstructionIsLDBV);
    RUN_TEST(test_parse_instruction_WhereInstructionIsLDIPV);
    RUN_TEST(test_parse_instruction_WhereInstructionIsLDVIP);
    RUN_TEST(test_parse_instruction_WhereInstructionIsLDRV);
    RUN_TEST(test_parse_instruction_WhereInstructionIsLDVR);
    return UNITY_END();
}
