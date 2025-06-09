#include "util/util.h"

#include "util/defs.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Get the value of (x,y) from `display`
 *
 * @param display 1D int arr of size `DISPLAY_WIDTH * DISPLAY_HEIGHT`
 * @param x the x value
 * @param y the y value
 *
 * @return value of (x,y) in `display`
 */
int *get_pixel(int *display, int x, int y)
{
	return &display[y * DISPLAY_WIDTH + x];
}

/**
 * @brief Get the integer value of hexadecimal ASCII representation
 *
 * @param c the char to convert
 *
 * @return -1 if failed, otherwise 0-15
 */
int hex_to_int(char c)
{
	if (c >= '0' && c <= '9')
	{
		return c - '0';
	}
	if (c >= 'A' && c <= 'F')
	{
		return c - 'A' + 10;
	}
	if (c >= 'a' && c <= 'f')
	{
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
 * @return 0 if failed, otherwise whatever the value is
 */
int parse_int(char *s)
{
	int result = -1;
	errno = 0;
	if (s[0] == '$' || s[0] == 'x' || s[0] == 'V' || s[0] == 'v')
	{
		result = strtol(s + 1, NULL, 16);
	}
	else
	{
		result = strtol(s, NULL, 10);
	}

	if (errno)
	{
		return -1;
	}

	return result;
}

void print_version(const char *argv0)
{
	printf("%s %s\n", argv0, VERSION);
}

/**
 * @brief Trim leading and trailing whitespace from `s`
 *
 * Puts a `NULL` character after the last non-whitespace character and
 * returns a pointer to the first non-whitespace character.
 *
 * @param s string to trim
 * @return pointer to first non-whitespace character after `s`
 */
char *trim(char *s)
{
	char *end;

	while (isspace(*s))
	{
		s++;
	}

	end = s + strlen(s) - 1;
	while (end > s && isspace(*end))
	{
		end--;
	}

	*(end + 1) = '\0';
	return s;
}
