#include "util.h"
#include "chip8.h"

int *get_pixel(int *display, int x, int y) {
    return &display[x * DISPLAY_WIDTH + y];
}
