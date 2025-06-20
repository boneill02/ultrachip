#include "chip8.h"
#include "font.h"
#include "util/exception.h"
#include "util/util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	chip8_t *c8;
	int opt;
	int cs = 0;
	int flags = 0;
	int colors[2] = {0, 0xFFFFFF};
	char *fontstr = NULL;

	srand(time(NULL));

	/* Parse args */
	while ((opt = getopt(argc, argv, "c:df:p:P:q:vV")) != -1) {
		switch (opt) {
			case 'c':
				cs = parse_int(optarg);
				break;
			case 'd':
				flags |= FLAG_DEBUG;
				break;
			case 'f':
				fontstr = optarg;
				break;
			case 'p':
				load_palette_file(colors, optarg);
				break;
			case 'P':
				load_palette_arg(colors, optarg);
				break;
			case 'v':
				flags |= FLAG_VERBOSE;
				break;
			case 'q':
				load_quirks(&flags, optarg);
				break;
			case 'V':
				print_version(argv[0]);
				safe_exit(0);
				break;
			default:
			  fprintf(stderr, "Usage: %s [-dvV] [-c clockspeed] [-f small,big] [-p file] [-P colors] [-q quirks] file\n", argv[0]);
			  exit(EXIT_FAILURE);
		}
	}


	c8 = init_chip8(argv[optind], flags);
	c8->colors[0] = colors[0];
	c8->colors[1] = colors[1];

	if (fontstr) {
		set_fonts_s(c8, fontstr);
	}

	if (cs) {
		c8->cs = cs;
	}


	simulate(c8);
	deinit_chip8(c8);

	return EXIT_SUCCESS;
}