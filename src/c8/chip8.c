#include "chip8.h"

#include "debug.h"
#include "util/decode.h"
#include "util/util.h"

#include <stdio.h>
#include <stdlib.h>

#define DEBUG(c) (c->flags & FLAG_DEBUG)
#define VERBOSE(c) (c->flags & FLAG_VERBOSE)
#define BORROWS(x, y) ((((int) x) - y) < 0)
#define CARRIES(x, y) ((((int) x) + y) > UINT8_MAX)

static void init_font(chip8_t *);
static int load_rom(chip8_t *, const char *);
static void parse_instruction(chip8_t *);

uint16_t font[] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0x90, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80, // F
};

void deinit_chip8(chip8_t *c8) {
	deinit_graphics();
	free(c8);
}

/**
 * @brief Initialize and return a `chip8_t` with the specified clockspeed.
 * 
 * This function allocates memory for a new `chip8_t` with all values set to 0,
 * sets the clockspeed to `cs`, adds the font to memory, and returns a pointer
 * to it.
 * 
 * @param cs clockspeed
 * 
 * @return pointer to initialized chip8_t (must be `free`'d). `NULL` if failed.
 */
chip8_t *init_chip8(int cs, int flags, const char *path) {
	chip8_t *c8;
	if (!(c8 = (chip8_t *) calloc(1, sizeof(chip8_t)))) {
		return NULL;
	}

	c8->flags = flags;
	c8->cs = cs;
	init_font(c8);

	if (!load_rom(c8, path)) {
		fprintf(stderr, "Error: Failed to load ROM.\n");
		free(c8);
		return NULL;
	}

	if (!init_graphics()) {
		fprintf(stderr, "Error: Failed to initialize graphics.\n");
		free(c8);
		return NULL;
	}

	return c8;
}

/**
 * @brief Main VM simulation loop. Exits when `running` is 0.
 * 
 * @param c8 the `chip8_t` to simulate
 */
