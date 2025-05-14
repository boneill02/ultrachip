#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "chip8.h"
#include "decode.h"

#define ARG_PRINT_ADDRESSES 0x1
#define ARG_DEFINE_LABELS 0x2

uint8_t label_map[0x1000];

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
				label_map[to] = count++;
			}
		}
	}
}

int
disassemble(FILE *input, FILE *output, int args)
{
	uint16_t ins = 0;
	uint16_t addr = PROG_START;
	int c;
	while ((c = fgetc(input)) != EOF) {
		if (addr % 2 == 0) {
			ins = ((uint16_t) c) << 8;
		} else {
			ins |= (uint16_t) c;
			if (label_map[addr - PROG_START]) {
				fprintf(output, "l%d:\n", label_map[addr - PROG_START]);
			}
			if (args & ARG_PRINT_ADDRESSES) {
				fprintf(output, "%03x: ", addr - 1);
			}
			fprintf(output, "%s\n", decode_instruction(ins));
		}
		addr++;
	}

	return 1;
}

int
main(int argc, char *argv[])
{
	/* parse args */
	int opt;
	int args = 0;
	char *outp = NULL;
	FILE *inf;
	FILE *outf = stdout;

	while ((opt = getopt(argc, argv, "alo:")) != -1) {
		switch (opt) {
			case 'a': args |= ARG_PRINT_ADDRESSES; break;
			case 'l': args |= ARG_DEFINE_LABELS; break;
			case 'o': outp = optarg; break;
			default:
				  fprintf(stderr, "usage: %s [-al] [-o outputfile] file", argv[0]);
				  exit(1);
		}
	}

	inf = fopen(argv[optind], "r");
	if (!inf) {
		fprintf(stderr, "failed to load rom file\n");
		exit(EXIT_FAILURE);
	}

	if (outp && !(outf = fopen(outp, "w"))) {
		fprintf(stderr, "failed to load output file\n");
		exit(EXIT_FAILURE);
	}

	if (args & ARG_DEFINE_LABELS) {
		find_labels(inf);
		rewind(inf);
	}

	disassemble(inf, outf, args);
	fclose(inf);
	if (outf != stdout) {
		fclose(outf);
	}
	return EXIT_SUCCESS;
}
