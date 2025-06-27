#include "unity.h"
#include "c8/chip8.c"
#include "c8/private/exception.h"
#include "c8/defs.h"
#include "c8/font.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FORMAT_A(a) ((a << 12) & 0xF000)
#define FORMAT_X(x) ((x << 8) & 0x0F00)
#define FORMAT_Y(y) ((y << 4) & 0x00F0)
#define FORMAT_B(b) (b & 0x000F)
#define FORMAT_KK(kk) (kk & 0x00FF)
#define FORMAT_NNN(nnn) (nnn & 0x0FFF)
#define GENERATE_RANDOMS \
	uint8_t x = rand() % 0x10; \
	uint8_t y = rand() % 0x10; \
	uint8_t kk = rand() % 0x100; \
	uint8_t b = rand() % 0x10; \
	uint8_t vx = rand() % 0x100; \
	uint8_t vy = rand() % 0x100; \
	uint16_t nnn = rand() % 0x1000;

#define BUILD_INSTRUCTION_AXYB(a, x, y, b) \
	(FORMAT_A(a) | FORMAT_X(x) | FORMAT_Y(y) | FORMAT_B(b))

#define BUILD_INSTRUCTION_AXKK(a, x, kk) \
	(FORMAT_A(a) | FORMAT_X(x) | FORMAT_KK(kk))

#define BUILD_INSTRUCTION_ANNN(a, nnn) \
	(FORMAT_A(a) | FORMAT_NNN(nnn))

#define INSERT_INSTRUCTION(pc, a) \
	c8.mem[pc] = (a >> 8) & 0xFF; \
	c8.mem[pc+1] = a & 0xFF;

#define AXKK(a, x, kk) INSERT_INSTRUCTION(pc, BUILD_INSTRUCTION_AXKK(a, x, kk))
#define ANNN(a, nnn) INSERT_INSTRUCTION(pc, BUILD_INSTRUCTION_ANNN(a, nnn))
#define AXYB(a, x, y, b) INSERT_INSTRUCTION(pc, BUILD_INSTRUCTION_AXYB(a, x, y, b))

#define RESET \
	memset(&c8, 0, sizeof(c8_t)); \
	int pc = 0x200; \
	c8.pc = 0x200; \
	GENERATE_RANDOMS;

c8_t c8;


void setUp(void) {
    srand(time(NULL));
}

void tearDown(void) {}

void test_parse_instruction_WhereInstructionIsCLS(void) {
    RESET;
    INSERT_INSTRUCTION(pc, 0x00E0);

    x %= C8_HIGH_DISPLAY_WIDTH * C8_HIGH_DISPLAY_HEIGHT;
    y %= C8_HIGH_DISPLAY_WIDTH * C8_HIGH_DISPLAY_HEIGHT;
    c8.display.p[x] = 1;
    c8.display.p[y] = 1;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(0, c8.display.p[x]);
    TEST_ASSERT_EQUAL_INT(0, c8.display.p[y]);
}

void test_parse_instruction_WhereInstructionIsRET(void) {
    RESET;
    INSERT_INSTRUCTION(pc, 0x00EE)

        c8.sp = 1;
    c8.stack[0] = nnn;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(nnn, c8.pc);
    TEST_ASSERT_EQUAL_INT(0, c8.sp);
}

void test_parse_instruction_WhereInstructionIsSCD(void) {
    RESET;
    AXYB(0, 0, 0xC, b);

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(b, c8.display.y);
}

void test_parse_instruction_WhereInstructionIsSCR(void) {
    RESET;
    INSERT_INSTRUCTION(pc, 0x00FB);

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(4, c8.display.x);
}

void test_parse_instruction_WhereInstructionIsSCL(void) {
    RESET;
    INSERT_INSTRUCTION(pc, 0x00FC);

    c8.display.x = 4;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(0, c8.display.x);
}

