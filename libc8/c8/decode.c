/**
 * @file c8/decode.c
 *
 * Stuff for disassembling bytecode.
 */

#include "decode.h"

#include "defs.h"
#include "c8/private/symbol.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    if ((in & 0xFFF0) == 0x00C0) {
        // Special case for SCD n
        // SCD is the only a=0 instruction that has a b parameter.
        snprintf(result, RESULT_SIZE, "SCD 0x%01X", b);
        return result;
    }
    else if (a == 0xB) {
        // Special case for JP V0, NNN
        // V0 here is not masked, it's part of the instruction.
        if (label_map[nnn]) {
            snprintf(result, RESULT_SIZE, "JP V0, label%d", label_map[nnn]);
        }
        else {
            snprintf(result, RESULT_SIZE, "JP V0, $%03X", nnn);
        }
        return result;
    }
    else if (a == 8 && b == 0x6) {
        // Special case for SHR Vx, Vy
        // This instruction can take multiple forms depending on quirks, so we
        // default to 2 parameters when decoding
        snprintf(result, RESULT_SIZE, "SHR V%01X, V%01X", x, y);
        return result;
    }
    else if (a == 8 && b == 0xE) {
        // Special case for SHL Vx, Vy
        // This instruction can take multiple forms depending on quirks, so we
        // default to 2 parameters when decoding
        snprintf(result, RESULT_SIZE, "SHL V%01X, V%01X", x, y);
        return result;
    }


    for (int i = 0; formats[i].cmd != I_NULL; i++) {
        if ((C8_A(formats[i].base) & a) == C8_A(in)) {
            int match = 1;
            if (a == 0x0 || a == 0xE || a == 0xF) {
                // 0x0, 0xE, and 0xF instructions have kk as a mask, so we need to check
                match = kk == C8_KK(formats[i].base);
            }
            else if (a == 0x8) {
                // 0x8 instructions have b as a mask, so we need to check
                match = b == C8_B(formats[i].base);
            }

            if (match) {
                snprintf(result, RESULT_SIZE, "%s", c8_instructionStrings[formats[i].cmd]);

                int idx = strlen(result);
                for (int j = 0; j < formats[i].pcount; j++) {
                    if (j > 0) {
                        snprintf(result + idx, RESULT_SIZE - idx, ",");
                        idx++;
                    }
                    switch (formats[i].ptype[j]) {
                    case SYM_INT12:
                        if (label_map[nnn]) {
                            snprintf(result + idx, RESULT_SIZE - idx, " label%d", label_map[nnn]);
                        }
                        else {
                            snprintf(result + idx, RESULT_SIZE - idx, " $%03X", nnn);
                        }
                        break;
                    case SYM_INT8:
                        snprintf(result + idx, RESULT_SIZE - idx, " 0x%02X", (in & formats[i].pmask[j]) >> shift(formats[i].pmask[j]));
                        break;
                    case SYM_INT4:
                        snprintf(result + idx, RESULT_SIZE - idx, " 0x%01X", (in & formats[i].pmask[j]) >> shift(formats[i].pmask[j]));
                        break;
                    case SYM_V:
                        snprintf(result + idx, RESULT_SIZE - idx, " V%01X", (in & formats[i].pmask[j]) >> shift(formats[i].pmask[j]));
                        break;
                    default:
                        snprintf(result + idx, RESULT_SIZE - idx, " %s", c8_identifierStrings[formats[i].ptype[j]]);
                        break;
                    }
                    idx = strlen(result);
                }
                return result;
            }
        }
    }

    snprintf(result, RESULT_SIZE, ".DW 0x%04X", in);
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
