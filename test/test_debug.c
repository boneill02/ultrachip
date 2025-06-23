#include "unity.h"

#include "c8/private/debug.c"
#include "c8/decode.c"
#include "c8/private/exception.c"
#include "c8/private/util.h"
#include "c8/defs.h"
#include "c8/font.h"

#include <stdint.h>
#include <stdlib.h>
#include <time.h>

void setUp(void) {
	srand(time(NULL));
}

void tearDown(void) { }

int main(void) {
	UNITY_BEGIN();
	return UNITY_END();
}