void test_parse_instruction_WhereInstructionIsEXIT(void) {
    RESET;
    INSERT_INSTRUCTION(pc, 0x00FD);

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(0, c8.running);
}

void test_parse_instruction_WhereInstructionIsLOW(void) {
    RESET;
    INSERT_INSTRUCTION(pc, 0x00FE);

    c8.display.mode = C8_DISPLAYMODE_HIGH;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(C8_DISPLAYMODE_LOW, c8.display.mode);
}

void test_parse_instruction_WhereInstructionIsHIGH(void) {
    RESET;
    INSERT_INSTRUCTION(pc, 0x00FF);

    c8.display.mode = C8_DISPLAYMODE_LOW;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(C8_DISPLAYMODE_HIGH, c8.display.mode);
}

void test_parse_instruction_WhereInstructionIsJPNNN(void) {
    RESET;
    ANNN(0x1, nnn);

    c8.sp = 0;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(nnn, c8.pc);
    TEST_ASSERT_EQUAL_INT(0, c8.sp);
}

void test_parse_instruction_WhereInstructionIsCALL(void) {
    RESET;
    ANNN(0x2, x);

    int old = c8.pc;
    c8.sp = 0;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(x, c8.pc);
    TEST_ASSERT_EQUAL_INT(1, c8.sp);
    TEST_ASSERT_EQUAL_INT(old, c8.stack[0]);
}

void test_parse_instruction_WhereInstructionIsCALL_WhereSPIs15(void) {
    RESET;
    ANNN(0x2, nnn);

    c8.sp = 15;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(STACK_OVERFLOW_EXCEPTION, ret);
}


void test_parse_instruction_WhereInstructionIsSEXKK_WhereVXEqualsKK(void) {
    RESET;
    AXKK(0x3, x, kk);

    c8.V[x] = kk;
    INSERT_INSTRUCTION(pc, BUILD_INSTRUCTION_AXKK(0x3, x, kk));

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(pc + 2, c8.pc);
}

void test_parse_instruction_WhereInstructionIsSEXKK_WhereVXDoesNotEqualKK(void) {
    RESET;
    AXKK(0x3, x, vx + 1);

    c8.V[x] = vx;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(pc, c8.pc);
}

void test_parse_instruction_WhereInstructionIsSNEXKK_WhereVXEqualsKK(void) {
    RESET;
    AXKK(0x4, x, kk);

    c8.V[x] = kk;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(pc, c8.pc);
}

void test_parse_instruction_WhereInstructionIsSNEXKK_WhereVXDoesNotEqualKK(void) {
    RESET;
    AXKK(0x4, x, kk);

    c8.V[x] = ((uint8_t)kk) + 1;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(pc + 2, c8.pc);
}

void test_parse_instruction_WhereInstructionIsSEXY_WhereVsAreEqual(void) {
    RESET;
    AXYB(0x5, x, y, 0);

    c8.V[x] = kk;
    c8.V[y] = kk;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(pc + 2, c8.pc);
}

void test_parse_instruction_WhereInstructionIsSEXY_WhereVsAreNotEqual(void) {
    RESET;
    AXYB(0x5, x, y, 0);

    c8.V[x] = vx;
    c8.V[y] = (vx + 1) % 0x100;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(pc, c8.pc);
}

void test_parse_instruction_WhereInstructionIsLDXKK(void) {
    RESET;
    AXKK(0x6, x, kk);

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(kk, c8.V[x]);
}

void test_parse_instruction_WhereInstructionIsADDXKK_WithCarry(void) {
    RESET;
    kk = (rand() % 127) + 128;
    AXKK(0x7, x, kk);

    vx = (rand() % 127) + 128;
    c8.V[x] = vx;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT((uint8_t)(vx + kk), c8.V[x]);
    TEST_ASSERT_EQUAL_INT(1, c8.V[0xF]);
}

