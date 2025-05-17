#include "util.h"

#include "chip8.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

int *get_pixel(int *display, int x, int y) {
    return &display[y * DISPLAY_WIDTH + x];
}

int parse_int(char *s) {
    if (s[0] == 'x') {
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
