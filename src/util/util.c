#include "util/util.h"

#include "util/defs.h"

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

char exception[EXCEPTION_MESSAGE_SIZE];
void *mallocs[MAX_MALLOCS];
FILE *files[MAX_FILES];

/**
 * @brief Get the integer value of hexadecimal ASCII representation
 *
 * @param c the char to convert
 *
 * @return -1 if failed, otherwise 0-15
 */
int hex_to_int(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    return -1;
}

/**
 * @brief Parse decimal or hexadecimal integer from `s`
 *
 * Converts string `s` to an integer. Assumes decimal unless the first
 * character is `'$'` or `'x'`.
 *
 * @param s The string to convert.
 *
 * @return -1 if failed, otherwise whatever the value is
 */
int parse_int(const char *s) {
	NULLCHECK1(s);
	if (strlen(s) == 0) {
		return -1;
	}

    int result = -1;
	char *endptr = NULL;
    errno = 0;
    if (s[0] == '$' || s[0] == 'x' || s[0] == 'V' || s[0] == 'v') {
        result = strtol(s+1, &endptr, 16);
    } else {
        result = strtol(s, &endptr, 10);
    }

    if (s == endptr || errno) {
        return -1;
    }

    return result;
}

/**
 * @brief Print version from preprocessor definition
 *
 * @param argv0 argv0
 */
void print_version(const char *argv0) {
    printf("%s %s\n", argv0, VERSION);
}

/**
 * @brief Close file and remove from list of open files
 *
 * @param f file to close
 */
void safe_fclose(FILE *f) {
	for (int i = 0; i < MAX_FILES; i++) {
		if (files[i] == f) {
			files[i] = NULL;
		}
	}

	fclose(f);
}

/**
 * @brief Open file and add list of open files
 *
 * @param p path of file
 * @param m modes (rw etc)
 * @return file pointer
 */
FILE *safe_fopen(const char *p, const char *m) {
	FILE *f = fopen(p, m);

	if (!f) {
		sprintf(exception, "Path: %s", p);
		safe_exit(LOAD_FILE_FAILURE_EXCEPTION);
	}

	for (int i = 0; i < MAX_FILES; i++) {
		if (!files[i]) {
			files[i] = f;
			return files[i];
		}
	}

	safe_exit(0);
	return NULL;
}

/**
 * @brief Free malloc'd memory and remove from list of mallocs
 *
 * @param m pointer to memory to free
 */
void safe_free(void *m) {
	for (int i = 0; i < MAX_MALLOCS; i++) {
		if (m == mallocs[i]) {
			mallocs[i] = NULL;
		}
	}

	free(m);
}

/**
 * @brief malloc some memory of the given size and add to list of mallocs
 *
 * @param size size to malloc
 * @return pointer to memory
 */
void *safe_malloc(size_t size) {
	void *m;

	for (int i = 0; i < MAX_MALLOCS; i++) {
		if (!mallocs[i]) {
			mallocs[i] = malloc(size);
			return mallocs[i];
		}
	}

	safe_exit(TOO_MANY_MALLOCS_EXCEPTION);

	return NULL;
}

/** 
 * @brief calloc some memory of the given size and add to list of mallocs
 *
 * @param size size to calloc
 * @return pointer to memory
 */
void *safe_calloc(size_t nmemb, size_t size) {
	void *m;

	for (int i = 0; i < MAX_MALLOCS; i++) {
		if (!mallocs[i]) {
			mallocs[i] = calloc(nmemb, size);
			return mallocs[i];
		}
	}

	safe_exit(TOO_MANY_MALLOCS_EXCEPTION);

	return NULL;
}

/** 
 * @brief realloc some memory of the given size and add to list of mallocs
 *
 * @param size size to realloc
 * @return pointer to memory
 */
void *safe_realloc(void *ptr, size_t size) {
	void *m;

	for (int i = 0; i < MAX_MALLOCS; i++) {
		if (!mallocs[i]) {
			mallocs[i] = realloc(ptr, size);
			return mallocs[i];
		}
	}

	safe_exit(TOO_MANY_MALLOCS_EXCEPTION);
	return NULL;
}

/** 
 * @brief close all files, free all mallocs, and print exception code/message if exists
 *
 * @param size size to calloc
 * @return pointer to memory
 */
void safe_exit(int status) {
	if (status != 1) {
		switch (status) {
			case NULL_ARGUMENT_EXCEPTION:
				fprintf(stderr, "%s\n", NULL_ARGUMENT_EXCEPTION_MESSAGE);
				break;
			case INVALID_INSTRUCTION_EXCEPTION:
				fprintf(stderr, "%s\n", INVALID_INSTRUCTION_EXCEPTION_MESSAGE);
				break;
			case TOO_MANY_LABELS_EXCEPTION:
				fprintf(stderr, "%s\n", TOO_MANY_LABELS_EXCEPTION_MESSAGE);
				break;
			case STACK_OVERFLOW_EXCEPTION:
				fprintf(stderr, "%s\n", STACK_OVERFLOW_EXCEPTION_MESSAGE);
				break;
			case INVALID_ARGUMENT_EXCEPTION:
				fprintf(stderr, "%s\n", INVALID_ARGUMENT_EXCEPTION_INTERNAL);
				break;
			case INVALID_ARGUMENT_EXCEPTION_INTERNAL:
				fprintf(stderr, "%s\n", INVALID_ARGUMENT_EXCEPTION_INTERNAL_MESSAGE);
				break;
			case DUPLICATE_LABEL_EXCEPTION:
				fprintf(stderr, "%s\n", DUPLICATE_LABEL_EXCEPTION_MESSAGE);
				break;
			case INVALID_SYMBOL_EXCEPTION:
				fprintf(stderr, "%s\n", INVALID_SYMBOL_EXCEPTION_MESSAGE);
				break;
			case TOO_MANY_MALLOCS_EXCEPTION:
				fprintf(stderr, "%s\n", TOO_MANY_MALLOCS_EXCEPTION_MESSAGE);
				break;
			default:
				fprintf(stderr, "(code %d): %s\n", status, UNKNOWN_EXCEPTION_MESSAGE);
		}
		
		if (strlen(exception)) {
			fprintf(stderr, "%s\n", exception);
		}
	}

	for (int i = 0; i < MAX_MALLOCS; i++) {
		if (mallocs[i]) {
			free(mallocs[i]);
		}
	}

	for (int i = 0; i < MAX_FILES; i++) {
		if (files[i]) {
			fclose(files[i]);
		}
	}

	exit(status);
}

/**
 * @brief Trim leading and trailing whitespace from `s`
 *
 * Puts a `NULL` character after the last non-whitespace character and 
 * returns a pointer to the first non-whitespace character.
 *
 * @param s string to trim
 * @return pointer to first non-whitespace character after `s`.
 */
char *trim(char *s) {
	if (s == NULL) {
		return NULL;
	}

    int len = strlen(s);
	int startIdx = 0;
	int endIdx = len - 1;

    while (startIdx < len && isspace(s[startIdx])) {
        startIdx++;
    }

	if (startIdx == len) {
		return &s[len+1]; // empty string
	}
	while (endIdx > 0 && isspace(s[endIdx])) {
		endIdx--;
	}

	if (endIdx < len) {
		s[endIdx + 1] = '\0';
	}
	return &s[startIdx];
}