void test_parse_instruction_WhereInstructionIsADDXKK_WithoutCarry(void) {
    RESET;
    kk = rand() % 127;
    AXKK(0x7, x, kk);

    vx = rand() % 127;
    c8.V[x] = vx;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(vx + kk, c8.V[x]);
    TEST_ASSERT_EQUAL_INT(0, c8.V[0xF]);
}
void test_parse_instruction_WhereInstructionIsLDXY(void) {
    RESET;
    AXYB(0x8, x, y, 0);

    c8.V[y] = kk;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(c8.V[x], c8.V[y]);
}

void test_parse_instruction_WhereInstructionIsORXY(void) {
    RESET;
    AXYB(0x8, x, y, 1);

    c8.V[x] = vx;
    c8.V[y] = vy;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(vx | vy, c8.V[x]);
}

void test_parse_instruction_WhereInstructionIsANDXY(void) {
    RESET;
    AXYB(0x8, x, y, 2);

    c8.V[x] = vx;
    c8.V[y] = vy;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(vx & vy, c8.V[x]);
}

void test_parse_instruction_WhereInstructionIsXORXY(void) {
    RESET;
    AXYB(0x8, x, y, 3);

    c8.V[x] = vx;
    c8.V[y] = vy;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(vx ^ vy, c8.V[x]);
}

void test_parse_instruction_WhereInstructionIsADDXY_WithCarry(void) {
    RESET;
    AXYB(0x8, x, y, 4);

    vx = (rand() % 127) + 128;
    vy = (rand() % 127) + 128;
    c8.V[x] = vx;
    c8.V[y] = vy;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT((vx + vy) % 0x100, c8.V[x]);
    TEST_ASSERT_EQUAL_INT(1, c8.V[0xF]);
}

void test_parse_instruction_WhereInstructionIsADDXY_WithoutCarry(void) {
    RESET;
    AXYB(0x8, x, y, 4);

    vx = rand() % 127;
    vy = rand() % 127;
    c8.V[x] = vx;
    c8.V[y] = vy;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(vx + vy, c8.V[x]);
    TEST_ASSERT_EQUAL_INT(0, c8.V[0xF]);
}

void test_parse_instruction_WhereInstructionIsSUBXY_WithBorrow(void) {
    RESET;
    AXYB(0x8, x, y, 5);

    vx = rand() % 128;
    vy = (rand() % 128) + 128;
    c8.V[x] = vx;
    c8.V[y] = vy;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT((uint8_t)(vx - vy), c8.V[x]);
    TEST_ASSERT_EQUAL_INT(0, c8.V[0xF]);
}

void test_parse_instruction_WhereInstructionIsSUBXY_WithoutBorrow(void) {
    RESET;
    AXYB(0x8, x, y, 5);

    vx = (rand() % 128) + 128;
    vy = rand() % 128;
    c8.V[x] = vx;
    c8.V[y] = vy;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(vx - vy, c8.V[x]);
    TEST_ASSERT_EQUAL_INT(1, c8.V[0xF]);
}

void test_parse_instruction_WhereInstructionIsSHRXY_WithFlag(void) {
    RESET;
    AXYB(0x8, x, y, 6);

    vy |= 0x2; // b10
    c8.V[y] = vy;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT((vy >> 1), c8.V[x]);
    TEST_ASSERT_EQUAL_INT(1, c8.V[0xF]);
}

void test_parse_instruction_WhereInstructionIsSHRXY_WithoutFlag(void) {
    RESET;
    AXYB(0x8, x, y, 6);

    vy &= 0xFD; // b11111101
    c8.V[y] = vy;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT((vy >> 1), c8.V[x]);
    TEST_ASSERT_EQUAL_INT(0, c8.V[0xF]);
}

void test_parse_instruction_WhereInstructionIsSUBNXY_WithBorrow(void) {
    RESET;
    AXYB(0x8, x, y, 7);

    vx = rand() % 128;
    vy = (rand() % 128) + 128;
    c8.V[x] = vx;
    c8.V[y] = vy;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT((uint8_t)(vy - vx), c8.V[x]);
    TEST_ASSERT_EQUAL_INT(1, c8.V[0xF]);
}

