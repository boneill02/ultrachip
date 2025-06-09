#include "chip8.h"
#include "util/util.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	int cs = CLOCK_SPEED;
	int flags = 0;
	int opt;
	chip8_t *c8;

	/* Parse args */
	while ((opt = getopt(argc, argv, "c:dv")) != -1) {
		switch (opt) {
			case 'c':
				cs = atoi(optarg);
				break;
			case 'd':
				flags |= FLAG_DEBUG;
				break;
			case 'v':
				flags |= FLAG_VERBOSE;
				break;
			case 'V':
				print_version(argv[0]);
				break;
			default:
			  fprintf(stderr, "Usage: %s [-dvV] [-c clockspeed] file\n", argv[0]);
			  exit(EXIT_FAILURE);
		}
	}

	srand(time(NULL));

	if (!(c8 = init_chip8(cs, flags, argv[optind]))) {
		fprintf(stderr, "Error: Failed to initialize.\n");
		return EXIT_FAILURE;
	}

	simulate(c8);

	deinit_chip8(c8);

	return EXIT_SUCCESS;
}