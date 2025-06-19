#include "unity.h"

#include "util/defs.h"
#include "util/decode.c"
#include "util/exception.c"
#include "util/util.c"
#include "c8/debug.c"

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
