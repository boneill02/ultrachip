/**
 * @file c8/chip8.c
 *
 * Stuff for simulating and modifying `c8_t`s.
 */

#include "chip8.h"

#include "font.h"

#include "private/debug.h"
#include "private/exception.h"
#include "private/instruction.h"
#include "private/util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define DEBUG(c) (c->flags & C8_FLAG_DEBUG)

static void draw(c8_t*, uint16_t);
static int load_rom(c8_t*, const char*);

/**
 * @brief Deinitialize graphics and free c8
 *
 * @param c8 `c8_t` to deinitialize
 */
void c8_deinit(c8_t* c8) {
    c8_deinit_graphics();
    free(c8);
}

/**
 * @brief Initialize and return a `c8_t` with the given flags
 *
 * This function allocates memory for a new `c8_t` with all values set to 0
 * or their default values, adds the font to memory, inititializes the graphics
 * system, and returns a pointer to the `c8_t`.
 *
 * @param path path to ROM file
 * @param flags flags
 *
 * @return pointer to initialized `c8_t`.
 */
c8_t* c8_init(const char* path, int flags) {
    NULLCHECK1(path);
    int res;

    c8_t* c8 = (c8_t*)calloc(1, sizeof(c8_t));

    if (!c8) {
        snprintf(c8_exception, EXCEPTION_MESSAGE_SIZE, "At %s", __func__);
        handle_exception(MEMORY_ALLOCATION_EXCEPTION);
        return NULL;
    }

    c8->flags = flags;
    c8->cs = C8_CLOCK_SPEED;
    c8->colors[1] = 0xFFFFFF;
    c8->display.mode = C8_DISPLAYMODE_HIGH;
    c8->mode = C8_MODE_CHIP8;


    if ((res = load_rom(c8, path)) < 0) {
        free(c8);
        return NULL;
    }

    c8_set_fonts(c8, 0, 0);
    c8_init_graphics();
    return c8;
}

/**
 * @brief Load palette from the given string into `colors`.
 *
 * @param c8 where to store the color codes
 * @param s string to load
 *
 * @return 1 if success
 */
int c8_load_palette_s(c8_t* c8, char* s) {
    NULLCHECK2(c8, s);
    char* c[2];
    int len = strlen(s);

    c[0] = s;
    for (int i = 0; i < len; i++) {
        if (s[i] == ',') {
            s[i] = '\0';
            c[1] = &s[i + 1];
        }
    }

    if (!c[1]) {
        handle_exception(INVALID_COLOR_PALETTE_EXCEPTION);
    }

    for (int i = 0; i < 2; i++) {
        if ((c8->colors[i] = parse_int(c[i])) == -1) {
            handle_exception(INVALID_COLOR_PALETTE_EXCEPTION);
        }
    }

    return 1;
}

/**
 * @brief Load palette from the given path into `colors`.
 *
 * @param c8 where to store the color codes
 * @param path palette file location
 *
 * @return 1 if success
 */
int c8_load_palette_f(c8_t* c8, const char* path) {
    NULLCHECK2(c8, path);

    char buf[BUFSIZ];
    buf[0] = '$';
    int c;
    FILE* f = fopen(path, "r");
    if (!f) {
        snprintf(c8_exception,
            EXCEPTION_MESSAGE_SIZE,
            "Failed to open color palette.\n");
        handle_exception(INVALID_COLOR_PALETTE_EXCEPTION);
        return 0;
    }
    for (int i = 0; i < 2; i++) {
        fgets(buf + 1, BUFSIZ - 1, f);
        if ((c = parse_int(buf)) == -1) {
            snprintf(c8_exception,
                EXCEPTION_MESSAGE_SIZE,
                "Integer parse error while loading color palette\n");
            handle_exception(INVALID_COLOR_PALETTE_EXCEPTION);
            return 0;
        }
        c8->colors[i] = c;
    }

    fclose(f);
    return 1;
}

