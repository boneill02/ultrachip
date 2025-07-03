#include "c8/private/instruction.h"

#include "c8/decode.h"
#include "c8/defs.h"
#include "c8/font.h"
#include "c8/private/exception.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define VERBOSE(c) (c->flags & C8_FLAG_VERBOSE)

#define SCHIP_EXCLUSIVE(c) \
    if (c->mode == C8_MODE_CHIP8) { \
        C8_EXCEPTION(INVALID_INSTRUCTION_EXCEPTION, "SCHIP instruction detected in CHIP-8 mode.\n"); \
        return INVALID_INSTRUCTION_EXCEPTION; \
    }

#define XOCHIP_EXCLUSIVE(c) \
    if (c->mode != C8_MODE_XOCHIP) { \
        const char *modeStr = (c->mode == C8_MODE_CHIP8) ? "CHIP-8" : "SCHIP"; \
        C8_EXCEPTION(INVALID_INSTRUCTION_EXCEPTION, "XOCHIP instruction detected in %s mode.\n", modeStr); \
        return INVALID_INSTRUCTION_EXCEPTION; \
    }

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

/* instruction groups */
static int base_instruction(c8_t*, uint16_t);
static int bitwise_instruction(c8_t*, uint16_t);
static int key_instruction(c8_t*, uint16_t);
static int misc_instruction(c8_t*, uint16_t);

static inline int i_scd_b(c8_t*, uint8_t);

/* base (00kk) instructions */
static inline int i_cls(c8_t*);
static inline int i_ret(c8_t*);
static inline int i_scr(c8_t*);
static inline int i_scl(c8_t*);
static inline int i_exit(c8_t*);
static inline int i_low(c8_t*);
static inline int i_high(c8_t*);

static inline int i_jp_nnn(c8_t*, uint16_t);
static inline int i_call_nnn(c8_t*, uint16_t);
static inline int i_se_vx_kk(c8_t*, uint8_t, uint8_t);
static inline int i_sne_vx_kk(c8_t*, uint8_t, uint8_t);
static inline int i_se_vx_vy(c8_t*, uint8_t, uint8_t);
static inline int i_ld_vx_kk(c8_t*, uint8_t, uint8_t);
static inline int i_add_vx_kk(c8_t*, uint8_t, uint8_t);

/* bitwise (8xyb) instructions */
static inline int i_ld_vx_vy(c8_t*, uint8_t, uint8_t);
static inline int i_or_vx_vy(c8_t*, uint8_t, uint8_t);
static inline int i_and_vx_vy(c8_t*, uint8_t, uint8_t);
static inline int i_xor_vx_vy(c8_t*, uint8_t, uint8_t);
static inline int i_add_vx_vy(c8_t*, uint8_t, uint8_t);
static inline int i_sub_vx_vy(c8_t*, uint8_t, uint8_t);
static inline int i_shr_vx_vy(c8_t*, uint8_t, uint8_t);
static inline int i_subn_vx_vy(c8_t*, uint8_t, uint8_t);
static inline int i_shl_vx_vy(c8_t*, uint8_t, uint8_t);

static inline int i_sne_vx_vy(c8_t*, uint8_t, uint8_t);
static inline int i_ld_i_nnn(c8_t*, uint16_t);
static inline int i_jp_v0_nnn(c8_t*, uint16_t);
static inline int i_rnd_vx_kk(c8_t*, uint8_t, uint8_t);
static inline int i_drw_vx_vy_b(c8_t*, uint8_t, uint8_t, uint8_t);

/* key (Ex00) instructions */
static inline int i_skp_vx(c8_t*, uint8_t);
static inline int i_sknp_vx(c8_t*, uint8_t);

/* misc (Fxkk) instructions */
static inline int i_ld_vx_dt(c8_t*, uint8_t);
static inline int i_ld_vx_k(c8_t*, uint8_t);
static inline int i_ld_dt_vx(c8_t*, uint8_t);
static inline int i_ld_st_vx(c8_t*, uint8_t);
static inline int i_add_i_vx(c8_t*, uint8_t);
static inline int i_ld_f_vx(c8_t*, uint8_t);
static inline int i_ld_hf_vx(c8_t*, uint8_t);
static inline int i_ld_b_vx(c8_t*, uint8_t);
static inline int i_ld_ip_vx(c8_t*, uint8_t);
static inline int i_ld_vx_ip(c8_t*, uint8_t);
static inline int i_ld_r_vx(c8_t*, uint8_t);
static inline int i_ld_vx_r(c8_t*, uint8_t);


/**
 * @brief Execute the instruction at `c8->pc`
 *
 * This function parses and executes the instruction at the current program
 * counter.
 *
 * If verbose flag is set, this will print the instruction to `stdout` as well.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @return amount to increase the program counter, or an exception code if an
 * error occurs.
 */
