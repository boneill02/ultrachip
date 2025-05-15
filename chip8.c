#include "chip8.h"

#include "decode.h"
#include "graphics.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int check_borrow(int, int);
int check_carry(int, int);
chip8_t *init_chip8(int);
void init_font(chip8_t *);
int load_rom(chip8_t *, const char *);
void parse_instruction(chip8_t *, uint16_t);
void simulate(chip8_t *c8);

int running = 0;
int debug = 0;
int verbose = 0;

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

int check_borrow(int x, int y) {
	return (((int) x) - y) < 0;
}

int check_carry(int x, int y) {
	return (((int) x) + y) > UINT8_MAX;
}

chip8_t *init_chip8(int cs) {
	chip8_t *c8;
	if (!(c8 = (chip8_t *) calloc(1, sizeof(chip8_t)))) {
		return NULL;
	}

	c8->cs = cs;
	init_font(c8);
	return c8;
}

void init_font(chip8_t *c8) {
	for (int i = 0; i < (0x10 * 5); i++) {
		c8->mem[FONT_START + i] = font[i];
	}
}

int load_rom(chip8_t *c8, const char *addr) {
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

void parse_instruction(chip8_t *c8, uint16_t in) {
	int x = (in & 0x0F00) >> 8;
	int kk = in & 0x00FF;
	int y = (in & 0x00F0) >> 4;
	int nnn = in & 0x0FFF;
	int a = (in & 0xF000) >> 12;
	int b = in & 0x000F;

	if (verbose) {
		printf("%s\n", decode_instruction(in, NULL));
	}

	switch (a) {
		case 0x0:
			if (in == 0x00E0) {
				/* CLS */
				for (int i = 0; i < DISPLAY_WIDTH; i++) {
					for (int j = 0; j < DISPLAY_HEIGHT; j++) {
						*get_pixel(c8->display, x, y) = 0;
					}
				}
			} else if (in == 0x00EE) {
				/* RET */
				c8->sp--;
				c8->pc = c8->stack[c8->sp];
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
			c8->V[0xF] = check_carry(c8->V[x], kk);
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
					c8->V[0xF] = check_carry(c8->V[x], c8->V[y]);
					c8->V[x] += c8->V[y];
					break;
				case 0x5:
					/* SUB Vx, Vy */
					c8->V[0xF] = !check_borrow(c8->V[x], c8->V[y]);
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
					c8->V[0xF] = check_borrow(c8->V[y], c8->V[x]) == 0 ? 1 : 0;
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
			for (int i = 0; i < b; i++) {
				for (int j = 0; j < 8; j++) {
					int dx = c8->V[x] + j;
					int dy = c8->V[y] + i;

					while (dx >= DISPLAY_WIDTH) {
						dx -= DISPLAY_WIDTH;
					}
					while (dy >= DISPLAY_HEIGHT) {
						dy -= DISPLAY_HEIGHT;
					}

					int before = *get_pixel(c8->display, dx, dy);
					if ((c8->mem[c8->I + i] >> (7 - j)) & 1) {
						*get_pixel(c8->display, dx, dy) ^= 1;
					}

					if (before != *get_pixel(c8->display, dx, dy)) {
						c8->V[0xF] = 1;
					}
				}
			}
			render(c8->display);
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
			}
	}

	if (c8->dt > 0) {
		c8->dt--;
	}

	if (c8->st > 0) {
		c8->st--;
	}
}

void simulate(chip8_t * c8) {
	uint16_t in;
	int t;

	running = 1;
	while (running) {
		t = tick(c8->key, c8->cs);
		if (t == -2) {
			/* Quit */
			running = false;
		}

		if (t >= 0 && c8->waitingForKey) {
			/* Waiting for key and a key was pressed */
			c8->V[c8->VK] = t;
			c8->waitingForKey = 0;
		}

		if (!c8->waitingForKey) {
			/* Not waiting for key, parse next instruction */
			in = ((uint16_t) c8->mem[c8->pc]) << 8 | c8->mem[c8->pc + 1];
			c8->pc += 2;
			parse_instruction(c8, in);
		}
	}
}

int main(int argc, char *argv[]) {
	int cs = CLOCK_SPEED;
	int opt;
	chip8_t *c8;

	/* Parse args */
	while ((opt = getopt(argc, argv, "c:dv")) != -1) {
		switch (opt) {
			case 'c':
				cs = atoi(optarg);
				break;
			case 'd':
				debug = 1;
				break;
			case 'v':
				verbose = 1;
				break;
			default:
			  fprintf(stderr, "Usage: %s [-dv] [-c clockspeed] file\n", argv[0]);
			  exit(EXIT_FAILURE);
		}
	}

	srand(time(NULL));

	if (!(c8 = init_chip8(cs))) {
		fprintf(stderr, "Error: Failed to allocate memory for CHIP-8 state.\n");
		return EXIT_FAILURE;
	}

	if (!load_rom(c8, argv[optind])) {
		fprintf(stderr, "Error: Failed to load ROM.\n");
		free(c8);
		return EXIT_FAILURE;
	}

	if (!init_graphics()) {
		fprintf(stderr, "Error: Failed to initialize graphics.\n");
		free(c8);
		return EXIT_FAILURE;
	}

	simulate(c8);

	deinit_graphics();
	free(c8);
	return EXIT_SUCCESS;
}
