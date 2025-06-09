#include "graphics.h"

/**
 * @brief Get the value of (x,y) from `display`
 * 
 * @param display 1D int arr of size `DISPLAY_WIDTH * DISPLAY_HEIGHT`
 * @param x the x value
 * @param y the y value
 * 
 * @return value of (x,y) in `display`
 */
int *get_pixel(display_t *display, int x, int y) {
    return &display->p[y * EXTENDED_DISPLAY_WIDTH + x];
}