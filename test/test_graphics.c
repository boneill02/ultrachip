#include "unity.h"
#include "c8/graphics.c"
#include "c8/defs.h"

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
