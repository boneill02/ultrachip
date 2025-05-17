#include "util.h"

#include "chip8.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

int *get_pixel(int *display, int x, int y) {
    return &display[y * DISPLAY_WIDTH + x];
}

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

int parse_int(char *s) {
    if (s[0] == '$' || s[0] == 'x') {
        return strtol(s+1, NULL, 16);
    } else {
        return strtol(s, NULL, 10);
    }
}

char *trim(char *s) {
    char *end;

    while (isspace(*s)) {
        s++;
    }

    end = s + strlen(s) - 1;
    while (end > s && isspace(*end)) {
        end--;
    }

    *(end + 1) = '\0';
	return s;
}
