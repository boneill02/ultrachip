#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "chip8.h"
#include "decode.h"

#define ARG_PRINT_ADDRESSES 0x1
#define ARG_DEFINE_LABELS 0x2

void find_labels(FILE *);
void disassemble(FILE *, FILE *, int);

uint8_t *labelMap = NULL;

/**
 * @brief Generate labels from `input` and add labels to `labelMap`.
 * 
 * This function finds jump instructions in CHIP-8 ROM file `input` and adds
 * incrementing values to `labelMap` accordingly.
 * 
 * @param input the ROM to get labels from
 */
void find_labels(FILE *input) {
	uint16_t addr = PROG_START;
	uint8_t count = 1;
	uint16_t ins = 0;
	uint16_t to;

	int c;
	while ((c = fgetc(input)) != EOF) {
		if (addr % 2 == 0) {
			ins = ((uint16_t) c) << 8;
		} else {
			ins |= (uint16_t) c;
			if ((to = jump(ins))) {
				labelMap[to] = count++;
			}
		}
	}
}

/**
 * @brief Convert bytecode from `input` to assembly and writes it to `output`.
 * 
 * `ARG_PRINT_ADDRESSES` should be AND'd to args to print addresses before each
 * assembly instruction.
 * 
 * `ARG_DEFINE_LABELS` should be AND'd to args to define labels.
 * 
 * @param input the CHIP-8 ROM file to disassemble
 * @param output the file to write the assembly to
 * @param args 0 with `ARG_PRINT_ADDRESSES` and/or `ARG_DEFINE_LABELS`
 * optionally AND'd
 */
void disassemble(FILE *input, FILE *output, int args) {
	int c;
	uint16_t addr = PROG_START;
	uint16_t ins = 0;

	while ((c = fgetc(input)) != EOF) {
		if (addr % 2 == 0) {
			ins = ((uint16_t) c) << 8;
		} else {
			ins |= (uint16_t) c;

			if (args & ARG_DEFINE_LABELS && labelMap[addr]) {
				fprintf(output, "label%d:\n", labelMap[addr]);
			}

			if (args & ARG_PRINT_ADDRESSES) {
				fprintf(output, "%03x: ", addr - 1);
			}
			fprintf(output, "%s\n", decode_instruction(ins, labelMap));
		}
		addr++;
	}
}

int main(int argc, char *argv[]) {
	int args = 0;
	int opt;
	char *outp = NULL;
	FILE *inf;
	FILE *outf = stdout;

	/* Parse args */
	while ((opt = getopt(argc, argv, "alo:")) != -1) {
		switch (opt) {
			case 'a': args |= ARG_PRINT_ADDRESSES; break;
			case 'l': args |= ARG_DEFINE_LABELS; break;
			case 'o': outp = optarg; break;
			default:
				  fprintf(stderr, "Usage: %s [-al] [-o outputfile] file\n", argv[0]);
				  exit(1);
		}
	}

	if (!(inf = fopen(argv[optind], "r"))) {
		fprintf(stderr, "Error: Failed to load rom file\n");
		exit(EXIT_FAILURE);
	}

	if (outp && !(outf = fopen(outp, "w"))) {
		fprintf(stderr, "Error: Failed to load output file\n");
		fclose(inf);
		exit(EXIT_FAILURE);
	}

	if (args & ARG_DEFINE_LABELS) {
		labelMap = (uint8_t *) calloc(0x1000, sizeof(uint8_t));
		find_labels(inf);
		rewind(inf);
	}

	disassemble(inf, outf, args);

	/* Cleanup */
	fclose(inf);
	if (outf != stdout) {
		fclose(outf);
	}
	if (labelMap) {
		free(labelMap);
	}
	return EXIT_SUCCESS;
}
