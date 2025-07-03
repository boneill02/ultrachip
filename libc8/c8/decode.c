/**
 * @file c8/decode.c
 *
 * Stuff for disassembling bytecode.
 */

#include "decode.h"

#include "defs.h"

#include <stdio.h>
#include <stdlib.h>

#define DEFINE_LABELS (args & C8_DECODE_DEFINE_LABELS)
#define PRINT_ADDRESSES (args & C8_DECODE_PRINT_ADDRESSES)
#define RESULT_SIZE 32

static void find_labels(FILE*, uint8_t*);

char result[RESULT_SIZE];

/**
 * @brief Convert bytecode from `input` to assembly and writes it to `output`.
 *
 * `ARG_PRINT_ADDRESSES` should be AND'd to args to print addresses before each
 * assembly instruction.
 *
 * `ARG_DEFINE_LABELS` should be AND'd to args to define labels.
 *
 * @param input the CHIP-8 ROM file to disassemble
 * @param output the file to write the assembly to
 * @param args 0 with `ARG_PRINT_ADDRESSES` and/or `ARG_DEFINE_LABELS`
 * optionally OR'd
 */
void c8_decode(FILE* input, FILE* output, int args) {
    int c;
    uint8_t* labelMap = NULL;
    uint16_t addr = C8_PROG_START;
    uint16_t ins = 0;

    if (DEFINE_LABELS) {
        labelMap = (uint8_t*)calloc(0x1000, sizeof(uint8_t));
        find_labels(input, labelMap);
        rewind(input);
    }

    while ((c = fgetc(input)) != EOF) {
        if (addr % 2 == 0) {
            ins = ((uint16_t)c) << 8;
        }
        else {
            ins |= (uint16_t)c;

            if (DEFINE_LABELS && labelMap[addr]) {
                fprintf(output, "label%d:\n", labelMap[addr]);
            }

            if (PRINT_ADDRESSES) {
                fprintf(output, "%03x: ", addr - 1);
            }
            fprintf(output, "%s\n", c8_decode_instruction(ins, labelMap));
        }
        addr++;
    }

    free(labelMap);
}

/**
 * @brief Decode `in` and return its assembly value.
 *
 * Gets the assembly value of instruction `in`, stores it in the global
 * variable `result`, and returns `result`.
 *
 * If `label_map` is not `NULL`, it should point to an aray of size `MEMSIZE`,
 * with all "labeled" elements set to a unique, non-zero integer. All other
 * elements should be zero.
 *
 * If `label_map` is not `NULL`, `label_map[nnn]` is greater than 0, and the
 * instruction contains a nnn argument, a label name will be generated and used
 * in the resulting string.
 *
 * @param in The instruction to decode
 * @param label_map The label map (can be NULL for no labels)
 *
 * @return `result` containing the associated assembly instruction
 */