void test_parse_instruction_WhereInstructionIsSUBNXY_WithoutBorrow(void) {
    RESET;
    AXYB(0x8, x, y, 7);

    vx = (rand() % 128) + 128;
    vy = rand() % 128;
    c8.V[x] = vx;
    c8.V[y] = vy;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT((uint8_t)(vy - vx), c8.V[x]);
    TEST_ASSERT_EQUAL_INT(0, c8.V[0xF]);
}

void test_parse_instruction_WhereInstructionIsSHLXY_WithFlag(void) {
    RESET;
    AXYB(0x8, x, y, 0xE);

    vy |= 0x40; // b01000000
    c8.V[y] = vy;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT((uint8_t)(vy << 1), c8.V[x]);
    TEST_ASSERT_EQUAL_INT(1, c8.V[0xF]);
}

void test_parse_instruction_WhereInstructionIsSHLXY_WithoutFlag(void) {
    RESET;
    AXYB(0x8, x, y, 0xE);

    vy &= 0xBF; // b10111111
    c8.V[y] = vy;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT((uint8_t)(vy << 1), c8.V[x]);
    TEST_ASSERT_EQUAL_INT(0, c8.V[0xF]);
}

void test_parse_instruction_WhereInstructionIsSNEXY_WhereVsAreEqual(void) {
    RESET;
    AXYB(9, x, y, 0);

    c8.V[x] = vx;
    c8.V[y] = vx;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(pc, c8.pc);
}

void test_parse_instruction_WhereInstructionIsSNEXY_WhereVsAreNotEqual(void) {
    RESET;
    AXYB(9, x, y, 0);

    c8.V[x] = vx;
    c8.V[y] = vx + 1;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(pc + 2, c8.pc);
}

void test_parse_instruction_WhereInstructionIsLDINNN(void) {
    RESET;
    ANNN(0xA, nnn);

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(nnn, c8.I);
}

void test_parse_instruction_WhereInstructionIsJPV0NNN(void) {
    RESET;
    ANNN(0xB, nnn);

    c8.V[0] = kk;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(0, c8.sp);
    TEST_ASSERT_EQUAL_INT(kk + nnn, c8.pc);
}

void test_parse_instruction_WhereInstructionIsRNDXKK(void) {
    RESET;
    if (kk == 0) {
        kk++;
    }
    AXKK(0xC, x, kk);

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_LESS_OR_EQUAL_INT(kk, c8.V[x]);
}

void test_parse_instruction_WhereInstructionIsDRWXYB_InStandardMode(void) {
    RESET;
    if (b == 0) {
        b++;
    }
    AXYB(0xD, x, y, b);

    vx %= C8_LOW_DISPLAY_WIDTH;
    vy %= C8_LOW_DISPLAY_HEIGHT;

    c8.V[x] = vx;
    c8.V[y] = vy;
    c8.I = 0x300;

    for (int i = 0; i < b; i++) {
        c8.mem[c8.I + i] = 1;
    }

    c8_display_t oldDisplay = c8.display;
    int ret = parse_instruction(&c8);
    for (int i = 0; i < b; i++) {
        int toDraw = c8.mem[c8.I + i];
        int cur = c8.display.p[(vy + i) * C8_LOW_DISPLAY_WIDTH + vx];
        int old = oldDisplay.p[(vy + i) * C8_LOW_DISPLAY_WIDTH + vx];

        TEST_ASSERT_EQUAL_INT(old ^ toDraw, cur);
    }
}

void test_parse_instruction_WhereInstructionIsSKPV_WhereKeyIsPressed(void) {
    RESET;
    AXKK(0xE, x, 0x9E);

    c8.V[x] = y;
    c8.key[y] = 1;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(pc + 2, c8.pc);
}

