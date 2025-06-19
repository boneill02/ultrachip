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
	int cs;
	int flags;
	int palette = 0;
	int colors[2];
	char *fontstr = NULL;

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
				palette = 1;	
				load_palette_file(colors, optarg);
				break;
			case 'P':
				palette = 1;	
				load_palette_arg(colors, optarg);
				break;
			case 'v':
				flags |= FLAG_VERBOSE;
				break;
			case 'q':
				flags |= load_quirks(optarg);
				break;
			case 'V':
				print_version(argv[0]);
				safe_exit(0);
			default:
			  fprintf(stderr, "Usage: %s [-dvV] [-c clockspeed] [-p file] [-P colors] [-q quirks] file\n", argv[0]);
			  exit(EXIT_FAILURE);
		}
	}

	srand(time(NULL));

	c8 = init_chip8(argv[optind], flags);

	if (fontstr) {
		set_fonts_s(c8, fontstr);
	}

	simulate(c8);
	deinit_chip8(c8);

	return EXIT_SUCCESS;
}