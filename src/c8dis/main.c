#include "dis.h"

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

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

	disassemble(inf, outf, args);

	/* Cleanup */
	fclose(inf);
	if (outf != stdout) {
		fclose(outf);
	}
	return EXIT_SUCCESS;
}