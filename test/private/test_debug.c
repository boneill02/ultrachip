#include "unity.h"

#include "util/defs.h"
#include "util/decode.c"
#include "util/exception.c"
#include "util/util.c"
#include "c8/debug.c"
#include "c8/font.c"

#include <stdint.h>
#include <stdlib.h>
#include <time.h>

void load_quirks(int *flags, const char *s) {
	NULLCHECK2(flags, s);

	for (int i = 0; i < strlen(s); i++) {
		switch (s[i]) {
			case 'b':
				*flags ^= FLAG_QUIRK_BITWISE;
				break;
			case 'd':
				*flags ^= FLAG_QUIRK_DRAW;
				break;
			case 'j':
				*flags ^= FLAG_QUIRK_JUMP;
				break;
			case 'l':
				*flags ^= FLAG_QUIRK_LOADSTORE;
				break;
			case 's':
				*flags ^= FLAG_QUIRK_SHIFT;
				break;
			default:
				safe_exit(INVALID_QUIRK_EXCEPTION);
		}
	}
}
void setUp(void) {
	srand(time(NULL));
}

void tearDown(void) { }

int main(void) {
	UNITY_BEGIN();
	return UNITY_END();
}
