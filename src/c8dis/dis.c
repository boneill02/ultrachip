#include "dis.h"

#include "util/defs.h"
#include "util/decode.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DEFINE_LABELS (args & ARG_DEFINE_LABELS)
#define PRINT_ADDRESSES (args & ARG_PRINT_ADDRESSES)

static void find_labels(FILE *, uint8_t *);

/**
 * @brief Generate labels from `input` and add labels to `labelMap`.
 *
 * This function finds jump instructions in CHIP-8 ROM file `input` and adds
 * incrementing values to `labelMap` accordingly.
 *
 * @param input the ROM to get labels from
 * @param labelMap where to store the labels
 */
static void find_labels(FILE *input, uint8_t *labelMap)
{
	uint16_t addr = PROG_START;
	uint8_t count = 1;
	uint16_t ins = 0;
	uint16_t to;

	int c;
	while ((c = fgetc(input)) != EOF)
	{
		if (addr % 2 == 0)
		{
			ins = ((uint16_t)c) << 8;
		}
		else
		{
			ins |= (uint16_t)c;
			if ((to = jump(ins)))
			{
				labelMap[to] = count++;
			}
		}
	}
}

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
 * optionally AND'd
 */
void disassemble(FILE *input, FILE *output, int args)
{
	int c;
	uint8_t *labelMap = NULL;
	uint16_t addr = PROG_START;
	uint16_t ins = 0;

	if (DEFINE_LABELS)
	{
		labelMap = (uint8_t *)calloc(0x1000, sizeof(uint8_t));
		find_labels(input, labelMap);
		rewind(input);
	}

	while ((c = fgetc(input)) != EOF)
	{
		if (addr % 2 == 0)
		{
			ins = ((uint16_t)c) << 8;
		}
		else
		{
			ins |= (uint16_t)c;

			if (DEFINE_LABELS && labelMap[addr])
			{
				fprintf(output, "label%d:\n", labelMap[addr]);
			}

			if (args & PRINT_ADDRESSES)
			{
				fprintf(output, "%03x: ", addr - 1);
			}
			fprintf(output, "%s\n", decode_instruction(ins, labelMap));
		}
		addr++;
	}

	free(labelMap);
}