/**
 * @brief Load quirk flags from string
 *
 * @param c8 where to store flags
 * @param s string to get quirks from
 */
void c8_load_quirks(c8_t* c8, const char* s) {
    NULLCHECK2(c8, s);

    for (int i = 0; i < strlen(s); i++) {
        switch (s[i]) {
        case 'b': c8->flags ^= C8_FLAG_QUIRK_BITWISE; break;
        case 'd': c8->flags ^= C8_FLAG_QUIRK_DRAW; break;
        case 'j': c8->flags ^= C8_FLAG_QUIRK_JUMP; break;
        case 'l': c8->flags ^= C8_FLAG_QUIRK_LOADSTORE; break;
        case 's': c8->flags ^= C8_FLAG_QUIRK_SHIFT; break;
        default:
            handle_exception(INVALID_QUIRK_EXCEPTION);
        }
    }
}

/**
 * @brief Main interpreter simulation loop. Exits when `c8->running` is 0.
 *
 * @param c8 the `c8_t` to simulate
 */
void c8_simulate(c8_t* c8) {
    int debugRet;
    int ret;
    int step = 1;

    srand(time(NULL));

    c8->pc = C8_PROG_START;
    c8->running = 1;

    if (c8->cs <= 0) {
        snprintf(c8_exception, EXCEPTION_MESSAGE_SIZE, "Invalid clock speed: %d\n", c8->cs);
        handle_exception(INVALID_CLOCK_SPEED_EXCEPTION);
        return;
    }

    while (c8->running) {
        usleep(1000000 / c8->cs);
        int t = c8_tick(c8->key);

        if (t == -2) {
            /* Quit */
            c8->running = 0;
            continue;
        }

        if (c8->key[16]) {
            /* Enter debug mode */
            c8->flags |= C8_FLAG_DEBUG;
            step = 1;
        }

        if (c8->key[17]) {
            /* Exit debug mode */
            if (DEBUG(c8)) {
                c8->flags ^= C8_FLAG_DEBUG;
            }
        }

        if (DEBUG(c8) && (has_breakpoint(c8, c8->pc) || step)) {
            /* Call debug REPL and process return value */
            debugRet = debug_repl(c8);

            switch (debugRet) {
            case DEBUG_QUIT:
                c8->running = 0;
                continue;
            case DEBUG_STEP:
                step = 1;
                break;

            }
        }

        if (t >= 0 && c8->waitingForKey) {
            /* Waiting for key and a key was pressed */
            c8->V[c8->VK] = t;
            c8->waitingForKey = 0;
        }

        if (!c8->waitingForKey) {
            /* Not waiting for key, parse next instruction */
            ret = parse_instruction(c8);
            if (ret < 0) {
                handle_exception(ret);
            }

            c8->pc += ret;

            if (c8->dt > 0) {
                c8->dt--;
            }

            if (c8->st > 0) {
                c8->st--; // TODO sound
            }

            if (c8->draw) {
                c8_render(&c8->display, c8->colors);
                c8->draw = 0;
            }
        }
    }
}

/**
 * @brief Load a ROM to `c8->mem` at path `addr`.
 *
 * @param c8 `c8_t` to store the ROM's contents
 * @param addr path to the ROM
 *
 * @return 1 if success.
 */
static int load_rom(c8_t* c8, const char* addr) {
    FILE* f;
    int size;

    f = fopen(addr, "r");
    if (!f) {
        snprintf(c8_exception, EXCEPTION_MESSAGE_SIZE, "File: %s\n", addr);
        handle_exception(LOAD_FILE_FAILURE_EXCEPTION);
        return LOAD_FILE_FAILURE_EXCEPTION;
    }

    /* Get file size */
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    if (ftell(f) > (0x1000 - 0x200)) {
        /* File is too big, failure */
        handle_exception(FILE_TOO_BIG_EXCEPTION);
    }
    rewind(f);

    /* Read the file into memory */
    fread(c8->mem + C8_PROG_START, size, 1, f);
    fclose(f);

    return 1;
}
