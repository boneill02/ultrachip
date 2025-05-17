#include <stdio.h>
#include <stdlib.h>

#include "decode.h"

char result[16];

char *decode_instruction(uint16_t in, uint8_t *label_map) {
	for (int i = 0; i < 16; i++)
		result[i] = '\0';
	int x = (in & 0x0F00) >> 8;
	int kk = in & 0x00FF;
	int nnn = in & 0x0FFF;
	int y = (in & 0x00F0) >> 4;
	int a = (in & 0xF000) >> 12;
	int b = in & 0x000F;

	switch (a) {
		case 0x0:
			if (in == 0x00E0)
				sprintf(result, "CLS");
			else if (in == 0x00EE)
				sprintf(result, "RET");
			break;
		case 0x1:
			if (label_map && label_map[nnn]) {
				sprintf(result, "JP label%d", label_map[nnn]);
			} else {
				sprintf(result, "JP 0x%03x", nnn);
			}
			break;
		case 0x2:
			if (label_map && label_map[nnn]) {
				sprintf(result, "CALL label%d", label_map[nnn]);
			} else {
				sprintf(result, "CALL 0x%03x", nnn);
			}
			break;
		case 0x3:
			sprintf(result, "SE V%01x, 0x%03x", x, kk);
			break;
		case 0x4:
			sprintf(result, "SNE V%01x, 0x%03x", x, kk);
			break;
		case 0x5:
			sprintf(result, "SE V%01x, V%01x", x, y);
			break;
		case 0x6:
			sprintf(result, "LD V%01x, 0x%02x", x, kk);
			break;
		case 0x7:
			sprintf(result, "ADD V%01x, 0x%02x", x, kk);
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
					sprintf(result, "SHR V%01x", x);
					break;
				case 0x7:
					sprintf(result, "SUBN V%01x, V%01x", x, y);
					break;
				case 0xE:
					sprintf(result, "SHL V%01x", x);
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
				sprintf(result, "LD I, %03x", nnn);
			}
			break;
		case 0xB:
			if (label_map && label_map[nnn]) {
				sprintf(result, "JP V0, label%d", label_map[nnn]);
			} else {
				sprintf(result, "JP V0, 0x%03x", nnn);
			}
			break;
		case 0xC:
			sprintf(result, "RND V%01x, 0x%02x", x, kk);
			break;
		case 0xD:
			sprintf(result, "DRW V%01x, V%02x, 0x%01x", x, y, b);
			break;
		case 0xE:
			if (kk == 0x9E) {
				sprintf(result, "SKP V%01x", x);
			}
			if (kk == 0xA1) {
				sprintf(result, "SKNP V%01x", x);
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
				case 0x33:
					sprintf(result, "LD B, V%01x", x);
					break;
				case 0x55:
					sprintf(result, "LD [I], V%01x", x);
					break;
				case 0x65:
					sprintf(result, "LD V%01x, [I]", x);
					break;
			}
			break;
		default:
			sprintf(result, "NOP");
	}

	return result;
}

uint16_t jump(uint16_t in) {
	int a = (in & 0xF000) >> 12;
	int nnn = in & 0x0FFF;

	switch (a) {
		case 0x1:
			return nnn;
		case 0x2:
			return nnn;
		case 0xa:
			return nnn;
		case  0xb:
			return nnn;
	}

	return 0;
}
