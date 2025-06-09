#include "encode.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char *dynamic_load(FILE *f) {
    int capacity = BUFSIZ;
    char *buf = (char *) malloc(capacity);
    char *newbuf;
    char ch;
    int len = 0;

    while ((ch = fgetc(f)) != EOF) {
        if (len >= capacity - 1) {
            capacity *= 2;
            newbuf = (char *) realloc(buf, capacity);

            if (!newbuf) {
                free(buf);
                fclose(f);
                return NULL;
            }
        }

        buf[len++] = ch;
    }

    buf[len] = '\0';
    fclose(f);
    return buf;
}


int main(int argc, char *argv[]) {
	int opt;
    int args = 0;
	char *outp = "a.c8";
	FILE *inf;
	FILE *outf;

	/* Parse args */
	while ((opt = getopt(argc, argv, "ov:")) != -1) {
		switch (opt) {
			case 'v': args |= ARG_VERBOSE; break;
			case 'o': outp = optarg; break;
			default:
				  fprintf(stderr, "Usage: %s [-v] [-o outputfile] file\n", argv[0]);
				  exit(1);
		}
	}

	if (!(inf = fopen(argv[optind], "r"))) {
        error("Failed to load input file", 0);
		exit(EXIT_FAILURE);
	}

    if (!outp || !(outf = fopen(outp, "wb"))) {
        error("Failed to load output file", 0);
        fclose(inf);
    }

    char *input = dynamic_load(inf);
	parse(input, outf);
    free(input);
	return EXIT_SUCCESS;
}