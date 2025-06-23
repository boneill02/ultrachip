#include "unity.h"

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