char* c8_decode_instruction(uint16_t in, uint8_t* label_map) {
    C8_EXPAND(in);
    for (int i = 0; i < 16; i++) {
        result[i] = '\0';
    }

    switch (a) {
    case 0x0:
        if (in == 0x00E0) {
            snprintf(result, RESULT_SIZE, "CLS");
        }
        else if (in == 0x00EE) {
            snprintf(result, RESULT_SIZE, "RET");
        }
        else if (x == 0x0 && y == 0xC) {
            snprintf(result, RESULT_SIZE, "SCD 0x%01X", b);
        }
        else if (in == 0x00FB) {
            snprintf(result, RESULT_SIZE, "SCR");
        }
        else if (in == 0x00FC) {
            snprintf(result, RESULT_SIZE, "SCL");
        }
        else if (in == 0x00FD) {
            snprintf(result, RESULT_SIZE, "EXIT");
        }
        else if (in == 0x00FE) {
            snprintf(result, RESULT_SIZE, "LOW");
        }
        else if (in == 0x00FF) {
            snprintf(result, RESULT_SIZE, "HIGH");
        }
        else {
            snprintf(result, RESULT_SIZE, ".dw %04X", in);
        }
        break;
    case 0x1:
        if (label_map && label_map[nnn]) {
            snprintf(result, RESULT_SIZE, "JP label%d", label_map[nnn]);
        }
        else {
            snprintf(result, RESULT_SIZE, "JP $%03X", nnn);
        }
        break;
    case 0x2:
        if (label_map && label_map[nnn]) {
            snprintf(result, RESULT_SIZE, "CALL label%d", label_map[nnn]);
        }
        else {
            snprintf(result, RESULT_SIZE, "CALL $%03X", nnn);
        }
        break;
    case 0x3: snprintf(result, RESULT_SIZE, "SE V%01X, 0x%02X", x, kk); break;
    case 0x4: snprintf(result, RESULT_SIZE, "SNE V%01X, 0x%02X", x, kk); break;
    case 0x5: snprintf(result, RESULT_SIZE, "SE V%01X, V%01X", x, y); break;
    case 0x6: snprintf(result, RESULT_SIZE, "LD V%01X, 0x%02X", x, kk); break;
    case 0x7: snprintf(result, RESULT_SIZE, "ADD V%01X, 0x%02X", x, kk); break;
    case 0x8:
        switch (b) {
        case 0x0: snprintf(result, RESULT_SIZE, "LD V%01X, V%01X", x, y); break;
        case 0x1: snprintf(result, RESULT_SIZE, "OR V%01X, V%01X", x, y); break;
        case 0x2: snprintf(result, RESULT_SIZE, "AND V%01X, V%01X", x, y); break;
        case 0x3: snprintf(result, RESULT_SIZE, "XOR V%01X, V%01X", x, y); break;
        case 0x4: snprintf(result, RESULT_SIZE, "ADD V%01X, V%01X", x, y); break;
        case 0x5: snprintf(result, RESULT_SIZE, "SUB V%01X, V%01X", x, y); break;
        case 0x6: snprintf(result, RESULT_SIZE, "SHR V%01X, V%01X", x, y); break;
        case 0x7: snprintf(result, RESULT_SIZE, "SUBN V%01X, V%01X", x, y); break;
        case 0xE: snprintf(result, RESULT_SIZE, "SHL V%01X, V%01X", x, y); break;
        default: snprintf(result, RESULT_SIZE, ".dw %04X", in); break;
        }
        break;
    case 0x9: snprintf(result, RESULT_SIZE, "SNE V%01X, V%01X", x, y); break;
    case 0xA:
        if (label_map && label_map[nnn]) {
            snprintf(result, RESULT_SIZE, "LD I, label%d", label_map[nnn]);
        }
        else {
            snprintf(result, RESULT_SIZE, "LD I, $%03X", nnn);
        }
        break;
    case 0xB:
        if (label_map && label_map[nnn]) {
            snprintf(result, RESULT_SIZE, "JP V0, label%d", label_map[nnn]);
        }
        else {
            snprintf(result, RESULT_SIZE, "JP V0, $%03X", nnn);
        }
        break;
    case 0xC: snprintf(result, RESULT_SIZE, "RND V%01X, 0x%02X", x, kk); break;
    case 0xD: snprintf(result, RESULT_SIZE, "DRW V%01X, V%01X, 0x%01X", x, y, b); break;
    case 0xE:
        if (kk == 0x9E) {
            snprintf(result, RESULT_SIZE, "SKP V%01X", x);
        }
        else if (kk == 0xA1) {
            snprintf(result, RESULT_SIZE, "SKNP V%01X", x);
        }
        else {
            snprintf(result, RESULT_SIZE, ".dw %04X", in);
        }
        break;
    case 0xF:
        switch (kk) {
        case 0x07: snprintf(result, RESULT_SIZE, "LD V%01X, DT", x); break;
        case 0x0A: snprintf(result, RESULT_SIZE, "LD V%01X, K", x); break;
        case 0x15: snprintf(result, RESULT_SIZE, "LD DT, V%01X", x); break;
        case 0x18: snprintf(result, RESULT_SIZE, "LD ST, V%01X", x); break;
        case 0x1E: snprintf(result, RESULT_SIZE, "ADD I, V%01X", x); break;
        case 0x29: snprintf(result, RESULT_SIZE, "LD F, V%01X", x); break;
        case 0x30: snprintf(result, RESULT_SIZE, "LD HF, V%01X", x); break;
        case 0x33: snprintf(result, RESULT_SIZE, "LD B, V%01X", x); break;
        case 0x55: snprintf(result, RESULT_SIZE, "LD [I], V%01X", x); break;
        case 0x65: snprintf(result, RESULT_SIZE, "LD V%01X, [I]", x); break;
        case 0x75: snprintf(result, RESULT_SIZE, "LD R, V%01X", x); break;
        case 0x85: snprintf(result, RESULT_SIZE, "LD V%01X, R", x); break;
        default: snprintf(result, RESULT_SIZE, ".dw %04X", in);
        }
        break;
    default: snprintf(result, RESULT_SIZE, ".dw %04X", in);
    }

    return result;
}

/**
 * @brief Returns the value to jump to if `n` is a jump instruction, or 0.
 *
 * @param in The instruction to check
 *
 * @return `in`'s `nnn` value if it exists, 0 otherwise.
 */
uint16_t jump(uint16_t in) {
    uint16_t a = C8_A(in);

    if (a == 0x1 || a == 0x2 || a == 0xa || a == 0xb) {
        return C8_NNN(in);
    }
    return 0;
}

/**
 * @brief Generate labels from `input` and add labels to `labelMap`.
 *
 * This function finds jump instructions in CHIP-8 ROM file `input` and adds
 * incrementing values to `labelMap` accordingly.
 *
 * @param input the ROM to get labels from
 * @param labelMap where to store the labels
 */
static void find_labels(FILE* input, uint8_t* labelMap) {
    uint16_t addr = C8_PROG_START;
    uint8_t count = 1;
    uint16_t ins = 0;
    uint16_t to;

    int c;
    while ((c = fgetc(input)) != EOF) {
        if (addr % 2 == 0) {
            ins = ((uint16_t)c) << 8;
        }
        else {
            ins |= (uint16_t)c;
            if ((to = jump(ins))) {
                labelMap[to] = count++;
            }
        }
        addr++;
    }
}
