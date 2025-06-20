/**
 * @file decode.c
 *
 * Stuff for disassembling bytecode.
 */

#include "util/decode.h"

#include "util/defs.h"

#include <stdio.h>
#include <stdlib.h>

char result[32];

/**
 * @brief Decode `in` and return its assembly value.
 *
 * Gets the assembly value of instruction `in`, stores it in the global
 * variable `result`, and returns `result`.
 *
 * If `label_map` is not `NULL`, it should point to an aray of size `MEMSIZE`,
 * with all "labeled" elements set to a unique, non-zero integer. All other
 * elements should be zero.
 *
 * If `label_map` is not `NULL`, `label_map[nnn]` is greater than 0, and the
 * instruction contains a nnn argument, a label name will be generated and used
 * in the resulting string.
 *
 * @param in The instruction to decode
 * @param label_map The label map (can be NULL for no labels)
 *
 * @return `result` containing the associated assembly instruction
 */
char *decode_instruction(uint16_t in, uint8_t *label_map) {
	for (int i = 0; i < 16; i++) {
		result[i] = '\0';
	}
	EXPAND(in);

	switch (a) {
		case 0x0:
			if (in == 0x00E0) {
				sprintf(result, "CLS");
			} else if (in == 0x00EE) {
				sprintf(result, "RET");
			} else if (x == 0x0 && y == 0xC) {
				sprintf(result, "SCD %d", b);
			} else if (in == 0x00FB) {
				sprintf(result, "SCR");
			} else if (in == 0x00FC) {
				sprintf(result, "SCL");
			} else if (in == 0x00FD) {
				sprintf(result, "EXIT");
			} else if (in == 0x00FE) {
				sprintf(result, "LOW");
			} else if (in == 0x00FF) {
				sprintf(result, "HIGH");
			} else {
				sprintf(result, ".db %04x", in);
			}
			break;
		case 0x1:
			if (label_map && label_map[nnn]) {
				sprintf(result, "JP label%d", label_map[nnn]);
			} else {
				sprintf(result, "JP $%03x", nnn);
			}
			break;
		case 0x2:
			if (label_map && label_map[nnn]) {
				sprintf(result, "CALL label%d", label_map[nnn]);
			} else {
				sprintf(result, "CALL $%03x", nnn);
			}
			break;
		case 0x3:
			sprintf(result, "SE V%01x, $%02x", x, kk);
			break;
		case 0x4:
			sprintf(result, "SNE V%01x, $%02x", x, kk);
			break;
		case 0x5:
			sprintf(result, "SE V%01x, V%01x", x, y);
			break;
		case 0x6:
			sprintf(result, "LD V%01x, $%02x", x, kk);
			break;
		case 0x7:
			sprintf(result, "ADD V%01x, $%02x", x, kk);
			break;
		case 0x8:
			switch (b) {
				case 0x0:
					sprintf(result, "LD V%01x, V%01x", x, y);
					break;
				case 0x1:
					sprintf(result, "OR V%01x, V%01x", x, y);
					break;
				case 0x2:
					sprintf(result, "AND V%01x, V%01x", x, y);
					break;
				case 0x3:
					sprintf(result, "XOR V%01x, V%01x", x, y);
					break;
				case 0x4:
					sprintf(result, "ADD V%01x, V%01x", x, y);
					break;
				case 0x5:
					sprintf(result, "SUB V%01x, V%01x", x, y);
					break;
				case 0x6:
					sprintf(result, "SHR V%01x, V%01x", x, y);
					break;
				case 0x7:
					sprintf(result, "SUBN V%01x, V%01x", x, y);
					break;
				case 0xE:
					sprintf(result, "SHL V%01x, V%01x", x, y);
					break;
				default:
					sprintf(result, ".db %04x", in);
					break;
			}
			break;
		case 0x9:
			sprintf(result, "SNE V%01x, V%01x", x, y);
			break;
		case 0xA:
			if (label_map && label_map[nnn]) {
				sprintf(result, "LD I, label%d", label_map[nnn]);
			} else {
				sprintf(result, "LD I, $%03x", nnn);
			}
			break;
		case 0xB:
			if (label_map && label_map[nnn]) {
				sprintf(result, "JP V0, label%d", label_map[nnn]);
			} else {
				sprintf(result, "JP V0, $%03x", nnn);
			}
			break;
		case 0xC:
			sprintf(result, "RND V%01x, $%02x", x, kk);
			break;
		case 0xD:
			sprintf(result, "DRW V%01x, V%01x, $%01x", x, y, b);
			break;
		case 0xE:
			if (kk == 0x9E) {
				sprintf(result, "SKP V%01x", x);
			} else if (kk == 0xA1) {
				sprintf(result, "SKNP V%01x", x);
			} else {
				sprintf(result, ".db %04x", in);
			}
			break;
		case 0xF:
			switch (kk) {
				case 0x07:
					sprintf(result, "LD V%01x, DT", x);
					break;
				case 0x0A:
					sprintf(result, "LD V%01x, K", x);
					break;
				case 0x15:
					sprintf(result, "LD DT, V%01x", x);
					break;
				case 0x18:
					sprintf(result, "LD ST, V%01x", x);
					break;
				case 0x1E:
					sprintf(result, "ADD I, V%01x", x);
					break;
				case 0x29:
					sprintf(result, "LD F, V%01x", x);
					break;
				case 0x30:
					sprintf(result, "LD HF, V%01x", x);
					break;
				case 0x33:
					sprintf(result, "LD B, V%01x", x);
					break;
				case 0x55:
					sprintf(result, "LD [I], V%01x", x);
					break;
				case 0x65:
					sprintf(result, "LD V%01x, [I]", x);
					break;
				case 0x75:
					sprintf(result, "LD R, V%01x", x);
					break;
				case 0x85:
					sprintf(result, "LD V%01x, R", x);
					break;
				default:
					sprintf(result, ".db %04x", in);
			}
			break;
		default:
			sprintf(result, ".db %04x", in);
	}

	return result;
}

/**
 * @brief Returns the value to jump to if `n` is a jump instruction, or 0.
 *
 * @param in The instruction to check
 *
 * @return `in`'s `nnn` value if it exists, 0 otherwise.
 */
uint16_t jump(uint16_t in) {
	uint16_t a = A(in);
	uint16_t nnn = NNN(in);

	if (a == 0x1 || a == 0x2 || a == 0xa || a == 0xb) {
		return nnn;
	}
	return 0;
}
