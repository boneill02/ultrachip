#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "decode.h"

int
main(int argc, char *argv[])
{
	FILE *f;
	if (argc == 2) {
		f = fopen(argv[1], "r");
		if (!f) {
			fprintf(stderr, "failed to load rom file\n");
			exit(EXIT_FAILURE);
		}
	} else {
		fprintf(stderr, "no rom specified\n");
		exit(EXIT_FAILURE);
	}

	uint16_t ins = 0;
	uint16_t addr = 0x200;
	int c;
	while ((c = fgetc(f)) != EOF) {
		if (addr % 2 == 0) {
			ins = ((uint16_t) c) << 8;
		} else {
			ins |= (uint16_t) c;
			printf("%03x: %s\n", addr - 1, decode_instruction(ins));
		}
		addr++;
	}

	fclose(f);
	return EXIT_SUCCESS;
}
