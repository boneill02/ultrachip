#include "unity.h"
#include "c8/decode.c"
#include "c8/private/exception.h"
#include "c8/defs.h"

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
#define BUILD_INSTRUCTION_AXYB(a, x, y, b) \
	(FORMAT_A(a) | FORMAT_X(x) | FORMAT_Y(y) | FORMAT_B(b))

#define BUILD_INSTRUCTION_AXKK(a, x, kk) \
	(FORMAT_A(a) | FORMAT_X(x) | FORMAT_KK(kk))

#define BUILD_INSTRUCTION_ANNN(a, nnn) \
	(FORMAT_A(a) | FORMAT_NNN(nnn))

uint8_t label_map[C8_MEMSIZE];
char buf[64];

int x = 0;
int y = 0;
int kk = 0;
int b = 0;
int nnn = 0;
const int label = 1;

void setUp(void) {
    memset(label_map, 0, 4096);
	x = rand() % 0xF;
	y = rand() % 0xF;
	kk = rand() % 0xFF;
	b = rand() % 0xF;
	nnn = rand() % 0xFFF;
}

void tearDown(void) {
}

void test_decode_instruction_should_parse(char* expected, uint16_t ins) {
    TEST_ASSERT_EQUAL_STRING(expected, c8_decode_instruction(ins, label_map));
}

void test_decode_instruction_WhereInstructionIsCLS(void) {
    test_decode_instruction_should_parse("CLS", 0x00E0);
}

void test_decode_instruction_WhereInstructionIsRET(void) {
    test_decode_instruction_should_parse("RET", 0x00EE);
}

