/**
 * @file c8/chip8.c
 *
 * Stuff for simulating and modifying `c8_t`s.
 */

#include "chip8.h"

#include "decode.h"
#include "font.h"

#include "private/debug.h"
#include "private/exception.h"
#include "private/util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DEBUG(c) (c->flags & C8_FLAG_DEBUG)
#define VERBOSE(c) (c->flags & C8_FLAG_VERBOSE)

#define QUIRK_BITWISE(c) \
	if (c->flags & C8_FLAG_QUIRK_BITWISE) { \
		c->V[0xF] = 0; \
	}

#define QUIRK_DRAW(c) \
	if (c->flags & C8_FLAG_QUIRK_DRAW) { \
		if () \
	}

#define QUIRK_LOADSTORE(c) \
	if (c->flags & C8_FLAG_QUIRK_LOADSTORE) { \
		c->I += x + 1; \
	}

#define QUIRK_SHIFT(c) \
	if (c->flags & C8_FLAG_QUIRK_SHIFT) { \
		y = x; \
	}

#define BORROWS(x, y) ((((int) x) - y) < 0)
#define CARRIES(x, y) ((((int) x) + y) > UINT8_MAX)

static void draw(c8_t*, uint16_t);
static int load_rom(c8_t*, const char*);
static int parse_instruction(c8_t*);

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

    c8_t* c8 = (c8_t*)calloc(1, sizeof(c8_t));

    if (!c8) {
        sprintf(c8_exception, "At %s", __func__);
        handle_exception(MEMORY_ALLOCATION_EXCEPTION);
        return NULL;
    }

    c8->flags = flags;
    c8->cs = C8_CLOCK_SPEED;
    c8->colors[1] = 0xFFFFFF;

    load_rom(c8, path);
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
        sprintf(c8_exception, "Failed to open color palette.\n");
        handle_exception(INVALID_COLOR_PALETTE_EXCEPTION);
        return 0;
    }
    for (int i = 0; i < 2; i++) {
        fgets(buf + 1, BUFSIZ - 1, f);
        if ((c = parse_int(buf)) == -1) {
            sprintf(c8_exception,
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
    int t;
    int debugRet;
    int ret;
    int step = 1;

    srand(time(NULL));

    c8->pc = C8_PROG_START;
    c8->running = 1;

    while (c8->running) {
        t = c8_tick(c8->key, c8->cs);

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
        sprintf(c8_exception, "File: %s\n", addr);
        handle_exception(LOAD_FILE_FAILURE_EXCEPTION);
        return 0;
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

/**
 * @brief Execute the instruction at `c8->pc`
 *
 * This function parses and executes the instruction at the current program
 * counter.
 *
 * If verbose flag is set, this will print the instruction to `stdout` as well.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @return amount to increase PC or exception code
 */
static int parse_instruction(c8_t* c8) {
    uint16_t in = (((uint16_t)c8->mem[c8->pc]) << 8) | c8->mem[c8->pc + 1];
    C8_EXPAND(in);

    if (VERBOSE(c8)) {
        printf("%04x: %s\n", c8->pc, c8_decode_instruction(in, NULL));
    }

    switch (a) {
    case 0x0:
        if (y == 0xC) {
            /* SCD n */
            c8->display.y += b;
            if (c8->display.y > C8_HIGH_DISPLAY_HEIGHT) {
                c8->display.y -= C8_HIGH_DISPLAY_HEIGHT;
            }
            return 2;
        }
        switch (kk) {
        case 0xE0:
            /* CLS */
            memset(&c8->display.p, 0,
                C8_HIGH_DISPLAY_WIDTH * C8_HIGH_DISPLAY_HEIGHT);
            return 2;
        case 0xEE:
            /* RET */
            c8->sp--;
            c8->pc = c8->stack[c8->sp];
            return 2;
        case 0xFB:
            /* SCR */
            c8->display.x += 4;
            if (c8->display.x > C8_HIGH_DISPLAY_WIDTH) {
                c8->display.x -= C8_HIGH_DISPLAY_WIDTH;
            }
            return 2;
        case 0xFC:
            /* SCL */
            c8->display.x -= 4;
            if (c8->display.x < 0) {
                c8->display.x += C8_HIGH_DISPLAY_WIDTH;
            }
            return 2;
        case 0xFD: c8->running = 0; return 0; // EXIT
        case 0xFE: c8->display.mode = C8_DISPLAYMODE_LOW; return 2; // LOW
        case 0xFF: c8->display.mode = C8_DISPLAYMODE_HIGH; return 2; // HIGH
        }
        break;
    case 0x1: c8->pc = nnn; return 0; // JP nnn
    case 0x2:
        /* CALL nnn */
        if (c8->sp == 15) {
            sprintf(c8_exception, "PC: %03x", c8->pc);
            return STACK_OVERFLOW_EXCEPTION;
        }
        c8->stack[c8->sp] = c8->pc;
        c8->sp++;
        c8->pc = nnn;
        return 0;
    case 0x3:
        /* SE Vx, kk */
        if (c8->V[x] == kk) {
            c8->pc += 2;
        }
        return 2;
    case 0x4:
        /* SNE Vx, kk */
        if (c8->V[x] != kk) {
            c8->pc += 2;
        }
        return 2;
    case 0x5:
        /* SE Vx, Vy */
        if (c8->V[x] == c8->V[y]) {
            c8->pc += 2;
        }
        return 2;
    case 0x6: c8->V[x] = kk; return 2; // LD Vx, kk
    case 0x7:
        /* ADD Vx, kk */
        c8->V[0xF] = CARRIES(c8->V[x], kk);
        c8->V[x] += kk;
        return 2;
    case 0x8:
        switch (b) {
        case 0x0: c8->V[x] = c8->V[y]; return 2; // LD Vx, Vy
        case 0x1:
            /* OR Vx, Vy */
            c8->V[x] |= c8->V[y];
            QUIRK_BITWISE(c8);
            return 2;
        case 0x2:
            /* AND Vx, Vy */
            c8->V[x] &= c8->V[y];
            QUIRK_BITWISE(c8);
            return 2;
        case 0x3:
            /* XOR Vx, Vy */
            c8->V[x] = c8->V[x] ^ c8->V[y];
            QUIRK_BITWISE(c8);
            return 2;
        case 0x4:
            /* ADD Vx, Vy */
            c8->V[0xF] = CARRIES(c8->V[x], c8->V[y]);
            c8->V[x] += c8->V[y];
            return 2;
        case 0x5:
            /* SUB Vx, Vy */
            c8->V[0xF] = !BORROWS(c8->V[x], c8->V[y]);
            c8->V[x] -= c8->V[y];
            return 2;
        case 0x6:
            /* SHR Vx, Vy */
            QUIRK_SHIFT(c8);
            c8->V[x] = c8->V[y] >> 1;
            c8->V[0xF] = c8->V[x] & 0x1;
            return 2;
        case 0x7:
            /* SUBN Vx, Vy */
            c8->V[0xF] = !BORROWS(c8->V[y], c8->V[x]);
            c8->V[x] = c8->V[y] - c8->V[x];
            return 2;
        case 0xE:
            /* SHL Vx, Vy */
            QUIRK_SHIFT(c8);
            c8->V[x] = c8->V[y] << 1;
            c8->V[0xF] = (c8->V[x] >> 7) & 1;
            return 2;
        }
        break;
    case 0x9:
        /* SNE Vx, Vy */
        if (c8->V[x] != c8->V[y]) {
            c8->pc += 2;
        }
        return 2;
    case 0xA:
        /* LD I, nnn */
        c8->I = nnn;
        return 2;
    case 0xB:
        /* JP V0, nnn */
        if (c8->flags & C8_FLAG_QUIRK_JUMP) {
            c8->pc = nnn + c8->V[(nnn >> 8) & 0xF];
        }
        else {
            c8->pc = nnn + c8->V[0];
        }
        return 0;
    case 0xC: c8->V[x] = rand() & kk; return 2; // RND Vx, kk
    case 0xD: draw(c8, in); return 2; // DRW Vx, Vy, b
    case 0xE:
        if (kk == 0x9E) {
            /* SKP Vx */
            if (c8->key[c8->V[x]]) {
                c8->pc += 2;
            }
        }
        else if (kk == 0xA1) {
            /* SKNP Vx */
            if (!c8->key[c8->V[x]]) {
                c8->pc += 2;
            }
        }
        return 2;
    case 0xF:
        switch (kk) {
        case 0x07: c8->V[x] = c8->dt; return 2; // LD Vx, DT
        case 0x0A:
            /* LD Vx, K */
            c8->VK = x;
            c8->waitingForKey = 1;
            return 2;
        case 0x15: c8->dt = c8->V[x]; return 2; // LD DT, Vx
        case 0x18: c8->st = c8->V[x]; return 2; // LD ST, Vx
        case 0x1E: c8->I += c8->V[x]; return 2; // ADD I, Vx
        case 0x29:
            /* LD F, Vx */
            c8->I = C8_FONT_START + (c8->V[x] * 5);
            return 2;
        case 0x30:
            /* LD HF, Vx */
            c8->I = C8_HIGH_FONT_START + (c8->V[x] * 10);
            return 2;
        case 0x33:
            /* LD B, Vx */
            c8->mem[c8->I] = (c8->V[x] / 100) % 10; // hundreds
            c8->mem[c8->I + 1] = (c8->V[x] / 10) % 10; // tens
            c8->mem[c8->I + 2] = c8->V[x] % 10; // ones
            return 2;
        case 0x55:
            /* LD [I], Vx */
            for (int i = 0; i < x; i++) {
                c8->mem[c8->I + i] = c8->V[i];
            }
            QUIRK_LOADSTORE(c8);
            return 2;
        case 0x65:
            /* LD Vx, [I] */
            for (int i = 0; i < x; i++) {
                c8->V[i] = c8->mem[c8->I + i];
            }
            QUIRK_LOADSTORE(c8);
            return 2;
        case 0x75:
            /* LD R, Vx */
            for (int i = 0; i < x; i++) {
                c8->R[i] = c8->V[i];
            }
            return 2;
        case 0x85:
            /* LD Vx, R */
            for (int i = 0; i < x; i++) {
                c8->V[i] = c8->R[i];
            }
            return 2;
        }
    }

    return 2;
}

static void draw(c8_t* c8, uint16_t in) {
    C8_EXPAND(in);

    c8->V[0xF] = 0;
    int dw = C8_LOW_DISPLAY_WIDTH;
    int dh = C8_LOW_DISPLAY_HEIGHT;
    int h = 8;
    if (c8->display.mode == C8_DISPLAYMODE_HIGH) {
        if (b == 0) {
            b = 16;
        }
        dw = C8_HIGH_DISPLAY_WIDTH;
        dh = C8_HIGH_DISPLAY_HEIGHT;
    }
    for (int i = 0; i < b; i++) {
        for (int j = 0; j < h; j++) {
            int dx = (c8->V[x] + j) % dw;
            int dy = (c8->V[y] + i) % dh;

            if (c8->flags & C8_FLAG_QUIRK_DRAW) {
                if (((dx % dw) + b >= dw) || (dy % dh) + h >= dh) {
                    continue;
                }
            }

            int before = *c8_get_pixel(&c8->display, dx, dy);
            int pix = c8->mem[c8->I + i];

            if (pix & (0x80 >> j)) {
                if (before) {
                    c8->V[0xF] = 1;
                }
                c8_get_pixel(&c8->display, dx, dy)[0] ^= 1;
            }
        }
    }

    c8->draw = 1;
}
