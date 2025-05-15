#include "chip8.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "decode.h"
#include "graphics.h"
#include "util.h"

int running = 0;
int debug = 0;

uint8_t mem[0x1000], V[16];
uint8_t sp = 0, dt = 0, st = 0;
uint16_t stack[16];
uint16_t pc = 0x200, I = 0;
int key[0x10];
int keyRegister;
int *display;
int clockSpeed = CLOCK_SPEED;
int waitingForKey = 0;

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

int check_borrow(int, int);
int check_carry(int, int);
void init_font(void);
int load_rom(const char *);
void simulate(void);

int check_borrow(int x, int y) {
	return (((int) x) - y) < 0;
}

int check_carry(int x, int y) {
	return (((int) x) + y) > UINT8_MAX;
}

void init_font(void) {
	for (int i = 0; i < (0x10 * 5); i++) {
		mem[FONT_START + i] = font[i];
	}
}

int load_rom(const char *addr) {
	FILE *f = fopen(addr, "r");
	int size;

	if (!f)
		return 0;

	/* get filesize */
	fseek(f, 0, SEEK_END);
	size = ftell(f);

	if (ftell(f) > (0x1000 - 0x200)) {
		fclose(f);
		return 0;
	}

	rewind(f);
	fread(mem + 0x200, size, 1, f);
	fclose(f);
	return 1;
}

void parse_instruction(uint16_t in) {
	int x = (in & 0x0F00) >> 8;
	int kk = in & 0x00FF;
	int y = (in & 0x00F0) >> 4;
	int nnn = in & 0x0FFF;
	int a = (in & 0xF000) >> 12;
	int b = in & 0x000F;

	switch (a) {
		case 0x0:
			if (in == 0x00E0) {
				/* CLS */
				for (int i = 0; i < DISPLAY_WIDTH; i++) {
					for (int j = 0; j < DISPLAY_HEIGHT; j++) {
						*get_pixel(display, x, y) = 0;
					}
				}
			} else if (in == 0x00EE) {
				/* RET */
				sp--;
				pc = stack[sp];
			}
			break;
		case 0x1:
			/* JP addr */
			pc = nnn;
			break;
		case 0x2:
			/* CALL addr */
			stack[sp] = pc;
			sp++;
			pc = nnn;
			break;
		case 0x3:
			/* SE Vx, byte */
			if (V[x] == kk)
				pc += 2;
			break;
		case 0x4:
			/* SNE Vx, byte */
			if (V[x] != kk)
				pc += 2;
			break;
		case 0x5:
			/* SE Vx, Vy */
			if (V[x] == V[y])
				pc += 2;
			break;
		case 0x6:
			/* LD Vx, byte */
			V[x] = kk;
			break;
		case 0x7:
			/* ADD Vx, byte */
			V[0xF] = check_carry(V[x], kk);
			V[x] += kk;
			break;
		case 0x8:
			switch (b) {
				case 0x0:
					/* LD Vx, Vy */
					V[x] = V[y];
					break;
				case 0x1:
					/* OR Vx, Vy */
					V[x] = V[x] | V[y];
					break;
				case 0x2:
					/* AND Vx, Vy */
					V[x] = V[x] & V[y];
					break;
				case 0x3:
					/* XOR Vx, Vy */
					V[x] = V[x] ^ V[y];
					break;
				case 0x4:
					/* ADD Vx, Vy */
					V[0xF] = check_carry(V[x], V[y]);
					V[x] += V[y];
					break;
				case 0x5:
					/* SUB Vx, Vy */
					V[0xF] = !check_borrow(V[x], V[y]);
					V[x] -= V[y];
					break;
				case 0x6:
					/* SHR Vx */
					V[x] = V[x] >> 1;
					V[0xF] = V[x] & 0x1;
					V[x] /= 2;
					break;
				case 0x7:
					/* SUBN Vx, Vy */
					V[0xF] = check_borrow(V[y], V[x]) == 0 ? 1 : 0;
					V[x] = V[y] - V[x];
					break;
				case 0xE:
					/* SHL Vx */
					V[x] = V[x] << 1;
					V[0xF] = (V[x] & 0x40) >> 7;
					V[x] *= 2;
					break;
			}
			break;
		case 0x9:
			/* SNE Vx, Vy */
			if (V[x] != V[y])
				pc += 2;
			break;
		case 0xA:
			/* LD I, addr */
			I = nnn;
			break;
		case 0xB:
			/* JP V0, addr */
			pc = nnn + V[0] - 2;
			break;
		case 0xC:
			/* RND Vx, byte */
			V[x] = rand() & kk;
			break;
		case 0xD:
			/* DRW Vx, Vy, b */
			V[0xF] = 0;
			for (int i = 0; i < b; i++) {
				for (int j = 0; j < 8; j++) {
					int dx = V[x] + j;
					int dy = V[y] + i;

					while (dx >= DISPLAY_WIDTH)
						dx -= DISPLAY_WIDTH;
					while (dy >= DISPLAY_HEIGHT)
						dy -= DISPLAY_HEIGHT;

					int before = *get_pixel(display, dx, dy);
					if ((mem[I + i] >> (7 - j)) & 1) {
						*get_pixel(display, dx, dy) ^= 1;
					}

					if (before != *get_pixel(display, dx, dy))
						V[0xF] = 1;
				}
			}
			render(display);
			break;
		case 0xE:
			if (kk == 0x9E) {
				/* SKP Vx */
				if (key[V[x]])
					pc += 2;
			} else if (kk == 0xA1) {
				/* SKNP Vx */
				if (!key[V[x]])
					pc += 2;
			}
			break;
		case 0xF:
			switch (kk) {
				case 0x07:
					/* LD Vx, DT */
					V[x] = dt;
					break;
				case 0x0A:
					/* LD Vx, K */
					keyRegister = x;
					waitingForKey = 1;
					break;
				case 0x15:
					/* LD DT, Vx */
					dt = V[x];
					break;
				case 0x18:
					/* LD ST, Vx */
					st = V[x];
					break;
				case 0x1E:
					/* ADD I, Vx */
					I += V[x];
					break;
				case 0x29:
					/* LD F, Vx */
					I = FONT_START + (V[x] * 5);
					break;
				case 0x33:
					/* LD B, Vx */
					mem[I] = (V[x] / 100) % 10; // hundreds
					mem[I + 1] = (V[x] / 10) % 10; // tens
					mem[I + 2] = V[x] % 10; // ones
					break;
				case 0x55:
					/* LD [I], Vx */
					for (int i = 0; i < x; i++)
						mem[I + i] = V[i];
					break;
				case 0x65:
					/* LD Vx, [I] */
					for (int i = 0; i < x; i++)
						V[i] = mem[I + i];
					break;
			}
	}

	if (dt > 0)
		dt--;
	if (st > 0)
		st--;
}

