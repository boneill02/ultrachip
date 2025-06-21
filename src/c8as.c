#include "libc8/encode.h"
#include "libc8/defs.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int assemble(const char *, const char *, int);
static char *dynamic_load(FILE *);

int main(int argc, char *argv[]) {
	int opt;
	int args = 0;
	const char *outpath = "a.c8";

	/* Parse args */
	while ((opt = getopt(argc, argv, "o:vV")) != -1) {
		switch (opt) {
			case 'o': outpath = optarg; break;
			case 'v': args |= ARG_VERBOSE; break;
			case 'V': printf("%s %d", argv[0], VERSION); exit(0);
			default:
				  fprintf(stderr, "Usage: %s [-v] [-o outputfile] file\n", argv[0]);
				  exit(1);
		}
	}

	int ret = assemble(argv[optind], outpath, args);
	return 0;
}

/**
 * @brief Assemble contents in input file and write to output file
 *
 * @param inpath path to input file
 * @param outpath path to output file
 * @param args CLI args
 *
 * @return 1 if success, 0 otherwise
 */
static int assemble(const char *inpath, const char *outpath, int args) {
	FILE *in;
	FILE *out;
	char *input;
	uint8_t *output;
	int len;
	int romSize = C8_MEMSIZE - C8_PROG_START;

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
	output = (uint8_t *) calloc(romSize, sizeof(uint8_t));

	len = c8_encode(input, output, args);

	for (int i = 0; i < len; i++) {
		fputc(output[i], out);
	}

	fclose(in);
	fclose(out);
	free(input);
	free(output);
	return 1;
}

/**
 * @brief Dynamically load an entire file into a string
 *
 * @param f file to load
 * @return pointer to string
 */
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
			free(buf);
			buf = newbuf;
		}

		buf[len++] = ch;
	}

	buf[len] = '\0';
	return buf;
}