void test_parse_instruction_WhereInstructionIsSKPV_WhereKeyIsNotPressed(void) {
    RESET;
    AXKK(0xE, x, 0x9E);

    c8.V[x] = y;
    c8.key[y] = 0;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(pc, c8.pc);
}

void test_parse_instruction_WhereInstructionIsSKNPV_WhereKeyIsPressed(void) {
    RESET;
    AXKK(0xE, x, 0xA1);

    c8.V[x] = y;
    c8.key[y] = 1;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(pc, c8.pc);
}

void test_parse_instruction_WhereInstructionIsSKNPV_WhereKeyIsNotPressed(void) {
    RESET;
    AXKK(0xE, x, 0xA1);

    c8.V[x] = y;
    c8.key[y] = 0;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(pc + 2, c8.pc);
}


void test_parse_instruction_WhereInstructionIsLDXDT(void) {
    RESET;
    AXKK(0xF, x, 0x07);

    c8.dt = y;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(y, c8.V[x]);
}

void test_parse_instruction_WhereInstructionIsLDXK(void) {
    RESET;
    AXKK(0xF, x, 0x0A);

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(x, c8.VK);
    TEST_ASSERT_EQUAL_INT(1, c8.waitingForKey);
}

void test_parse_instruction_WhereInstructionIsLDDTX(void) {
    RESET;
    AXKK(0xF, x, 0x15);

    if (y < 2) {
        y += 2;
    }
    c8.V[x] = y;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(y, c8.dt);
}

void test_parse_instruction_WhereInstructionIsLDSTX(void) {
    RESET;
    AXKK(0xF, x, 0x18);

    if (y < 2) {
        y += 2;
    }
    c8.V[x] = y;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(y, c8.st);
}

void test_parse_instruction_WhereInstructionIsADDIX(void) {
    RESET;
    AXKK(0xF, x, 0x1E);

    c8.V[x] = y;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(y, c8.I);
}

void test_parse_instruction_WhereInstructionIsLDFX(void) {
    RESET;
    AXKK(0xF, x, 0x29);

    c8.V[x] = y;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(C8_FONT_START + (y * 5), c8.I);
}

void test_parse_instruction_WhereInstructionIsLDHFX(void) {
    RESET;
    AXKK(0xF, x, 0x30);

    c8.V[x] = y;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(C8_HIGH_FONT_START + (y * 10), c8.I);
}

void test_parse_instruction_WhereInstructionIsLDBX(void) {
    RESET;
    AXKK(0xF, x, 0x33);

    c8.V[x] = kk;

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    TEST_ASSERT_EQUAL_INT(kk / 100, c8.mem[c8.I]);
    TEST_ASSERT_EQUAL_INT((kk / 10) % 10, c8.mem[c8.I + 1]);
    TEST_ASSERT_EQUAL_INT(kk % 10, c8.mem[c8.I + 2]);
}

void test_parse_instruction_WhereInstructionIsLDIPX(void) {
    RESET;
    AXKK(0xF, x, 0x55);

    c8.I = 0x300;
    for (int i = 0; i < x; i++) {
        c8.V[i] = (uint8_t)rand();
    }

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    for (int i = 0; i < x; i++) {
        TEST_ASSERT_EQUAL_INT(c8.mem[c8.I + i], c8.V[i]);
    }
}

void test_parse_instruction_WhereInstructionIsLDXIP(void) {
    RESET;
    AXKK(0xF, x, 0x65);

    c8.I = 0x300;
    for (int i = 0; i < x; i++) {
        c8.mem[c8.I + i] = (uint8_t)rand();
    }

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    for (int i = 0; i < x; i++) {
        TEST_ASSERT_EQUAL_INT(c8.V[i], c8.mem[c8.I + i]);
    }
}

void test_parse_instruction_WhereInstructionIsLDRX(void) {
    RESET;
    AXKK(0xF, x, 0x75);

    for (int i = 0; i < x; i++) {
        c8.V[i] = 0x30;
    }

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    for (int i = 0; i < x; i++) {
        TEST_ASSERT_EQUAL_INT(c8.R[i], c8.V[i]);
    }
}

