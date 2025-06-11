#include "unity.h"
#include "instruction.h"

#include <stdint.h>
#include <stdlib.h>
#include <time.h>

char buf[64];
instruction_t ins;
symbol_list_t symbols;
int fc = 0;

void setUp(void) {
	srand(time(NULL));

	symbols.s = calloc(SYMBOL_CEILING, sizeof(symbol_t));

	for(fc = 0; formats[fc].cmd != I_NULL; fc++);
}

void tearDown(void) {
	free(symbols.s);
}

static int shift(uint16_t fmt) {
	int shift = 0;
	while ((fmt & 1) == 0) {
		fmt >>= 1;
		shift++;
	}

	return shift;
}

void generate_valid_instruction_symbols(int idx, int ln) {
	instruction_format_t *fmt = &formats[rand() % fc];

	symbols.s[idx].ln = ln;
	symbols.s[idx].type = SYM_INSTRUCTION;
	symbols.s[idx].value = fmt->cmd;

	idx++;
	symbols.len = idx + fmt->pcount;
	for (int i = 0; i < fmt->pcount; i++) {
		symbols.s[idx+i].ln = ln;
		symbols.s[idx+i].type = fmt->ptype[i];
		switch (fmt->ptype[i]) {
			case SYM_R:
			case SYM_V:
			case SYM_INT:
				symbols.s[idx+i].value = rand() % 16;
				break;
			default:
				symbols.s[idx+i].value = 0;
		}
	}
}

void generate_invalid_instruction_symbols(int idx, int ln) {
	instruction_format_t *fmt = &formats[rand() % fc];

	symbols.s[idx].ln = ln;
	symbols.s[idx].type = SYM_INSTRUCTION;
	symbols.s[idx].value = fmt->cmd;

	int count = fmt->pcount == 0 ? 1 : 0;

	idx++;
	symbols.len = idx + fmt->pcount;
	for (int i = 0; i < count; i++) {
		symbols.s[idx+i].ln = ln;
		symbols.s[idx+i].type = fmt->ptype[i];
		switch (fmt->ptype[i]) {
			case SYM_V:
				symbols.s[idx+i].value = rand() % 16;
				break;
			case SYM_INT:
				symbols.s[idx+i].value = rand() % 0x100;
				break;
			default:
				symbols.s[idx+i].value = 0;
		}
	}

	symbols.s[idx + count].ln = ln;
	symbols.s[idx + count].type = SYM_NULL;
}

void test_build_instruction_valid(void) {
	int idx = rand() % SYMBOL_CEILING - 5;
	generate_valid_instruction_symbols(idx, rand());

	TEST_ASSERT_NOT_EQUAL_INT(0, build_instruction(&ins, &symbols, idx));
}

void test_build_instruction_invalid(void) {
	int idx = rand() % SYMBOL_CEILING - 5;
	generate_invalid_instruction_symbols(idx, rand());
	symbol_list_t symbols[SYMBOL_CEILING];	

	TEST_ASSERT_EQUAL_INT(0, build_instruction(&ins, symbols, idx));
}

void test_build_instruction_valid_null_symbol_table(void) {
	generate_invalid_instruction_symbols(0, 0);
	int idx = rand() % SYMBOL_CEILING - 5;

	TEST_ASSERT_EQUAL_INT(0, build_instruction(&ins, NULL, idx));
}

void test_build_instruction_valid_negative_idx(void) {
	generate_valid_instruction_symbols(0, 0);
	int idx = (rand() % SYMBOL_CEILING) * -1;
	symbol_list_t symbols[SYMBOL_CEILING];	

	TEST_ASSERT_EQUAL_INT(0, build_instruction(&ins, symbols, idx));
}

int main(void) {
    UNITY_BEGIN();
	RUN_TEST(test_build_instruction_valid);
	RUN_TEST(test_build_instruction_invalid);
	RUN_TEST(test_build_instruction_valid_negative_idx);
	RUN_TEST(test_build_instruction_valid_null_symbol_table);
    return UNITY_END();
}