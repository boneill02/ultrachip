#include "encode.h"

#include <stdlib.h>
#include <string.h>

#define TEST "SNE V2 46\n"

int main(int argc, char *argv[]) {
    char *testStr = malloc(500);
    strcpy(testStr, TEST);
    parse(testStr);

    free(testStr);
    return 0;
}