void test_decode_instruction_WhereInstructionIsSCD(void) {
    uint16_t ins = 0x00C0 | b;

    sprintf(buf, "SCD 0x%01X", b);
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

void test_decode_instruction_WhereInstructionIsJPNNN(void) {
    int ins = BUILD_INSTRUCTION_ANNN(1, nnn);
    label_map[nnn] = 0;

    sprintf(buf, "JP $%03X", nnn);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsJPNNN_WithLabel(void) {
    int ins = BUILD_INSTRUCTION_ANNN(1, nnn);
    label_map[nnn] = label;

    sprintf(buf, "JP label%d", label);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsCALL(void) {
    int ins = BUILD_INSTRUCTION_ANNN(2, nnn);
    label_map[nnn] = 0;

    sprintf(buf, "CALL $%03X", nnn);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsCALL_WithLabel(void) {
    int ins = BUILD_INSTRUCTION_ANNN(2, nnn);
    label_map[nnn] = label;

    sprintf(buf, "CALL label%d", label);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsSEXKK(void) {
    int ins = BUILD_INSTRUCTION_AXKK(3, x, kk);

    sprintf(buf, "SE V%01X, 0x%02X", x, kk);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsSNEXKK(void) {
    int ins = BUILD_INSTRUCTION_AXKK(4, x, kk);

    sprintf(buf, "SNE V%01X, 0x%02X", x, kk);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsSEXY(void) {
    int ins = BUILD_INSTRUCTION_AXYB(5, x, y, 0);

    sprintf(buf, "SE V%01X, V%01X", x, y);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsLDXKK(void) {
    int ins = BUILD_INSTRUCTION_AXKK(6, x, kk);

    sprintf(buf, "LD V%01X, 0x%02X", x, kk);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsADDXKK(void) {
    int ins = BUILD_INSTRUCTION_AXKK(7, x, kk);

    sprintf(buf, "ADD V%01X, 0x%02X", x, kk);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsLDXY(void) {
    int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 0);

    sprintf(buf, "LD V%01X, V%01X", x, y);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsORXY(void) {
    int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 1);

    sprintf(buf, "OR V%01X, V%01X", x, y);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsANDXY(void) {
    int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 2);

    sprintf(buf, "AND V%01X, V%01X", x, y);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsXORXY(void) {
    int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 3);

    sprintf(buf, "XOR V%01X, V%01X", x, y);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsADDXY(void) {
    int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 4);

    sprintf(buf, "ADD V%01X, V%01X", x, y);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsSUBXY(void) {
    int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 5);

    sprintf(buf, "SUB V%01X, V%01X", x, y);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsSHRXY(void) {
    int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 6);

    sprintf(buf, "SHR V%01X, V%01X", x, y);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsSUBNXY(void) {
    int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 7);

    sprintf(buf, "SUBN V%01X, V%01X", x, y);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsSHLXY(void) {
    int ins = BUILD_INSTRUCTION_AXYB(8, x, y, 0xE);

    sprintf(buf, "SHL V%01X, V%01X", x, y);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsSNEXY(void) {
    int ins = BUILD_INSTRUCTION_AXYB(9, x, y, 0);

    sprintf(buf, "SNE V%01X, V%01X", x, y);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsLDINNN(void) {
    int ins = BUILD_INSTRUCTION_ANNN(0xA, nnn);
    label_map[nnn] = 0;

    sprintf(buf, "LD I, $%03X", nnn);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsLDINNN_WithLabel(void) {
    int ins = BUILD_INSTRUCTION_ANNN(0xA, nnn);
    label_map[nnn] = label;

    sprintf(buf, "LD I, label%d", label);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsJPV0NNN(void) {
    int ins = BUILD_INSTRUCTION_ANNN(0xB, nnn);
    label_map[nnn] = 0;

    sprintf(buf, "JP V0, $%03X", nnn);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsJPV0NNN_WithLabel(void) {
    int ins = BUILD_INSTRUCTION_ANNN(0xB, nnn);
    label_map[nnn] = label;

    sprintf(buf, "JP V0, label%d", label);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsRNDXKK(void) {
    uint16_t ins = BUILD_INSTRUCTION_AXKK(0xC, x, kk);

    sprintf(buf, "RND V%01X, 0x%02X", x, kk);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsDRWXYB(void) {
    uint16_t ins = BUILD_INSTRUCTION_AXYB(0xD, x, y, b);

    sprintf(buf, "DRW V%01X, V%01X, 0x%01X", x, y, b);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsSKPX(void) {
    uint16_t ins = BUILD_INSTRUCTION_AXKK(0xE, x, 0x9E);

    sprintf(buf, "SKP V%01X", x);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsSKNPX(void) {
    uint16_t ins = BUILD_INSTRUCTION_AXKK(0xE, x, 0xA1);

    sprintf(buf, "SKNP V%01X", x);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsLDXDT(void) {
    uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x07);

    sprintf(buf, "LD V%01X, DT", x);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsLDXK(void) {
    uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x0A);

    sprintf(buf, "LD V%01X, K", x);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsLDDTX(void) {
    uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x15);

    sprintf(buf, "LD DT, V%01X", x);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsLDSTX(void) {
    uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x18);

    sprintf(buf, "LD ST, V%01X", x);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsADDIX(void) {
    uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x1E);

    sprintf(buf, "ADD I, V%01X", x);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsLDFX(void) {
    uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x29);

    sprintf(buf, "LD F, V%01X", x);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsLDHFX(void) {
    uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x30);

    sprintf(buf, "LD HF, V%01X", x);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsLDBX(void) {
    uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x33);

    sprintf(buf, "LD B, V%01X", x);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsLDIPX(void) {
    uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x55);

    sprintf(buf, "LD [I], V%01X", x);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsLDXIP(void) {
    uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x65);

    sprintf(buf, "LD V%01X, [I]", x);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsLDRX(void) {
    uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x75);

    sprintf(buf, "LD R, V%01X", x);
    test_decode_instruction_should_parse(buf, ins);
}

void test_decode_instruction_WhereInstructionIsLDXR(void) {
    uint16_t ins = BUILD_INSTRUCTION_AXKK(0xF, x, 0x85);

    sprintf(buf, "LD V%01X, R", x);
    test_decode_instruction_should_parse(buf, ins);
}

int main(void) {
    srand(time(NULL));

    UNITY_BEGIN();
    RUN_TEST(test_decode_instruction_WhereInstructionIsCLS);
    RUN_TEST(test_decode_instruction_WhereInstructionIsRET);
    RUN_TEST(test_decode_instruction_WhereInstructionIsSCD);
    RUN_TEST(test_decode_instruction_WhereInstructionIsSCR);
    RUN_TEST(test_decode_instruction_WhereInstructionIsSCL);
    RUN_TEST(test_decode_instruction_WhereInstructionIsEXIT);
    RUN_TEST(test_decode_instruction_WhereInstructionIsLOW);
    RUN_TEST(test_decode_instruction_WhereInstructionIsHIGH);
    RUN_TEST(test_decode_instruction_WhereInstructionIsJPNNN);
    RUN_TEST(test_decode_instruction_WhereInstructionIsJPNNN_WithLabel);
    RUN_TEST(test_decode_instruction_WhereInstructionIsCALL);
    RUN_TEST(test_decode_instruction_WhereInstructionIsCALL_WithLabel);
    RUN_TEST(test_decode_instruction_WhereInstructionIsSEXKK);
    RUN_TEST(test_decode_instruction_WhereInstructionIsSNEXKK);
    RUN_TEST(test_decode_instruction_WhereInstructionIsSEXY);
    RUN_TEST(test_decode_instruction_WhereInstructionIsLDXKK);
    RUN_TEST(test_decode_instruction_WhereInstructionIsADDXKK);
    RUN_TEST(test_decode_instruction_WhereInstructionIsLDXY);
    RUN_TEST(test_decode_instruction_WhereInstructionIsORXY);
    RUN_TEST(test_decode_instruction_WhereInstructionIsANDXY);
    RUN_TEST(test_decode_instruction_WhereInstructionIsXORXY);
    RUN_TEST(test_decode_instruction_WhereInstructionIsADDXY);
    RUN_TEST(test_decode_instruction_WhereInstructionIsSUBXY);
    RUN_TEST(test_decode_instruction_WhereInstructionIsSHRXY);
    RUN_TEST(test_decode_instruction_WhereInstructionIsSUBNXY);
    RUN_TEST(test_decode_instruction_WhereInstructionIsSHLXY);
    RUN_TEST(test_decode_instruction_WhereInstructionIsSNEXY);
    RUN_TEST(test_decode_instruction_WhereInstructionIsLDINNN);
    RUN_TEST(test_decode_instruction_WhereInstructionIsLDINNN_WithLabel);
    RUN_TEST(test_decode_instruction_WhereInstructionIsJPV0NNN);
    RUN_TEST(test_decode_instruction_WhereInstructionIsJPV0NNN_WithLabel);
    RUN_TEST(test_decode_instruction_WhereInstructionIsRNDXKK);
    RUN_TEST(test_decode_instruction_WhereInstructionIsDRWXYB);
    RUN_TEST(test_decode_instruction_WhereInstructionIsSKPX);
    RUN_TEST(test_decode_instruction_WhereInstructionIsSKNPX);
    RUN_TEST(test_decode_instruction_WhereInstructionIsLDXDT);
    RUN_TEST(test_decode_instruction_WhereInstructionIsLDXK);
    RUN_TEST(test_decode_instruction_WhereInstructionIsLDDTX);
    RUN_TEST(test_decode_instruction_WhereInstructionIsLDSTX);
    RUN_TEST(test_decode_instruction_WhereInstructionIsADDIX);
    RUN_TEST(test_decode_instruction_WhereInstructionIsLDFX);
    RUN_TEST(test_decode_instruction_WhereInstructionIsLDHFX);
    RUN_TEST(test_decode_instruction_WhereInstructionIsLDBX);
    RUN_TEST(test_decode_instruction_WhereInstructionIsLDIPX);
    RUN_TEST(test_decode_instruction_WhereInstructionIsLDXIP);
    RUN_TEST(test_decode_instruction_WhereInstructionIsLDRX);
    RUN_TEST(test_decode_instruction_WhereInstructionIsLDXR);
    return UNITY_END();
}