int parse_instruction(c8_t* c8) {
    uint16_t in = (((uint16_t)c8->mem[c8->pc]) << 8) | c8->mem[c8->pc + 1];
    C8_EXPAND(in);

    if (VERBOSE(c8)) {
        printf("%04x: %s\n", c8->pc, c8_decode_instruction(in, NULL));
    }

    switch (a) {
    case 0x0: return y == 0xC ? i_scd_b(c8, b) : base_instruction(c8, kk);
    case 0x1: return i_jp_nnn(c8, nnn);
    case 0x2: return i_call_nnn(c8, nnn);
    case 0x3: return i_se_vx_kk(c8, x, kk);
    case 0x4: return i_sne_vx_kk(c8, x, kk);
    case 0x5: return i_se_vx_vy(c8, x, y);
    case 0x6: return i_ld_vx_kk(c8, x, kk);
    case 0x7: return i_add_vx_kk(c8, x, kk);
    case 0x8: return bitwise_instruction(c8, in);
    case 0x9: return i_sne_vx_vy(c8, x, y);
    case 0xA: return i_ld_i_nnn(c8, nnn);
    case 0xB: return i_jp_v0_nnn(c8, nnn);
    case 0xC: return i_rnd_vx_kk(c8, x, kk);
    case 0xD: return i_drw_vx_vy_b(c8, x, y, b);
    case 0xE: return key_instruction(c8, in);
    case 0xF: return misc_instruction(c8, in);
    default: // unreachable
        C8_EXCEPTION(INVALID_INSTRUCTION_EXCEPTION, "Invalid instruction: %04x", in);
        return INVALID_INSTRUCTION_EXCEPTION;
    }
}

static int base_instruction(c8_t* c8, uint16_t in) {
    switch (C8_KK(in)) {
    case 0xE0: return i_cls(c8);
    case 0xEE: return i_ret(c8);
    case 0xFB: return i_scr(c8);
    case 0xFC: return i_scl(c8);
    case 0xFD: return i_exit(c8);
    case 0xFE: return i_low(c8);
    case 0xFF: return i_high(c8);
    default:
        C8_EXCEPTION(INVALID_INSTRUCTION_EXCEPTION, "Invalid instruction: %04x", in);
        return INVALID_INSTRUCTION_EXCEPTION;
    }
}

static int bitwise_instruction(c8_t* c8, uint16_t in) {
    switch (C8_B(in)) {
    case 0x0: return i_ld_vx_vy(c8, C8_X(in), C8_Y(in));
    case 0x1: return i_or_vx_vy(c8, C8_X(in), C8_Y(in));
    case 0x2: return i_and_vx_vy(c8, C8_X(in), C8_Y(in));
    case 0x3: return i_xor_vx_vy(c8, C8_X(in), C8_Y(in));
    case 0x4: return i_add_vx_vy(c8, C8_X(in), C8_Y(in));
    case 0x5: return i_sub_vx_vy(c8, C8_X(in), C8_Y(in));
    case 0x6: return i_shr_vx_vy(c8, C8_X(in), C8_Y(in));
    case 0x7: return i_subn_vx_vy(c8, C8_X(in), C8_Y(in));
    case 0xE: return i_shl_vx_vy(c8, C8_X(in), C8_Y(in));
    default:
        C8_EXCEPTION(INVALID_INSTRUCTION_EXCEPTION, "Invalid instruction: %04x", in);
        return INVALID_INSTRUCTION_EXCEPTION;
    }
}

static int key_instruction(c8_t* c8, uint16_t in) {
    switch (C8_KK(in)) {
    case 0x9E: return i_skp_vx(c8, C8_X(in));
    case 0xA1: return i_sknp_vx(c8, C8_X(in));
    default:
        C8_EXCEPTION(INVALID_INSTRUCTION_EXCEPTION, "Invalid instruction: %04x", in);
        return INVALID_INSTRUCTION_EXCEPTION;
    }
}

static int misc_instruction(c8_t* c8, uint16_t in) {
    switch (C8_KK(in)) {
    case 0x07: return i_ld_vx_dt(c8, C8_X(in));
    case 0x0A: return i_ld_vx_k(c8, C8_X(in));
    case 0x15: return i_ld_dt_vx(c8, C8_X(in));
    case 0x18: return i_ld_st_vx(c8, C8_X(in));
    case 0x1E: return i_add_i_vx(c8, C8_X(in));
    case 0x29: return i_ld_f_vx(c8, C8_X(in));
    case 0x30: return i_ld_hf_vx(c8, C8_X(in));
    case 0x33: return i_ld_b_vx(c8, C8_X(in));
    case 0x55: return i_ld_ip_vx(c8, C8_X(in));
    case 0x65: return i_ld_vx_ip(c8, C8_X(in));
    case 0x75: return i_ld_r_vx(c8, C8_X(in));
    case 0x85: return i_ld_vx_r(c8, C8_X(in));
    default:
        C8_EXCEPTION(INVALID_INSTRUCTION_EXCEPTION, "Invalid instruction: %04x", in);
        return INVALID_INSTRUCTION_EXCEPTION;
    }
}

