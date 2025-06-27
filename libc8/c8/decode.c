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

static void find_labels(FILE*, uint8_t*);

char result[32];

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
            sprintf(result, "CLS");
        }
        else if (in == 0x00EE) {
            sprintf(result, "RET");
        }
        else if (x == 0x0 && y == 0xC) {
            sprintf(result, "SCD %d", b);
        }
        else if (in == 0x00FB) {
            sprintf(result, "SCR");
        }
        else if (in == 0x00FC) {
            sprintf(result, "SCL");
        }
        else if (in == 0x00FD) {
            sprintf(result, "EXIT");
        }
        else if (in == 0x00FE) {
            sprintf(result, "LOW");
        }
        else if (in == 0x00FF) {
            sprintf(result, "HIGH");
        }
        else {
            sprintf(result, ".db %04x", in);
        }
        break;
    case 0x1:
        if (label_map && label_map[nnn]) {
            sprintf(result, "JP label%d", label_map[nnn]);
        }
        else {
            sprintf(result, "JP $%03x", nnn);
        }
        break;
    case 0x2:
        if (label_map && label_map[nnn]) {
            sprintf(result, "CALL label%d", label_map[nnn]);
        }
        else {
            sprintf(result, "CALL $%03x", nnn);
        }
        break;
    case 0x3: sprintf(result, "SE V%01x, $%02x", x, kk); break;
    case 0x4: sprintf(result, "SNE V%01x, $%02x", x, kk); break;
    case 0x5: sprintf(result, "SE V%01x, V%01x", x, y); break;
    case 0x6: sprintf(result, "LD V%01x, $%02x", x, kk); break;
    case 0x7: sprintf(result, "ADD V%01x, $%02x", x, kk); break;
    case 0x8:
        switch (b) {
        case 0x0: sprintf(result, "LD V%01x, V%01x", x, y); break;
        case 0x1: sprintf(result, "OR V%01x, V%01x", x, y); break;
        case 0x2: sprintf(result, "AND V%01x, V%01x", x, y); break;
        case 0x3: sprintf(result, "XOR V%01x, V%01x", x, y); break;
        case 0x4: sprintf(result, "ADD V%01x, V%01x", x, y); break;
        case 0x5: sprintf(result, "SUB V%01x, V%01x", x, y); break;
        case 0x6: sprintf(result, "SHR V%01x, V%01x", x, y); break;
        case 0x7: sprintf(result, "SUBN V%01x, V%01x", x, y); break;
        case 0xE: sprintf(result, "SHL V%01x, V%01x", x, y); break;
        default: sprintf(result, ".db %04x", in); break;
        }
        break;
    case 0x9: sprintf(result, "SNE V%01x, V%01x", x, y); break;
    case 0xA:
        if (label_map && label_map[nnn]) {
            sprintf(result, "LD I, label%d", label_map[nnn]);
        }
        else {
            sprintf(result, "LD I, $%03x", nnn);
        }
        break;
    case 0xB:
        if (label_map && label_map[nnn]) {
            sprintf(result, "JP V0, label%d", label_map[nnn]);
        }
        else {
            sprintf(result, "JP V0, $%03x", nnn);
        }
        break;
    case 0xC: sprintf(result, "RND V%01x, $%02x", x, kk); break;
    case 0xD: sprintf(result, "DRW V%01x, V%01x, $%01x", x, y, b); break;
    case 0xE:
        if (kk == 0x9E) {
            sprintf(result, "SKP V%01x", x);
        }
        else if (kk == 0xA1) {
            sprintf(result, "SKNP V%01x", x);
        }
        else {
            sprintf(result, ".db %04x", in);
        }
        break;
    case 0xF:
        switch (kk) {
        case 0x07: sprintf(result, "LD V%01x, DT", x); break;
        case 0x0A: sprintf(result, "LD V%01x, K", x); break;
        case 0x15: sprintf(result, "LD DT, V%01x", x); break;
        case 0x18: sprintf(result, "LD ST, V%01x", x); break;
        case 0x1E: sprintf(result, "ADD I, V%01x", x); break;
        case 0x29: sprintf(result, "LD F, V%01x", x); break;
        case 0x30: sprintf(result, "LD HF, V%01x", x); break;
        case 0x33: sprintf(result, "LD B, V%01x", x); break;
        case 0x55: sprintf(result, "LD [I], V%01x", x); break;
        case 0x65: sprintf(result, "LD V%01x, [I]", x); break;
        case 0x75: sprintf(result, "LD R, V%01x", x); break;
        case 0x85: sprintf(result, "LD V%01x, R", x); break;
        default: sprintf(result, ".db %04x", in);
        }
        break;
    default: sprintf(result, ".db %04x", in);
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
