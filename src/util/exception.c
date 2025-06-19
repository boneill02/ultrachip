#include "exception.h"

#include "defs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char exception[EXCEPTION_MESSAGE_SIZE];
char exception_messages[19][EXCEPTION_MESSAGE_SIZE] = {
	UNKNOWN_EXCEPTION_MESSAGE,
	NULL_ARGUMENT_EXCEPTION_MESSAGE,
	INVALID_INSTRUCTION_EXCEPTION_MESSAGE,
	TOO_MANY_LABELS_EXCEPTION_MESSAGE,
	STACK_OVERFLOW_EXCEPTION_MESSAGE,
	INVALID_ARGUMENT_EXCEPTION_MESSAGE,
	INVALID_ARGUMENT_EXCEPTION_INTERNAL_MESSAGE,
	DUPLICATE_LABEL_EXCEPTION_MESSAGE,
	INVALID_SYMBOL_EXCEPTION_MESSAGE,
	TOO_MANY_MALLOCS_EXCEPTION_MESSAGE,
	UNKNOWN_EXCEPTION_MESSAGE,
	TOO_MANY_SYMBOLS_EXCEPTION_MESSAGE,
	LOAD_FILE_FAILURE_EXCEPTION_MESSAGE,
	FILE_TOO_BIG_EXCEPTION_MESSAGE,
	INVALID_COLOR_PALETTE_EXCEPTION_MESSAGE,
	INVALID_QUIRK_EXCEPTION_MESSAGE,
	FAILED_GRAPHICS_INITIALIZATION_EXCEPTION_MESSAGE,
	INVALID_FONT_EXCEPTION_MESSAGE,
};

void *mallocs[MAX_MALLOCS];
FILE *files[MAX_FILES];


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
 */
void safe_exit(int status) {
	if (status < -3 && status >= -16) {
		fprintf(stderr, "%s\n", exception_messages[(status * -1) - 2]);
	}

	if (strlen(exception)) {
		fprintf(stderr, "%s\n", exception);
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