/**
 * @brief `SCD b` instruction (`00Cb`)
 *
 * This instruction scrolls the display down by `b` pixels.
 *
 * @note This is a SCHIP instruction. `c8` must be in SCHIP or XO-CHIP mode to
 * execute this instruction.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param b the number of pixels to scroll down
 *
 * @return 2, the number of bytes to increase the program counter by,
 * or INVALID_INSTRUCTION_EXCEPTION if `c8` is in CHIP-8 mode.
 */
static inline int i_scd_b(c8_t* c8, uint8_t b) {
    SCHIP_EXCLUSIVE(c8);
    c8->display.y += b;
    if (c8->display.y > C8_HIGH_DISPLAY_HEIGHT) {
        c8->display.y -= C8_HIGH_DISPLAY_HEIGHT;
    }

    c8->draw = 1;
    return 2;
}

/**
 * @brief `CLS` instruction (`00E0`)
 *
 * This instruction clears the display and sets the draw flag.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @return 2, the number of bytes to increase the program counter by.
 */
static inline int i_cls(c8_t* c8) {
    memset(&c8->display.p, 0, C8_HIGH_DISPLAY_WIDTH * C8_HIGH_DISPLAY_HEIGHT);
    c8->draw = 1;
    return 2;
}

/**
 * @brief `RET` instruction (`00EE`)
 *
 * This instruction pops the top of the stack and sets the program counter to it.
 * Usually, the PC should be at a `CALL` instruction after this, so the program
 * counter must be increased by 2.
 *
 * @param c8 the `c8_t` to execute the instruction from
 *
 * @return 2, the number of bytes to increase the program counter by,
 * or STACK_UNDERFLOW_EXCEPTION if the stack is empty.
 */
static inline int i_ret(c8_t* c8) {
    if (c8->sp == 0) {
        C8_EXCEPTION(STACK_UNDERFLOW_EXCEPTION, "Stack underflow at %03x", c8->pc);
        return STACK_UNDERFLOW_EXCEPTION;
    }
    c8->sp--;
    c8->pc = c8->stack[c8->sp];
    return 2;
}

/**
 * @brief `SCR` instruction (`00FB`)
 *
 * This instruction scrolls the display right by 4 pixels. If the current x
 * value is greater than the display width, it wraps around to the left side.
 *
 * @note This is a SCHIP instruction. `c8` must be in SCHIP or XO-CHIP mode to
 * execute this instruction.
 *
 * @param c8 the `c8_t` to execute the instruction from
 *
 * @return 2, the number of bytes to increase the program counter by, or
 * INVALID_INSTRUCTION_EXCEPTION if `c8` is in CHIP-8 mode.
 */
static inline int i_scr(c8_t* c8) {
    SCHIP_EXCLUSIVE(c8);
    c8->display.x += 4;
    if (c8->display.x > C8_HIGH_DISPLAY_WIDTH) {
        c8->display.x -= C8_HIGH_DISPLAY_WIDTH;
    }
    return 2;
}

/**
 * @brief `SCL` instruction (`00FC`)
 *
 * This instruction scrolls the display left by 4 pixels. If the x value ends up
 * less than 0, it wraps around to the right side.
 *
 * @note This is a SCHIP instruction. `c8` must be in SCHIP or XO-CHIP mode to
 * execute this instruction.
 *
 * @param c8 the `c8_t` to execute the instruction from
 *
 * @return 2, the number of bytes to increase the program counter by, or
 * INVALID_INSTRUCTION_EXCEPTION if `c8` is in CHIP-8 mode.
 */
static inline int i_scl(c8_t* c8) {
    SCHIP_EXCLUSIVE(c8);
    if (c8->display.x < 4) {
        c8->display.x += C8_HIGH_DISPLAY_WIDTH;
    }
    c8->display.x -= 4;
    return 2;
}

/**
 * @brief `EXIT` instruction (`00FD`)
 *
 * This instruction stops the interpreter from running.
 *
 * @note This is a SCHIP instruction. `c8` must be in SCHIP or XO-CHIP mode to
 * execute this instruction.
 *
 * @param c8 the `c8_t` to execute the instruction from
 *
 * @return 0, or INVALID_INSTRUCTION_EXCEPTION if `c8` is in SCHIP mode.
 */
