#include "unity.h"
#include "util/decode.c"
#include "util/util.c"
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
	uint8_t x = rand() % 0xF; \
	uint8_t y = rand() % 0xF; \
	uint8_t kk = rand() % 0xFF; \
	uint8_t b = rand() % 0xF; \
	uint8_t vx = rand() % 0xFF; \
	uint8_t vy = rand() % 0xFF; \
	uint16_t nnn = rand() % 0xFFF;

#define BUILD_INSTRUCTION_AXYB(a, x, y, b) \
	(FORMAT_A(a) | FORMAT_X(x) | FORMAT_Y(y) | FORMAT_B(b))

#define BUILD_INSTRUCTION_AXKK(a, x, kk) \
	(FORMAT_A(a) | FORMAT_X(x) | FORMAT_KK(kk))

#define BUILD_INSTRUCTION_ANNN(a, nnn) \
	(FORMAT_A(a) | FORMAT_NNN(nnn))

#define INSERT_INSTRUCTION(pc, a) \
	c8.mem[pc] = (a >> 8) & 0xFF; \
	c8.mem[pc+1] = a & 0xFF;

#define RESET \
	memset(&c8, 0, sizeof(chip8_t)); \
	int pc = 0x200; \
	c8.pc = 0x200;

chip8_t c8;

int tick(int *a, int b) {}
void render(display_t *a , int *b) {}
void deinit_graphics(void) { }
int init_graphics(void) { }
int *get_pixel(display_t *display, int x, int y) {
	if (display->mode == DISPLAY_EXTENDED) {
		x += display->x;
		y += display->y;
	}
    return &display->p[y * STANDARD_DISPLAY_WIDTH + x];
}

int debug_repl(chip8_t *c8) { }
int has_breakpoint(chip8_t *c8, uint16_t pc) { }

void setUp(void) {
	srand(time(NULL));
}

void tearDown(void) { }


void test_parse_instruction_WhereInstructionIsCLS(void) {
	RESET;

	c8.display.p[0] = 1;
	c8.display.p[10] = 1;
	INSERT_INSTRUCTION(pc, 0x00E0);

	int ret = parse_instruction(&c8);

	TEST_ASSERT_EQUAL_INT(1, ret);
	TEST_ASSERT_EQUAL_INT(0, c8.display.p[0]);
	TEST_ASSERT_EQUAL_INT(0, c8.display.p[10]);
}

void test_parse_instruction_WhereInstructionIsRET(void) {
	RESET;

	c8.sp = 1;
	c8.pc = 0x206;
	c8.stack[0] = 0x200;
	INSERT_INSTRUCTION(c8.pc, 0x00EE)

	int ret = parse_instruction(&c8);

	TEST_ASSERT_EQUAL_INT(1, ret);
	TEST_ASSERT_EQUAL_INT(0x200, c8.pc);
	TEST_ASSERT_EQUAL_INT(0, c8.sp);
}

