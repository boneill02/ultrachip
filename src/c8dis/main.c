#include "dis.h"

#include "util/exception.h"
#include "util/util.h"

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
			case 'a':
				args |= ARG_PRINT_ADDRESSES;
				break;
			case 'l':
				args |= ARG_DEFINE_LABELS;
				break;
			case 'o':
				outp = optarg;
				break;
			case 'V':
				print_version(argv[0]);
				safe_exit(0);
			default:
				  fprintf(stderr, "Usage: %s [-al] [-o outputfile] file\n", argv[0]);
				  safe_exit(1);
		}
	}

	inf = safe_fopen(argv[optind], "r");

	if (outp) {
		outf = safe_fopen(outp, "w");
	}

	disassemble(inf, outf, args);
	safe_exit(1);
}