static inline int i_exit(c8_t* c8) {
    SCHIP_EXCLUSIVE(c8);
    c8->running = 0;
    return 0;
}

/**
 * @brief `LOW` instruction (`00FE`)
 *
 * This instruction sets the display mode to low resolution (64x32).
 *
 * @note This is a SCHIP instruction. `c8` must be in SCHIP or XO-CHIP mode to
 * execute this instruction.
 *
 * @param c8 the `c8_t` to execute the instruction from
 *
 * @return 2, the number of bytes to increase the program counter by, or
 * INVALID_INSTRUCTION_EXCEPTION if `c8` is in CHIP-8 mode.
 */
static inline int i_low(c8_t* c8) {
    SCHIP_EXCLUSIVE(c8);
    c8->display.mode = C8_DISPLAYMODE_LOW;
    return 2;
}

/**
 * @brief `HIGH` instruction (`00FF`)
 *
 * This instruction sets the display mode to high resolution (128x64).
 *
 * @note This is a SCHIP instruction. `c8` must be in SCHIP or XO-CHIP mode to
 * execute this instruction.
 *
 * @param c8 the `c8_t` to execute the instruction from
 *
 * @return 2, the number of bytes to increase the program counter by, or
 * INVALID_INSTRUCTION_EXCEPTION if `c8` is in CHIP-8 mode.
 */
static inline int i_high(c8_t* c8) {
    SCHIP_EXCLUSIVE(c8);
    c8->display.mode = C8_DISPLAYMODE_HIGH;
    return 2;
}

/**
 * @brief `JP nnn` instruction (`1nnn`)
 *
 * This instruction sets the program counter to `nnn`.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param nnn the address to jump to
 *
 * @return 0, the number of bytes to increase the program counter by.
 */
static inline int i_jp_nnn(c8_t* c8, uint16_t nnn) {
    c8->pc = nnn;
    return 0;
}

/**
 * @brief `CALL nnn` instruction (`2nnn`)
 *
 * This instruction pushes the current program counter onto the stack and
 * sets the program counter to `nnn`.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param nnn the address to call
 *
 * @return 0, the number of bytes to increase the program counter by.
 */
static inline int i_call_nnn(c8_t* c8, uint16_t nnn) {
    if (c8->sp >= 16) {
        C8_EXCEPTION(STACK_OVERFLOW_EXCEPTION, "Stack overflow at %03x", c8->pc);
        return STACK_OVERFLOW_EXCEPTION;
    }
    c8->stack[c8->sp] = c8->pc;
    c8->sp++;
    c8->pc = nnn;
    return 0;
}

/**
 * @brief `SE Vx, kk` instruction (`3xkk`)
 *
 * This instruction checks if the value in register Vx is equal to kk.
 * If they are equal, it increases the program counter by 2.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param x the index of the register Vx (0-15)
 * @param kk the byte value to compare against
 *
 * @return 2, the number of bytes to increase the program counter by.
 */
static inline int i_se_vx_kk(c8_t* c8, uint8_t x, uint8_t kk) {
    if (c8->V[x] == kk) {
        c8->pc += 2;
    }
    return 2;
}

/**
 * @brief `SNE Vx, kk` instruction (4xkk)
 *
 * This instruction checks if the value in register Vx is not equal to kk.
 * If they are not equal, it increases the program counter by 2.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param x the index of the register Vx (0-15)
 * @param kk the byte value to compare against
 *
 * @return 2, the number of bytes to increase the program counter by.
 */
static inline int i_sne_vx_kk(c8_t* c8, uint8_t x, uint8_t kk) {
    if (c8->V[x] != kk) {
        c8->pc += 2;
    }
    return 2;
}

/**
 * @brief `SE Vx, Vy` instruction (`5xy0`)
 *
 * This instruction checks if the value in register Vx is equal to the value in
 * register Vy. If they are not equal, it increases the program counter by 2.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param x the index of the register Vx (0-15)
 * @param y the index of the register Vy (0-15)
 *
 * @return 2, the number of bytes to increase the program counter by.
 */
static inline int i_se_vx_vy(c8_t* c8, uint8_t x, uint8_t y) {
    if (c8->V[x] == c8->V[y]) {
        c8->pc += 2;
    }
    return 2;
}

/**
 * @brief `LD Vx, kk` instruction (`6xkk`)
 *
 * This instruction loads the value `kk` into register Vx.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param x the index of the register Vx (0-15)
 * @param kk the byte value to load into Vx
 *
 * @return 2, the number of bytes to increase the program counter by.
 */
static inline int i_ld_vx_kk(c8_t* c8, uint8_t x, uint8_t kk) {
    c8->V[x] = kk;
    return 2;
}

