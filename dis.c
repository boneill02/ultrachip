#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "chip8.h"
#include "decode.h"

#define ARG_PRINT_ADDRESSES 0x1
#define ARG_DEFINE_LABELS 0x2

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
	while ((opt = getopt(argc, argv, "al")) != -1) {
		switch (opt) {
			case 'a': args |= ARG_PRINT_ADDRESSES; break;
			case 'l': args |= ARG_DEFINE_LABELS; break;
			default:
				  fprintf(stderr, "usage: %s [-al] file", argv[0]);
				  exit(1);
		}
	}

	FILE *f;
	f = fopen(argv[optind], "r");
	if (!f) {
		fprintf(stderr, "failed to load rom file\n");
		exit(EXIT_FAILURE);
	}

	disassemble(f, stdout, args);
	fclose(f);
	return EXIT_SUCCESS;
}