void test_parse_instruction_WhereInstructionIsLDXR(void) {
    RESET;
    AXKK(0xF, x, 0x85);

    for (int i = 0; i < x; i++) {
        c8.R[i] = rand() % 0x100;
    }

    int ret = parse_instruction(&c8);
    TEST_ASSERT_EQUAL_INT(2, ret);
    for (int i = 0; i < x; i++) {
        TEST_ASSERT_EQUAL_INT(c8.V[i], c8.R[i]);
    }
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
    RUN_TEST(test_parse_instruction_WhereInstructionIsSEXKK_WhereVXEqualsKK);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSEXKK_WhereVXDoesNotEqualKK);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSNEXKK_WhereVXEqualsKK);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSNEXKK_WhereVXDoesNotEqualKK);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSEXY_WhereVsAreEqual);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSEXY_WhereVsAreNotEqual);
    RUN_TEST(test_parse_instruction_WhereInstructionIsLDXKK);
    RUN_TEST(test_parse_instruction_WhereInstructionIsADDXKK_WithCarry);
    RUN_TEST(test_parse_instruction_WhereInstructionIsADDXKK_WithoutCarry);
    RUN_TEST(test_parse_instruction_WhereInstructionIsLDXY);
    RUN_TEST(test_parse_instruction_WhereInstructionIsORXY);
    RUN_TEST(test_parse_instruction_WhereInstructionIsANDXY);
    RUN_TEST(test_parse_instruction_WhereInstructionIsXORXY);
    RUN_TEST(test_parse_instruction_WhereInstructionIsADDXY_WithCarry);
    RUN_TEST(test_parse_instruction_WhereInstructionIsADDXY_WithoutCarry);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSUBXY_WithBorrow);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSUBXY_WithoutBorrow);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSHRXY_WithFlag);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSHRXY_WithoutFlag);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSUBNXY_WithBorrow);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSUBNXY_WithoutBorrow);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSHLXY_WithFlag);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSHLXY_WithoutFlag);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSNEXY_WhereVsAreEqual);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSNEXY_WhereVsAreNotEqual);
    RUN_TEST(test_parse_instruction_WhereInstructionIsLDINNN);
    RUN_TEST(test_parse_instruction_WhereInstructionIsJPV0NNN);
    RUN_TEST(test_parse_instruction_WhereInstructionIsRNDXKK);
    RUN_TEST(test_parse_instruction_WhereInstructionIsDRWXYB_InStandardMode);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSKPV_WhereKeyIsPressed);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSKPV_WhereKeyIsNotPressed);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSKNPV_WhereKeyIsPressed);
    RUN_TEST(test_parse_instruction_WhereInstructionIsSKNPV_WhereKeyIsNotPressed);
    RUN_TEST(test_parse_instruction_WhereInstructionIsLDXDT);
    RUN_TEST(test_parse_instruction_WhereInstructionIsLDXK);
    RUN_TEST(test_parse_instruction_WhereInstructionIsLDDTX);
    RUN_TEST(test_parse_instruction_WhereInstructionIsLDSTX);
    RUN_TEST(test_parse_instruction_WhereInstructionIsADDIX);
    RUN_TEST(test_parse_instruction_WhereInstructionIsLDFX);
    RUN_TEST(test_parse_instruction_WhereInstructionIsLDHFX);
    RUN_TEST(test_parse_instruction_WhereInstructionIsLDBX);
    RUN_TEST(test_parse_instruction_WhereInstructionIsLDIPX);
    RUN_TEST(test_parse_instruction_WhereInstructionIsLDXIP);
    RUN_TEST(test_parse_instruction_WhereInstructionIsLDRX);
    RUN_TEST(test_parse_instruction_WhereInstructionIsLDXR);
    return UNITY_END();
}