/**
 * @brief `ADD Vx, kk` instruction (`7xkk`)
 *
 * This instruction adds the value `kk` to the value in register Vx.
 * If the result exceeds 255, it sets the carry flag in VF.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param x the index of the register Vx (0-15)
 * @param kk the byte value to add to Vx
 *
 * @return 2, the number of bytes to increase the program counter by.
 */
static inline int i_add_vx_kk(c8_t* c8, uint8_t x, uint8_t kk) {
    c8->V[0xF] = CARRIES(c8->V[x], kk);
    c8->V[x] += kk;
    return 2;
}

/**
 * @brief `LD Vx, Vy` instruction (`8xy0`)
 *
 * This instruction loads the value in register Vy into register Vx.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param x the index of the register Vx (0-15)
 * @param y the index of the register Vy (0-15)
 *
 * @return 2, the number of bytes to increase the program counter by.
 */
static inline int i_ld_vx_vy(c8_t* c8, uint8_t x, uint8_t y) {
    c8->V[x] = c8->V[y];
    return 2;
}

/**
 * @brief `OR Vx, Vy` instruction (`8xy1`)
 *
 * This instruction performs a bitwise OR operation between the values in registers Vx and Vy,
 * and stores the result in register Vx.
 *
 * @note This instruction is affected by the QUIRK_BITWISE flag. If the flag is
 * set in `c8.quirks`, VF is set to 0.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param x the index of the register Vx (0-15)
 * @param y the index of the register Vy (0-15)
 *
 * @return 2, the number of bytes to increase the program counter by.
 */
static inline int i_or_vx_vy(c8_t* c8, uint8_t x, uint8_t y) {
    c8->V[x] |= c8->V[y];
    QUIRK_BITWISE(c8);
    return 2;
}

/**
 * @brief `AND Vx, Vy` instruction (`8xy2`)
 *
 * This instruction performs a bitwise AND operation between the values in registers Vx and Vy,
 * and stores the result in register Vx.
 *
 * @note This instruction is affected by the QUIRK_BITWISE flag. If the flag is
 * set in `c8.quirks`, VF is set to 0.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param x the index of the register Vx (0-15)
 * @param y the index of the register Vy (0-15)
 *
 * @return 2, the number of bytes to increase the program counter by.
 */
static inline int i_and_vx_vy(c8_t* c8, uint8_t x, uint8_t y) {
    c8->V[x] &= c8->V[y];
    QUIRK_BITWISE(c8);
    return 2;
}

/**
 * @brief `XOR Vx, Vy` instruction (`8xy3`)
 *
 * This instruction performs a bitwise XOR operation between the values in registers Vx and Vy,
 * and stores the result in register Vx.
 *
 * @note This instruction is affected by the QUIRK_BITWISE flag. If the flag is
 * set in `c8.quirks`, VF is set to 0.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param x the index of the register Vx (0-15)
 * @param y the index of the register Vy (0-15)
 *
 * @return 2, the number of bytes to increase the program counter by.
 */
static inline int i_xor_vx_vy(c8_t* c8, uint8_t x, uint8_t y) {
    c8->V[x] = c8->V[x] ^ c8->V[y];
    QUIRK_BITWISE(c8);
    return 2;
}

/**
 * @brief `ADD Vx, Vy` instruction (`8xy4`)
 *
 * This instruction adds the value in register Vy to the value in register Vx.
 * If the result exceeds 255, it sets VF to 1.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param x the index of the register Vx (0-15)
 * @param y the index of the register Vy (0-15)
 *
 * @return 2, the number of bytes to increase the program counter by.
 */
static inline int i_add_vx_vy(c8_t* c8, uint8_t x, uint8_t y) {
    c8->V[0xF] = CARRIES(c8->V[x], c8->V[y]);
    c8->V[x] += c8->V[y];
    return 2;
}

/**
 * @brief `SUB Vx, Vy` instruction (`8xy5`)
 *
 * This instruction subtracts the value in register Vy from the value in register Vx.
 * If the result is not negative, it sets VF to 1.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param x the index of the register Vx (0-15)
 * @param y the index of the register Vy (0-15)
 *
 * @return 2, the number of bytes to increase the program counter by.
 */
static inline int i_sub_vx_vy(c8_t* c8, uint8_t x, uint8_t y) {
    c8->V[0xF] = !BORROWS(c8->V[x], c8->V[y]);
    c8->V[x] -= c8->V[y];
    return 2;
}

