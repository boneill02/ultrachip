#include "parse.h"
#include "util/defs.h"
#include "util/exception.h"
#include "util/util.h"

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

	int ret = assemble(argv[optind], outpath, args);
	safe_exit(ret);
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
static int assemble(char *inpath, char *outpath, int args) {
	NULLCHECK2(inpath, outpath);

	FILE *in;
	FILE *out;
	char *input;
	uint8_t *output;
	int len;
	int romSize = MEMSIZE - PROG_START;

	if (!(in = safe_fopen(inpath, "r"))) {
		fprintf(stderr, "Failed to load input file\n");
		return 0;
	}

	if (!outpath || !(out = safe_fopen(outpath, "wb"))) {
		fprintf(stderr, "Failed to load output file\n");
		fclose(in);
		return 0;
	}

	input = dynamic_load(in);
	output = safe_calloc(MEMSIZE - PROG_START, sizeof(uint8_t));

	len = parse(input, output, args);

	if (len < 0) {
		safe_exit(len);
	}

	for (int i = 0; i < len; i++) {
		fputc(output[i], out);
	}

	safe_fclose(in);
	safe_fclose(out);
	safe_free(input);
	safe_free(output);
	return 1;
}

/**
 * @brief Dynamically load an entire file into a string
 *
 * @param f file to load
 * @return pointer to string
 */
static char *dynamic_load(FILE *f) {
	NULLCHECK1(f);

	int capacity = BUFSIZ;
	char *buf = (char *) safe_malloc(capacity);
	char *newbuf;
	char ch;
	int len = 0;

	while ((ch = fgetc(f)) != EOF) {
		if (len >= capacity - 1) {
			capacity *= 2;
			newbuf = (char *) safe_realloc(buf, capacity);
		}

		buf[len++] = ch;
	}

	buf[len] = '\0';
	return buf;
}
