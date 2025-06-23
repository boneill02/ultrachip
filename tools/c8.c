#include "c8/chip8.h"
#include "c8/font.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	c8_t *c8 = c8_init(argv[argc - 1], 0);
	int opt;
	char *fontstr = NULL;

	srand(time(NULL));

	/* Parse args */
	while ((opt = getopt(argc, argv, "c:df:p:P:q:vV")) != -1) {
		switch (opt) {
			case 'c': c8->cs = atoi(optarg); break;
			case 'd': c8->flags |= C8_FLAG_DEBUG; break;
			case 'f': fontstr = optarg; break;
			case 'p': c8_load_palette_f(c8, optarg); break;
			case 'P': c8_load_palette_s(c8, optarg); break;
			case 'v': c8->flags |= C8_FLAG_VERBOSE; break;
			case 'q': c8_load_quirks(c8, optarg); break;
			case 'V': printf("%s %d", argv[0], VERSION); exit(0);
			default:
			  fprintf(stderr, "Usage: %s [-dvV] [-c clockspeed] [-f small,big] [-p file] [-P colors] [-q quirks] file\n", argv[0]);
			  exit(1);
		}
	}

	if (fontstr) {
		c8_set_fonts_s(c8, fontstr);
	}

	c8_simulate(c8);
	c8_deinit(c8);

	return EXIT_SUCCESS;
}