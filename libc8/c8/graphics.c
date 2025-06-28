/**
 * @file c8/graphics.c
 *
 * Backend-agnostic graphics-related functions
 */

#include "graphics.h"

#include <stdio.h>

 /**
  * @brief Play sound
  *
  * This definition is overridden in libc8_sdl2 by definition in
  * internal/graphics_sdl.c.
  *
  * In libc8, this should be overridden by the user if c8_simulate() is called.
  */
void __attribute__((weak)) c8_beep(void) {
    fprintf(stderr, "c8_beep() not implemented.");
}

/**
 * @brief Deinitialize graphics system
 *
 * This definition is overridden in libc8_sdl2 by definition in
 * internal/graphics_sdl.c.
 *
 * In libc8, this should be overridden by the user if c8_simulate() is called.
 */
void __attribute__((weak)) c8_deinit_graphics(void) {
    fprintf(stderr, "c8_deinit_graphics() not implemented.");
}

/**
 * @brief Initialize graphics system
 *
 * This definition is overridden in libc8_sdl2 by definition in
 * internal/graphics_sdl.c.
 *
 * In libc8, this should be overridden by the user if c8_simulate() is called.
 */
uint8_t __attribute__((weak)) c8_init_graphics(void) {
    fprintf(stderr, "c8_init_graphics() not implemented.\n");
    return -1;
}

/**
 * @brief Render graphics
 *
 * This definition is overridden in libc8_sdl2 by definition in
 * internal/graphics_sdl.c.
 *
 * In libc8, this should be overridden by the user if c8_simulate() is called.
 */
void __attribute__((weak)) c8_render(c8_display_t* display, int* colors) {
    fprintf(stderr, "c8_render() not implemented.");
}

/**
 * @brief Grab current keypresses and delay execution to match clockspeed
 *
 * This definition is overridden in libc8_sdl2 by definition in
 * internal/graphics_sdl.c.
 *
 * In libc8, this should be overridden by the user if c8_simulate() is called.
 */
int __attribute__((weak)) c8_tick(int* key) {
    fprintf(stderr, "tick() not implemented.\n");
    return -1;
}

/**
 * @brief Get the value of (x,y) from `display`
 *
 * @param display `display_t` to get pixel from
 * @param x the x value
 * @param y the y value
 *
 * @return pointer to value of (x,y) in `display`
 */
uint8_t* c8_get_pixel(c8_display_t* display, int x, int y) {
    if (display->mode == C8_DISPLAYMODE_HIGH) {
        x += display->x;
        y += display->y;
    }
    return &display->p[y * C8_LOW_DISPLAY_WIDTH + x];
}
