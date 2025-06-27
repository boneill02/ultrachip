/**
 * @file c8/private/util.c
 * @note NOT EXPORTED
 *
 * Miscellaneous useful functions.
 */

#include "util.h"

#include "../defs.h"
#include "exception.h"

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

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
int parse_int(const char* s) {
    NULLCHECK1(s);
    int len = strlen(s);
    if (len == 0) {
        return -1;
    }

    int result = -1;
    char* endptr = NULL;
    errno = 0;
    if (s[0] == '$' || s[0] == 'x' || s[0] == 'V' || s[0] == 'v') {
        result = strtol(s + 1, &endptr, 16);
    }
    else if (len > 2 && s[0] == '0' && s[1] == 'x') {
        result = strtol(s + 2, &endptr, 16);
    }
    else if (len > 2 && s[0] == '0' && s[1] == 'b') {
        result = strtol(s + 2, &endptr, 2);
    }
    else {
        result = strtol(s, &endptr, 10);
    }

    if (s == endptr || errno) {
        return -1;
    }

    return result;
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
char* trim(char* s) {
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
        return &s[len + 1]; // empty string
    }
    while (endIdx > 0 && isspace(s[endIdx])) {
        endIdx--;
    }

    if (endIdx < len) {
        s[endIdx + 1] = '\0';
    }
    return &s[startIdx];
}