/**
 * @brief `SHR Vx, Vy` instruction (`8xy6`)
 *
 * This instruction shifts the value in register Vy right by 1 bit and stores
 * the result in register Vx. The least significant bit of the result is stored
 * in VF.
 *
 * @note This instruction is affected by the QUIRK_SHIFT flag. If the flag is
 * set in `c8.quirks`, the value of y is set to x, effectively making it a right
 * shift of Vx.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param x the index of the register Vx (0-15)
 * @param y the index of the register Vy (0-15)
 *
 * @return 2, the number of bytes to increase the program counter by.
 */
static inline int i_shr_vx_vy(c8_t* c8, uint8_t x, uint8_t y) {
    QUIRK_SHIFT(c8);
    c8->V[x] = c8->V[y] >> 1;
    c8->V[0xF] = c8->V[x] & 0x1;
    return 2;
}

/**
 * @brief `SUBN Vx, Vy` instruction (`8xy7`)
 *
 * This instruction subtracts the value in register Vx from the value in register Vy.
 * If the result is not negative, it sets VF to 1.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param x the index of the register Vx (0-15)
 * @param y the index of the register Vy (0-15)
 *
 * @return 2, the number of bytes to increase the program counter by.
 */
static inline int i_subn_vx_vy(c8_t* c8, uint8_t x, uint8_t y) {
    c8->V[0xF] = !BORROWS(c8->V[y], c8->V[x]);
    c8->V[x] = c8->V[y] - c8->V[x];
    return 2;
}

/**
 * @brief `SHL Vx, Vy` instruction (`8xyE`)
 *
 * This instruction shifts the value in register Vy left by 1 bit and stores
 * the result in register Vx. The most significant bit of the result is stored
 * in VF.
 *
 * @note This instruction is affected by the QUIRK_SHIFT flag. If the flag is
 * set in `c8.quirks`, the value of y is set to x, effectively making it a left
 * shift of Vx.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param x the index of the register Vx (0-15)
 * @param y the index of the register Vy (0-15)
 *
 * @return 2, the number of bytes to increase the program counter by.
 */
static inline int i_shl_vx_vy(c8_t* c8, uint8_t x, uint8_t y) {
    QUIRK_SHIFT(c8);
    c8->V[x] = c8->V[y] << 1;
    c8->V[0xF] = (c8->V[x] >> 7) & 1;
    return 2;
}

/**
 * @brief `SNE Vx, Vy` instruction (`9xy0`)
 *
 * This instruction checks if the value in register Vx is not equal to the value in register Vy.
 * If they are not equal, it increases the program counter by 2.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param x the index of the register Vx (0-15)
 * @param y the index of the register Vy (0-15)
 *
 * @return 2, the number of bytes to increase the program counter by.
 */
static inline int i_sne_vx_vy(c8_t* c8, uint8_t x, uint8_t y) {
    if (c8->V[x] != c8->V[y]) {
        c8->pc += 2;
    }
    return 2;
}

/**
 * @brief `LD I, nnn` instruction (`Annn`)
 *
 * This instruction sets the index register I to the value nnn.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param nnn the address to set I to
 *
 * @return 2, the number of bytes to increase the program counter by.
 */
static inline int i_ld_i_nnn(c8_t* c8, uint16_t nnn) {
    c8->I = nnn;
    return 2;
}

/**
 * @brief `JP V0, nnn` instruction (`Bnnn`)
 *
 * This instruction sets the program counter to `nnn` plus the value in register V0.
 *
 * @note If the QUIRK_JUMP flag is set in `c8->flags`, it uses the value in
 * register V[8th bit of nnn] instead of V0.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param nnn the address to jump to
 *
 * @return 0, the number of bytes to increase the program counter by.
 */
static inline int i_jp_v0_nnn(c8_t* c8, uint16_t nnn) {
    if (c8->flags & C8_FLAG_QUIRK_JUMP) {
        c8->pc = nnn + c8->V[(nnn >> 8) & 0xF];
    } else {
        c8->pc = nnn + c8->V[0];
    }
    return 0;
}

/**
 * @brief `RND Vx, kk` instruction (`Cxkk`)
 *
 * This instruction generates a random number and performs a bitwise AND operation
 * with `kk`, storing the result in register Vx.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param x the index of the register Vx (0-15)
 * @param kk the byte value to AND with the random number
 *
 * @return 2, the number of bytes to increase the program counter by.
 */
static inline int i_rnd_vx_kk(c8_t* c8, uint8_t x, uint8_t kk) {
    c8->V[x] = rand() & kk;
    return 2;
}

/**
 * @brief `DRW Vx, Vy, b` instruction (`Dxyb`)
 *
 * This instruction draws a sprite at the coordinates specified by the values in
 * registers Vx and Vy. The sprite is `b` bytes long, starting from the address
 * in the index register I. The sprite is XOR'd onto the display, and if any
 * pixels are turned off that were previously on, the VF register is set to 1.
 * Then the draw flag is set to 1.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param x the index of the register Vx (0-15)
 * @param y the index of the register Vy (0-15)
 * @param b the number of bytes in the sprite (1-16)
 *
 * @return 2, the number of bytes to increase the program counter by.
 */
