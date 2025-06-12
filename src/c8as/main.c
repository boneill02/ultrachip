#include "parse.h"
#include "util/util.h"
#include "util/defs.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int assemble(char *, char *, int);
static char *dynamic_load(FILE *);

int main(int argc, char *argv[]) {
	int opt;
	int args = 0;
	char *outpath = "a.c8";

	/* Parse args */
	while ((opt = getopt(argc, argv, "o:vV")) != -1) {
		switch (opt) {
			case 'o':
				outpath = optarg;
				break;
			case 'v':
				args |= ARG_VERBOSE;
				break;
			case 'V':
				print_version(argv[0]);
				exit(EXIT_SUCCESS);
			default:
				  fprintf(stderr, "Usage: %s [-v] [-o outputfile] file\n", argv[0]);
				  exit(1);
		}
	}

	return assemble(argv[optind], outpath, args);
}

static int assemble(char *inpath, char *outpath, int args) {
	FILE *in;
	FILE *out;
	char *input;
	uint8_t *output;
	int len;
	int romSize = MEMSIZE - PROG_START;

	if (!(in = fopen(inpath, "r"))) {
		fprintf(stderr, "Failed to load input file\n");
		return 0;
	}

	if (!outpath || !(out = fopen(outpath, "wb"))) {
		fprintf(stderr, "Failed to load output file\n");
		fclose(in);
		return 0;
	}

	input = dynamic_load(in);
	output = calloc(MEMSIZE - PROG_START, sizeof(uint8_t));

	len = parse(input, output, args);

	for (int i = 0; i < len; i++) {
		fputc(output[i], out);
	}

	fclose(in);
	fclose(out);
	free(input);
	free(output);
	return 1;
}

static char *dynamic_load(FILE *f) {
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
	return buf;
}
