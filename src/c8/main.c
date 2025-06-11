#include "chip8.h"
#include "util/util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	int cs = CLOCK_SPEED;
	int flags = 0;
	int opt;
	chip8_t *c8;
	int colors[2];
	int palette = 0;

	/* Parse args */
	while ((opt = getopt(argc, argv, "c:dvVp:")) != -1) {
		switch (opt) {
			case 'c':
				cs = atoi(optarg);
				break;
			case 'd':
				flags |= FLAG_DEBUG;
				break;
			case 'p':
				palette = 1;
				if (load_palette(colors, optarg)) {
					return EXIT_FAILURE;
				}
				break;
			case 'v':
				flags |= FLAG_VERBOSE;
				break;
			case 'V':
				print_version(argv[0]);
				exit(EXIT_SUCCESS);
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

	if (palette) {
		memcpy(&c8->colors, colors, sizeof(int) * 2);
	}

	simulate(c8);

	deinit_chip8(c8);

	return EXIT_SUCCESS;
}