void test_parse_instruction_WhereInstructionIsSCD(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsSCR(void) {
	RESET;
	GENERATE_RANDOMS;
	INSERT_INSTRUCTION(pc, 0x00FB);

	int ret = parse_instruction(&c8);
	TEST_ASSERT_EQUAL_INT(1, ret);
	TEST_ASSERT_EQUAL_INT(4, c8.display.x);
}

void test_parse_instruction_WhereInstructionIsSCL(void) {
	RESET;
	GENERATE_RANDOMS;
	INSERT_INSTRUCTION(pc, 0x00FC);

	c8.display.x = 4;
	int ret = parse_instruction(&c8);
	TEST_ASSERT_EQUAL_INT(1, ret);
	TEST_ASSERT_EQUAL_INT(0, c8.display.x);
}

void test_parse_instruction_WhereInstructionIsEXIT(void) {
	RESET;
	GENERATE_RANDOMS;
	INSERT_INSTRUCTION(pc, 0x00FD);

	int ret = parse_instruction(&c8);
	TEST_ASSERT_EQUAL_INT(1, ret);
	TEST_ASSERT_EQUAL_INT(0, c8.running);
}

void test_parse_instruction_WhereInstructionIsLOW(void) {
	RESET;
	GENERATE_RANDOMS;
	INSERT_INSTRUCTION(pc, 0x00FE);

	c8.display.mode = DISPLAY_EXTENDED;
	int ret = parse_instruction(&c8);
	TEST_ASSERT_EQUAL_INT(1, ret);
	TEST_ASSERT_EQUAL_INT(DISPLAY_STANDARD, c8.display.mode);
}

void test_parse_instruction_WhereInstructionIsHIGH(void) {
	RESET;
	GENERATE_RANDOMS;
	INSERT_INSTRUCTION(pc, 0x00FF);

	c8.display.mode = DISPLAY_STANDARD;
	int ret = parse_instruction(&c8);
	TEST_ASSERT_EQUAL_INT(1, ret);
	TEST_ASSERT_EQUAL_INT(DISPLAY_EXTENDED, c8.display.mode);
}

void test_parse_instruction_WhereInstructionIsJPNNN(void) {
	RESET;

	uint16_t new = 0x210;
	c8.sp = 0;
	INSERT_INSTRUCTION(pc, BUILD_INSTRUCTION_ANNN(0x1, new));

	int ret = parse_instruction(&c8);

	TEST_ASSERT_EQUAL_INT(1, ret);
	TEST_ASSERT_EQUAL_INT(0x210, c8.pc);
	TEST_ASSERT_EQUAL_INT(0, c8.sp);
}

void test_parse_instruction_WhereInstructionIsCALL(void) {
	RESET;

	uint16_t old = pc;
	uint16_t new = 0x210;
	c8.sp = 0;

	INSERT_INSTRUCTION(pc, BUILD_INSTRUCTION_ANNN(0x2, new));

	int ret = parse_instruction(&c8);

	TEST_ASSERT_EQUAL_INT(1, ret);
	TEST_ASSERT_EQUAL_INT(new, c8.pc);
	TEST_ASSERT_EQUAL_INT(1, c8.sp);
	TEST_ASSERT_EQUAL_INT(old, c8.stack[0]);
}

void test_parse_instruction_WhereInstructionIsCALL_WhereSPIs15(void) {
	RESET;

	uint16_t new = 0x210;
	c8.sp = 15;
	INSERT_INSTRUCTION(pc, BUILD_INSTRUCTION_ANNN(0x2, new));

	int ret = parse_instruction(&c8);

	TEST_ASSERT_EQUAL_INT(STACK_OVERFLOW_EXCEPTION, ret);
}


void test_parse_instruction_WhereInstructionIsSEVKK_WhereVXEqualsKK(void) {
	RESET;
	GENERATE_RANDOMS;

	c8.V[x] = kk;
	INSERT_INSTRUCTION(pc, BUILD_INSTRUCTION_AXKK(0x3, x, kk));

	int ret = parse_instruction(&c8);

	TEST_ASSERT_EQUAL_INT(1, ret);
	TEST_ASSERT_EQUAL_INT(pc + 2, c8.pc);
}

void test_parse_instruction_WhereInstructionIsSEVKK_WhereVXDoesNotEqualKK(void) {
	RESET;
	GENERATE_RANDOMS;

	c8.V[x] = vx;
	INSERT_INSTRUCTION(pc, BUILD_INSTRUCTION_AXKK(0x3, x, vx + 1));

	int ret = parse_instruction(&c8);

	TEST_ASSERT_EQUAL_INT(1, ret);
	TEST_ASSERT_EQUAL_INT(pc, c8.pc);
}

void test_parse_instruction_WhereInstructionIsSNEVKK_WhereVXEqualsKK(void) {
	RESET;
	GENERATE_RANDOMS;

	c8.V[x] = vx;
	INSERT_INSTRUCTION(pc, BUILD_INSTRUCTION_AXKK(0x4, x, vx));

	int ret = parse_instruction(&c8);

	TEST_ASSERT_EQUAL_INT(1, ret);
	TEST_ASSERT_EQUAL_INT(pc, c8.pc);
}

void test_parse_instruction_WhereInstructionIsSNEVKK_WhereVXDoesNotEqualKK(void) {
	RESET;
	GENERATE_RANDOMS;

	c8.V[x] = ((uint8_t) kk) + 1;
	INSERT_INSTRUCTION(pc, BUILD_INSTRUCTION_AXKK(0x4, x, kk));

	int ret = parse_instruction(&c8);

	TEST_ASSERT_EQUAL_INT(1, ret);
	TEST_ASSERT_EQUAL_INT(pc + 2, c8.pc);
}

void test_parse_instruction_WhereInstructionIsSEVV_WhereVsAreEqual(void) {
	RESET;
	GENERATE_RANDOMS;

	c8.V[x] = kk;
	c8.V[y] = kk;
	INSERT_INSTRUCTION(pc, BUILD_INSTRUCTION_AXYB(0x5, x, y, 0));

	int ret = parse_instruction(&c8);

	TEST_ASSERT_EQUAL_INT(1, ret);
	TEST_ASSERT_EQUAL_INT(pc + 2, c8.pc);
}

void test_parse_instruction_WhereInstructionIsSEVV_WhereVsAreNotEqual(void) {
	RESET;
	GENERATE_RANDOMS;

	c8.V[x] = vx;
	c8.V[y] = vx + 1;
	INSERT_INSTRUCTION(pc, BUILD_INSTRUCTION_AXYB(0x5, x, y, 0));

	int ret = parse_instruction(&c8);

	TEST_ASSERT_EQUAL_INT(1, ret);
	TEST_ASSERT_EQUAL_INT(pc, c8.pc);
}

void test_parse_instruction_WhereInstructionIsLDVKK(void) {
	RESET;
	GENERATE_RANDOMS;

	INSERT_INSTRUCTION(pc, BUILD_INSTRUCTION_AXKK(0x6, x, kk));

	int ret = parse_instruction(&c8);

	TEST_ASSERT_EQUAL_INT(1, ret);
	TEST_ASSERT_EQUAL_INT(kk, c8.V[x]);
}

void test_parse_instruction_WhereInstructionIsADDVKK_WithCarry(void) {
	RESET;
	GENERATE_RANDOMS;

	vx = 254;
	c8.V[x] = vx;
	kk++;
	INSERT_INSTRUCTION(pc, BUILD_INSTRUCTION_AXKK(0x7, x, kk));

	int ret = parse_instruction(&c8);

	TEST_ASSERT_EQUAL_INT(1, ret);
	TEST_ASSERT_EQUAL_INT((vx + kk) % 256, c8.V[x]);
	TEST_ASSERT_EQUAL_INT(1, c8.V[0xF]);
}

void test_parse_instruction_WhereInstructionIsADDVKK_WithoutCarry(void) {
	RESET;
	GENERATE_RANDOMS;

	vx = 0;
	c8.V[x] = vx;
	INSERT_INSTRUCTION(pc, BUILD_INSTRUCTION_AXKK(0x7, x, kk));

	int ret = parse_instruction(&c8);

	TEST_ASSERT_EQUAL_INT(1, ret);
	TEST_ASSERT_EQUAL_INT(vx + kk, c8.V[x]);
	TEST_ASSERT_EQUAL_INT(0, c8.V[0xF]);
}
void test_parse_instruction_WhereInstructionIsLDVV(void) {
	RESET;
	GENERATE_RANDOMS;

	c8.V[y] = kk;
	INSERT_INSTRUCTION(pc, BUILD_INSTRUCTION_AXYB(0x8, x, y, 0));

	int ret = parse_instruction(&c8);

	TEST_ASSERT_EQUAL_INT(1, ret);
	TEST_ASSERT_EQUAL_INT(c8.V[x], c8.V[y]);
}

void test_parse_instruction_WhereInstructionIsORVV(void) {
	RESET;
	GENERATE_RANDOMS;

	c8.V[x] = vx;
	c8.V[y] = vy;
	INSERT_INSTRUCTION(pc, BUILD_INSTRUCTION_AXYB(0x8, x, y, 1));

	int ret = parse_instruction(&c8);

	TEST_ASSERT_EQUAL_INT(1, ret);
	TEST_ASSERT_EQUAL_INT(c8.V[x], c8.V[x] | c8.V[y]);
}

void test_parse_instruction_WhereInstructionIsANDVV(void) {
	RESET;
	GENERATE_RANDOMS;

	c8.V[x] = vx;
	c8.V[y] = vy;
	INSERT_INSTRUCTION(pc, BUILD_INSTRUCTION_AXYB(0x8, x, y, 2));

	int ret = parse_instruction(&c8);

	TEST_ASSERT_EQUAL_INT(1, ret);
	TEST_ASSERT_EQUAL_INT(c8.V[x], c8.V[x] & c8.V[y]);
}

void test_parse_instruction_WhereInstructionIsXORVV(void) {
	RESET;
	GENERATE_RANDOMS;

	c8.V[x] = vx;
	c8.V[y] = vy;
	INSERT_INSTRUCTION(pc, BUILD_INSTRUCTION_AXYB(0x8, x, y, 3));

	int ret = parse_instruction(&c8);

	TEST_ASSERT_EQUAL_INT(1, ret);
	TEST_ASSERT_EQUAL_INT(c8.V[x], c8.V[x] ^ c8.V[y]);
}

void test_parse_instruction_WhereInstructionIsADDVV_WithCarry(void) {
	RESET;
	GENERATE_RANDOMS;

	c8.V[x] = vx;
	c8.V[y] = vy;
	INSERT_INSTRUCTION(pc, BUILD_INSTRUCTION_AXYB(0x8, x, y, 4));

	int ret = parse_instruction(&c8);

	TEST_ASSERT_EQUAL_INT(1, ret);
	TEST_ASSERT_EQUAL_INT(c8.V[x], c8.V[x] ^ c8.V[y]);
}

void test_parse_instruction_WhereInstructionIsADDVV_WithoutCarry(void) {
	RESET;
	GENERATE_RANDOMS;
}

void test_parse_instruction_WhereInstructionIsSUBVV_WithBorrow(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsSUBVV_WithoutBorrow(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsSHRV(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsSUBNVV_WithBorrow(void) {
	RESET;
	// TODO
}

void test_parse_instruction_WhereInstructionIsSUBNVV_WithoutBorrow(void) {
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
    RUN_TEST(test_parse_instruction_WhereInstructionIsJPNNN);
    RUN_TEST(test_parse_instruction_WhereInstructionIsCALL);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSEVKK_WhereVXEqualsKK);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSEVKK_WhereVXDoesNotEqualKK);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSNEVKK_WhereVXEqualsKK);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSNEVKK_WhereVXDoesNotEqualKK);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSEVV_WhereVsAreEqual);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSEVV_WhereVsAreNotEqual);
    RUN_TEST(test_parse_instruction_WhereInstructionIsLDVKK);
    RUN_TEST(test_parse_instruction_WhereInstructionIsADDVKK_WithCarry);
    RUN_TEST(test_parse_instruction_WhereInstructionIsADDVKK_WithoutCarry);
    RUN_TEST(test_parse_instruction_WhereInstructionIsLDVV);
    RUN_TEST(test_parse_instruction_WhereInstructionIsORVV);
    RUN_TEST(test_parse_instruction_WhereInstructionIsANDVV);
    RUN_TEST(test_parse_instruction_WhereInstructionIsXORVV);
    RUN_TEST(test_parse_instruction_WhereInstructionIsADDVV_WithCarry);
    RUN_TEST(test_parse_instruction_WhereInstructionIsADDVV_WithoutCarry);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSUBVV_WithBorrow);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSUBVV_WithoutBorrow);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSHRV);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSUBNVV_WithBorrow);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSUBNVV_WithoutBorrow);
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
