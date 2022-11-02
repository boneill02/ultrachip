#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "chip8.h"
#include "decode.h"

#define ARG_PRINT_ADDRESSES 0x1
#define ARG_DEFINE_LABELS 0x2

int
disassemble(FILE *f, int args)
{

	uint16_t ins = 0;
	uint16_t addr = PROG_START;
	int c;
	while ((c = fgetc(f)) != EOF) {
		if (addr % 2 == 0) {
			ins = ((uint16_t) c) << 8;
		} else {
			ins |= (uint16_t) c;

			if (args & ARG_PRINT_ADDRESSES) {
				printf("%03x: %s\n", addr - 1, decode_instruction(ins));
			} else {
				printf("%s\n", decode_instruction(ins));
			}
		}
		addr++;
	}
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

	disassemble(f, args);
	fclose(f);
	return EXIT_SUCCESS;
}