static inline int i_drw_vx_vy_b(c8_t* c8, uint8_t x, uint8_t y, uint8_t b) {
    c8->V[0xF] = 0;
    int dw = C8_LOW_DISPLAY_WIDTH;
    int dh = C8_LOW_DISPLAY_HEIGHT;
    int h = 8;
    int ox = 0;
    int oy = 0;

    if (c8->display.mode == C8_DISPLAYMODE_HIGH) {
        if (b == 0) {
            b = 16;
        }
        dw = C8_HIGH_DISPLAY_WIDTH;
        dh = C8_HIGH_DISPLAY_HEIGHT;
        ox = c8->display.x;
        oy = c8->display.y;
    }

    for (int i = 0; i < b; i++) {
        for (int j = 0; j < h; j++) {
            int dx = (c8->V[x] + j + ox) % dw;
            int dy = (c8->V[y] + i + oy) % dh;

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
    return 2;
}

/**
 * @brief `SKP Vx` instruction (`Ex9E`)
 *
 * This instruction checks if the key corresponding to the value in register Vx
 * is pressed. If it is pressed, it increases the program counter by 2.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param x the index of the register Vx (0-15)
 *
 * @return 2, the number of bytes to increase the program counter by.
 */
static inline int i_skp_vx(c8_t* c8, uint8_t x) {
    if (c8->key[c8->V[x]]) {
        c8->pc += 2;
    }
    return 2;
}

/**
 * @brief `SKNP Vx` instruction (`ExA1`)
 *
 * This instruction checks if the key corresponding to the value in register Vx
 * is not pressed. If it is not pressed, it increases the program counter by 2.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param x the index of the register Vx (0-15)
 *
 * @return 2, the number of bytes to increase the program counter by.
 */
static inline int i_sknp_vx(c8_t* c8, uint8_t x) {
    if (!c8->key[c8->V[x]]) {
        c8->pc += 2;
    }
    return 2;
}

/**
 * @brief `LD Vx, DT` instruction (`Fx07`)
 *
 * This instruction loads the value of the delay timer into register Vx.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param x the index of the register Vx (0-15)
 *
 * @return 2, the number of bytes to increase the program counter by.
 */
static inline int i_ld_vx_dt(c8_t* c8, uint8_t x) {
    c8->V[x] = c8->dt;
    return 2;
}

/**
 * @brief `LD Vx, K` instruction (`Fx0A`)
 *
 * This instruction waits for a key press and stores the index of the pressed
 * key in register Vx. If no key is pressed, it sets the `waitingForKey` flag
 * to 1.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param x the index of the register Vx (0-15)
 *
 * @return 2 if a key is pressed, 0 if no key is pressed and waitingForKey is set.
 */
static inline int i_ld_vx_k(c8_t* c8, uint8_t x) {
    // Check if a key is already pressed
    for (int i = 0; i < 16; i++) {
        if (c8->key[i]) {
            c8->V[x] = i;
            return 2;
        }
    }

    // Wait for a key press
    c8->VK = x;
    c8->waitingForKey = 1;
    return 0;
}

/**
 * @brief `LD DT, Vx` instruction (`Fx15`)
 *
 * This instruction sets the delay timer to the value in register Vx.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param x the index of the register Vx (0-15)
 *
 * @return 2, the number of bytes to increase the program counter by.
 */
static inline int i_ld_dt_vx(c8_t* c8, uint8_t x) {
    c8->dt = c8->V[x];
    return 2;
}

/**
 * @brief `LD ST, Vx` instruction (`Fx18`)
 *
 * This instruction sets the sound timer to the value in register Vx.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param x the index of the register Vx (0-15)
 * @return 2, the number of bytes to increase the program counter by.
 */
static inline int i_ld_st_vx(c8_t* c8, uint8_t x) {
    c8->st = c8->V[x];
    return 2;
}

/**
 * @brief `ADD I, Vx` instruction (`Fx1E`)
 *
 * This instruction adds the value in register Vx to the index register I.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param x the index of the register Vx (0-15)
 *
 * @return 2, the number of bytes to increase the program counter by.
 */
static inline int i_add_i_vx(c8_t* c8, uint8_t x) {
    c8->I += c8->V[x];
    return 2;
}

/**
 * @brief `LD F, Vx` instruction (`Fx29`)
 *
 * This instruction sets the index register I to the address of the
 * font character corresponding to the value in register Vx.
 *
 * The font characters are stored in the range `C8_FONT_START` to
 * `C8_FONT_START` + 80.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param x the index of the register Vx (0-15)
 * @return 2, the number of bytes to increase the program counter by.
 */
static inline int i_ld_f_vx(c8_t* c8, uint8_t x) {
    c8->I = C8_FONT_START + (c8->V[x] * 5);
    return 2;
}

/**
 * @brief `LD HF, Vx` instruction (`Fx30`)
 *
 * This instruction sets the index register I to the address of the
 * font character corresponding to the value in register Vx.
 *
 * The font characters are stored in the range `C8_HIGH_FONT_START` to
 * `C8_HIGH_FONT_START` + 160.
 *
 * @note This instruction is only available in SCHIP and XO-CHIP modes.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param x the index of the register Vx (0-15)
 * @return 2, the number of bytes to increase the program counter by, or
 * `INVALID_INSTRUCTION_EXCEPTION` if `c8` is in CHIP-8 mode.
 */
static inline int i_ld_hf_vx(c8_t* c8, uint8_t x) {
    SCHIP_EXCLUSIVE(c8);

    c8->I = C8_HIGH_FONT_START + (c8->V[x] * 10);
    return 2;
}

/**
 * @brief `LD I, B` instruction (`Fx33`)
 *
 * This instruction stores the binary-coded decimal representation of the value
 * in register Vx at the address in index register I. The hundreds digit is stored
 * at I, the tens digit at I + 1, and the ones digit at I + 2.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param x the index of the register Vx (0-15)
 *
 * @return 2, the number of bytes to increase the program counter by.
 */
static inline int i_ld_b_vx(c8_t* c8, uint8_t x) {
    c8->mem[c8->I] = (c8->V[x] / 100) % 10; // hundreds
    c8->mem[c8->I + 1] = (c8->V[x] / 10) % 10; // tens
    c8->mem[c8->I + 2] = c8->V[x] % 10; // ones
    return 2;
}

/**
 * @brief `LD [I], Vx` instruction (`Fx55`)
 *
 * This instruction stores the values of registers V0 to Vx at the address in
 * index register I. The values are stored in memory starting from address I.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param x the index of the register Vx (0-15)
 *
 * @return 2, the number of bytes to increase the program counter by.
 */
static inline int i_ld_ip_vx(c8_t* c8, uint8_t x) {
    for (int i = 0; i < x; i++) {
        c8->mem[c8->I + i] = c8->V[i];
    }
    QUIRK_LOADSTORE(c8);
    return 2;
}

/**
 * @brief `LD Vx, [I]` instruction (`Fx65`)
 *
 * This instruction loads the values from memory starting at address I into
 * registers V0 to Vx. The values are loaded in order from I to I + (x - 1).
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param x the number of registers to load (0-15)
 *
 * @return 2, the number of bytes to increase the program counter by.
 */
static inline int i_ld_vx_ip(c8_t* c8, uint8_t x) {
    for (int i = 0; i < x; i++) {
        c8->V[i] = c8->mem[c8->I + i];
    }
    QUIRK_LOADSTORE(c8);
    return 2;
}

/**
 * @brief `LD R, Vx` instruction (`Fx75`)
 *
 * This instruction copies the values from registers V0 to Vx into the
 * registers R0 to Rx. The values are copied in order from V0 to Vx
 * to R0 to Rx.
 *
 * @note This instruction is only available in SCHIP and XO-CHIP modes.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param x the number of registers to copy (0-15)
 *
 * @return 2, the number of bytes to increase the program counter by,
 * or INVALID_INSTRUCTION_EXCEPTION if `c8` is in CHIP-8 mode.
 */
static inline int i_ld_r_vx(c8_t* c8, uint8_t x) {
    SCHIP_EXCLUSIVE(c8);
    for (int i = 0; i < x; i++) {
        c8->R[i] = c8->V[i];
    }
    return 2;
}

/**
 * @brief `LD Vx, R` instruction (`Fx85`)
 *
 * This instruction copies the values from registers R0 to Rx into the
 * registers V0 to Vx. The values are copied in order from R0 to Rx
 * to V0 to Vx.
 *
 * @note This instruction is only available in SCHIP and XO-CHIP modes.
 *
 * @param c8 the `c8_t` to execute the instruction from
 * @param x the number of registers to copy (0-15)
 *
 * @return 2, the number of bytes to increase the program counter by,
 * or INVALID_INSTRUCTION_EXCEPTION if `c8` is in CHIP-8 mode.
 */
static inline int i_ld_vx_r(c8_t* c8, uint8_t x) {
    SCHIP_EXCLUSIVE(c8);

    for (int i = 0; i < x; i++) {
        c8->V[i] = c8->R[i];
    }
    return 2;
}
