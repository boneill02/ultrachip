#include "c8/decode.h"

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
	while ((opt = getopt(argc, argv, "alo:V")) != -1) {
		switch (opt) {
			case 'a': args |= C8_DECODE_PRINT_ADDRESSES; break;
			case 'l': args |= C8_DECODE_DEFINE_LABELS; break;
			case 'o': outp = optarg; break;
			case 'V': printf("%s %d", argv[0], VERSION); exit(0);
			default:
				  fprintf(stderr, "Usage: %s [-al] [-o outputfile] file\n", argv[0]);
				  exit(1);
		}
	}

	inf = fopen(argv[optind], "r");

	if (outp) {
		outf = fopen(outp, "w");
	}

	c8_decode(inf, outf, args);
	fclose(inf);
	if (outp) {
		fclose(outf);
	}
	safe_exit(1);
}