void print_debug(void) {
	uint16_t in = (mem[pc] << 8) | mem[pc + 1];
	char *decoded = decode_instruction(in, NULL);
	printf("INSTRUCTION: %04x\t%s\nPC: %03x\nSP: %02x\nDT: %02x\nST: %02x\n"
		   "I: %04x\n", in, decoded, pc, sp, dt, st, I);
	for (int i = 0; i < 0x10; i++) {
		printf("V%01x: %02x\n", i, V[i]);
	}

	// TODO dump mem to file
}

void simulate(void) {
	int t;
	uint16_t in = 0;

	running = 1;
	while (running) {
		t = tick(key, clockSpeed);
		if (t == -2) {
			running = false;
		}

		if (t >= 0 && waitingForKey) {
			V[keyRegister] = t;
			waitingForKey = 0;
		}

		if (!waitingForKey) {
			in = ((uint16_t) mem[pc]) << 8 | mem[pc + 1];
			pc += 2;
		}

		parse_instruction(in);
	}
}

int main(int argc, char *argv[]) {
	int opt;
	while ((opt = getopt(argc, argv, "c:d")) != -1) {
		switch (opt) {
			case 'c':
				clockSpeed = atoi(optarg);
				break;
			case 'd':
				debug = 1;
				break;
			default:
				  fprintf(stderr, "Usage: %s [-d] [-c clockspeed] file", argv[0]);
				  exit(EXIT_FAILURE);
		}
	}

	srand(time(NULL));

	if (!init_graphics()) {
		fprintf(stderr, "Failed to initialize graphics");
		exit(EXIT_FAILURE);
	}

	display = malloc(sizeof(int) * DISPLAY_WIDTH * DISPLAY_HEIGHT);
	if (!display) {
		fprintf(stderr, "Failed to allocate display memory\n");
		exit(EXIT_FAILURE);
	}

	if (!load_rom(argv[optind])) {
		fprintf(stderr, "Failed to load rom file\n");
		exit(EXIT_FAILURE);
	}

	init_font();
	simulate();

	deinit_graphics();
	free(display);

	return EXIT_SUCCESS;
}
