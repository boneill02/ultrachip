#include <stdint.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 400
#define WINDOW_SCALE_X DEFAULT_WINDOW_WIDTH / DISPLAY_WIDTH
#define WINDOW_SCALE_Y DEFAULT_WINDOW_HEIGHT / DISPLAY_HEIGHT

int display[DISPLAY_WIDTH][DISPLAY_HEIGHT];
int running = 0;

uint8_t mem[0xFFF];
uint8_t V[16];
uint16_t pc = 0x200;
uint16_t sp = 0, dt = 0, st = 0;

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Rect pix = {
	.x = 0,
	.y = 0,
	.w = WINDOW_SCALE_X,
	.h = WINDOW_SCALE_Y,
};
SDL_Rect winRect = {
	.x = 0,
	.y = 0,
	.w = DEFAULT_WINDOW_WIDTH,
	.h = DEFAULT_WINDOW_HEIGHT,
};

int
check_carry(int x, int y)
{
	return (((int) x) + y) > UINT8_MAX;
}

int
check_borrow(int x, int y)
{
	return (((int) x) - y) < 0;
}

void
draw(int x, int y)
{
	display[x][y] = true;
}

void
render(void)
{
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	for (int i = 0; i < DISPLAY_WIDTH; i++) {
		for (int j = 0; j < DISPLAY_HEIGHT; j++) {
			if (display[i][j]) {
				pix.x = i * WINDOW_SCALE_X;
				pix.y = j * WINDOW_SCALE_Y;
				SDL_RenderFillRect(renderer, &pix);
			}
		}
	}

	SDL_RenderPresent(renderer);
}

void
parse_instruction(uint16_t in)
{
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
						display[i][j] = false;
					}
				}
			} else if (in == 0x00EE) {
				/* RET */
				pc = mem[sp];
				sp -= 1;
			}
			break;
		case 0x1:
			/* JP addr */
			pc = nnn;
			break;
		case 0x2:
			/* CALL addr */
			sp++;
			mem[sp] = pc;
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
					V[0xF] = check_borrow(V[x], V[y]) == 0 ? 1 : 0;
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
			I = mem[nnn];
			break;
		case 0xB:
			/* JP V0, addr */
			pc = nnn + V[0];
			break;
		case 0xC:
			/* RND Vx, byte */
			// TODO implement
			break;
		case 0xD:
			/* DRW Vx, Vy, n */
			// TODO implement
			break;
		case 0xE:
			if (kk == 0x9E) {
				/* SKP Vx */
				// TODO implement
			} else if (kk == 0xA1) {
				/* SKNP Vx */
				// TODO implement
			}
			break;
		case 0xF:
			switch (kk) {
				case 0x07:
					/* LD Vx, DT */
					// TODO implement
					break;
				case 0x0A:
					/* LD Vx, K */
					// TODO implement
					break;
				case 0x15:
					/* LD DT, Vx */
					// TODO implement
					break;
				case 0x18:
					/* LD ST, Vx */
					// TODO implement
					break;
				case 0x1E:
					/* ADD I, Vx */
					I += V[x];
					break;
				case 0x29:
					/* LD F, Vx */
					// TODO implement
					break;
				case 0x33:
					/* LD B, Vx */
					// TODO implement
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
}

int
main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	window = SDL_CreateWindow("CHIP8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	SDL_Event e;

	running = 1;
	while (running) {
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_QUIT:
					running = false;	
					break;
			}
		}
	}

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}