void simulate(chip8_t * c8) {
	int t;
	int debugRet;
	int step = 0;

	c8->pc = PROG_START;
	c8->running = 1;

	if (DEBUG(c8)) {
		debugRet = debug_repl(c8);
	}
	while (c8->running) {
		t = tick(c8->key, c8->cs);

		if (DEBUG(c8) && (has_breakpoint(c8->pc) || step)) {
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

		if (t == -2) {
			/* Quit */
			c8->running = 0;
		}

		if (t >= 0 && c8->waitingForKey) {
			/* Waiting for key and a key was pressed */
			c8->V[c8->VK] = t;
			c8->waitingForKey = 0;
		}

		if (!c8->waitingForKey) {
			/* Not waiting for key, parse next instruction */
			c8->pc += 2;
			parse_instruction(c8);
		}
	}
}

/**
 * @brief Add the font to `c8->mem`.
 * 
 * @param c8 `chip8_t` to add the font to
 */
static void init_font(chip8_t *c8) {
	for (int i = 0; i < (0x10 * 5); i++) {
		c8->mem[FONT_START + i] = font[i];
	}
}

/**
 * @brief Load a ROM to `c8->mem` at path `addr`.
 * 
 * @param c8 `chip8_t` to store the ROM's contents
 * @param addr path to the ROM
 * 
 * @return 0 if failed, 1 otherwise.
 */
static int load_rom(chip8_t *c8, const char *addr) {
	FILE *f;
	int size;
	
	if (!(f = fopen(addr, "r"))) {
		/* Can't open file */
		return 0;
	}

	/* Get file size */
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	if (ftell(f) > (0x1000 - 0x200)) {
		/* File is too big, failure */
		fclose(f);
		return 0;
	}
	rewind(f);

	/* Read the file into memory */
	fread(c8->mem + PROG_START, size, 1, f);
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
 * @param c8 the `chip8_t` to execute the instruction from
 */
static void parse_instruction(chip8_t *c8) {
	uint16_t in = (((uint16_t) c8->mem[c8->pc]) << 8) | c8->mem[c8->pc + 1];
	EXPAND(in);

	if (VERBOSE(c8)) {
		printf("%s\n", decode_instruction(in, NULL));
	}

	switch (a) {
		case 0x0:
			if (y == 0xC) {
				/* SCD n */
				c8->display.y += b;
				break;
			}
			switch (kk) {
				case 0xE0:
					/* CLS */
					for (int i = 0; i < EXTENDED_DISPLAY_WIDTH; i++) {
						for (int j = 0; j < EXTENDED_DISPLAY_HEIGHT; j++) {
							*get_pixel(&c8->display, x, y) = 0;
						}
					}
					break;
				case 0xEE:
					/* RET */
					c8->sp--;
					c8->pc = c8->stack[c8->sp];
					break;
				case 0xFB:
					/* SCR */
					c8->display.x += 4;
					break;
				case 0xFC:
					/* SCL */
					c8->display.x -= 4;
					break;
				case 0xFD:
					/* EXIT */
					c8->running = 0;
					break;
				case 0xFE:
					/* LOW */
					c8->display.mode = DISPLAY_STANDARD;
					break;
				case 0xFF:
					/* HIGH */
					c8->display.mode = DISPLAY_EXTENDED;
					break;
			}
			break;
		case 0x1:
			/* JP nnn */
			c8->pc = nnn;
			break;
		case 0x2:
			/* CALL nnn */
			c8->stack[c8->sp] = c8->pc;
			c8->sp++;
			c8->pc = nnn;
			break;
		case 0x3:
			/* SE Vx, kk */
			if (c8->V[x] == kk) {
				c8->pc += 2;
			}
			break;
		case 0x4:
			/* SNE Vx, kk */
			if (c8->V[x] != kk) {
				c8->pc += 2;
			}
			break;
		case 0x5:
			/* SE Vx, Vy */
			if (c8->V[x] == c8->V[y]) {
				c8->pc += 2;
			}
			break;
		case 0x6:
			/* LD Vx, kk */
			c8->V[x] = kk;
			break;
		case 0x7:
			/* ADD Vx, kk */
			c8->V[0xF] = CARRIES(c8->V[x], kk);
			c8->V[x] += kk;
			break;
		case 0x8:
			switch (b) {
				case 0x0:
					/* LD Vx, Vy */
					c8->V[x] = c8->V[y];
					break;
				case 0x1:
					/* OR Vx, Vy */
					c8->V[x] = c8->V[x] | c8->V[y];
					break;
				case 0x2:
					/* AND Vx, Vy */
					c8->V[x] = c8->V[x] & c8->V[y];
					break;
				case 0x3:
					/* XOR Vx, Vy */
					c8->V[x] = c8->V[x] ^ c8->V[y];
					break;
				case 0x4:
					/* ADD Vx, Vy */
					c8->V[0xF] = CARRIES(c8->V[x], c8->V[y]);
					c8->V[x] += c8->V[y];
					break;
				case 0x5:
					/* SUB Vx, Vy */
					c8->V[0xF] = !BORROWS(c8->V[x], c8->V[y]);
					c8->V[x] -= c8->V[y];
					break;
				case 0x6:
					/* SHR Vx */
					c8->V[x] = c8->V[x] >> 1;
					c8->V[0xF] = c8->V[x] & 0x1;
					c8->V[x] /= 2;
					break;
				case 0x7:
					/* SUBN Vx, Vy */
					c8->V[0xF] = BORROWS(c8->V[y], c8->V[x]) == 0 ? 1 : 0;
					c8->V[x] = c8->V[y] - c8->V[x];
					break;
				case 0xE:
					/* SHL Vx */
					c8->V[x] = c8->V[x] << 1;
					c8->V[0xF] = (c8->V[x] & 0x40) >> 7;
					c8->V[x] *= 2;
					break;
			}
			break;
		case 0x9:
			/* SNE Vx, Vy */
			if (c8->V[x] != c8->V[y])
				c8->pc += 2;
			break;
		case 0xA:
			/* LD I, nnn */
			c8->I = nnn;
			break;
		case 0xB:
			/* JP V0, nnn */
			c8->pc = nnn + c8->V[0] - 2;
			break;
		case 0xC:
			/* RND Vx, kk */
			c8->V[x] = rand() & kk;
			break;
		case 0xD:
			/* DRW Vx, Vy, b */
			c8->V[0xF] = 0;
			int width = STANDARD_DISPLAY_WIDTH;
			int height = STANDARD_DISPLAY_HEIGHT;
			int sh = 8;
			if (b == 0 && c8->display.mode == DISPLAY_EXTENDED) {
				b = 16;
				sh = 16;
				width = EXTENDED_DISPLAY_WIDTH;
				height = EXTENDED_DISPLAY_HEIGHT;
			}
			for (int i = 0; i < b; i++) {
				for (int j = 0; j < sh; j++) {
					int dx = (c8->V[x] + j) % width;
					int dy = (c8->V[y] + i) % height;

					int before = *get_pixel(&c8->display, dx, dy);
					if ((c8->mem[c8->I + i] >> (7 - j)) & 1) {
						*get_pixel(&c8->display, dx, dy) ^= 1;
					}

					if (before != *get_pixel(&c8->display, dx, dy)) {
						c8->V[0xF] = 1;
					}
				}
			}

			render(&c8->display);
			break;
		case 0xE:
			if (kk == 0x9E) {
				/* SKP Vx */
				if (c8->key[c8->V[x]]) {
					c8->pc += 2;
				}
			} else if (kk == 0xA1) {
				/* SKNP Vx */
				if (!c8->key[c8->V[x]]) {
					c8->pc += 2;
				}
			}
			break;
		case 0xF:
			switch (kk) {
				case 0x07:
					/* LD Vx, dt */
					c8->V[x] = c8->dt;
					break;
				case 0x0A:
					/* LD Vx, K */
					c8->VK = x;
					c8->waitingForKey = 1;
					break;
				case 0x15:
					/* LD DT, Vx */
					c8->dt = c8->V[x];
					break;
				case 0x18:
					/* LD ST, Vx */
					c8->st = c8->V[x];
					break;
				case 0x1E:
					/* ADD I, Vx */
					c8->I += c8->V[x];
					break;
				case 0x29:
					/* LD F, Vx */
					c8->I = FONT_START + (c8->V[x] * 5);
					break;
				case 0x33:
					/* LD B, Vx */
					c8->mem[c8->I] = (c8->V[x] / 100) % 10; // hundreds
					c8->mem[c8->I + 1] = (c8->V[x] / 10) % 10; // tens
					c8->mem[c8->I + 2] = c8->V[x] % 10; // ones
					break;
				case 0x55:
					/* LD [I], Vx */
					for (int i = 0; i < x; i++) {
						c8->mem[c8->I + i] = c8->V[i];
					}
					break;
				case 0x65:
					/* LD Vx, [I] */
					for (int i = 0; i < x; i++) {
						c8->V[i] = c8->mem[c8->I + i];
					}
					break;
				case 0x75:
					/* LD R, Vx */
					for (int i = 0; i < x; i++) {
						c8->R[i] = c8->V[i];
					}
					break;
				case 0x85:
					/* LD Vx, R */
					for (int i = 0; i < x; i++) {
						c8->V[i] = c8->R[i];
					}
					break;
			}
	}

	if (c8->dt > 0) {
		c8->dt--;
	}

	if (c8->st > 0) {
		c8->st--; // TODO sound
	}
}

