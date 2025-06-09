#include "debug.h"

#include "chip8.h"
#include "util/decode.h"
#include "util/util.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @enum Command
 * @brief Represents command types
 *
 * This enumeration defines all possible debug mode commands
 */
enum Command {
	CMD_NONE = -1,
	CMD_ADD_BREAKPOINT = 0,
	CMD_RM_BREAKPOINT,
	CMD_CONTINUE,
	CMD_NEXT,
	CMD_SET,
	CMD_LOAD,
	CMD_SAVE,
	CMD_PRINT,
	CMD_HELP,
	CMD_QUIT,
};

/**
 * @enum Argument
 * @brief Represents argument types
 *
 * This enumeration defines all possible debug mode arguments.
 */
enum Argument {
	ARG_NONE = -1,
	ARG_SP = 0,
	ARG_DT,
	ARG_ST,
	ARG_PC,
	ARG_I,
	ARG_VK,
	ARG_STACK,
	ARG_V,
	ARG_ADDR,
	ARG_FILE,
};

/**
 * @union Arg
 * @brief Stores an argument's value (string or int)
 * 
 * Stores an argument's value as a string or int.
 * `s` gets the string value.
 * `i` gets the integer value.
 */
union ArgValue {
	char *s;
	int i;
};

/**
 * @struct arg_s
 * @brief Represents an argument for a command with a type and value.
 */
struct arg_s {
	enum Argument type;
	union ArgValue value;
};

/**
 * @struct cmd_s
 * @brief Represents a command with an ID, argument ID, and associated argument.
 *
 * This structure defines a command that includes:
 * - the command identifier (`id`),
 * - an argument (`arg`)
 * - value to set `arg->value` to (`setValue`, for set commands)
 */
struct cmd_s {
	enum Command id;
	struct arg_s arg;
	int setValue;
};

/**
 * @struct cmd_t
 * @brief Represents a command with an ID, argument ID, and associated argument.
 *
 * This structure defines a command that includes:
 * - the command identifier (`id`),
 * - an argument identifier (`argid`), and
 * - an argument value (`arg`) which can be a string or an integer.
 *
 * The `arg` member is a union that stores the actual argument data.
 */
typedef struct cmd_s cmd_t;

int breakpoints[MEMSIZE];

static int get_command(cmd_t *, char *);
static int load_file_arg(cmd_t *, char *);
static int load_state(chip8_t *, const char *);
static int save_state(chip8_t *, const char *);
static int parse_arg(cmd_t *, char *);
static void print_help(void);
static void print_value(chip8_t *, cmd_t *);
static int set_value(chip8_t *, cmd_t *);

const char *args[] = {
	"SP",
	"DT",
	"ST",
	"PC",
	"I",
	"stack",
};

/**
 * These are string values of all possible commands, ordered to match the
 * Command enumerator.
 */
const char *cmds[] = {
	"break",
	"rmbreak",
	"continue",
	"next",
	"set",
	"load",
	"save",
	"print",
	"help",
	"quit",
};

/**
 * @brief Debug command line loop.
 * 
 * This function parses user commands from stdin and prints the result until
 * one of the following conditions is met:
 * 
 * - continue command is evaluated (return DEBUG_CONTINUE)
 * 
 * - quit command is evaluated (return DEBUG_QUIT)
 * 
 * - next command is evaluated (return DEBUG_STEP)
 * 
 * @param c8 the current CHIP-8 state
 * @return DEBUG_CONTINUE, DEBUG_STEP, or DEBUG_QUIT
 */
int debug_repl(chip8_t *c8) {
	char buf[64];
	cmd_t cmd;

	printf("debug > ");
	while(scanf("%63[^\n]", buf) != EOF) {
		if (get_command(&cmd, buf)) {
			switch(cmd.id) {
				case CMD_ADD_BREAKPOINT:
					if (cmd.arg.type == -1) {
						breakpoints[c8->pc] = 1;
					} else {
						breakpoints[cmd.arg.value.i] = 1;
					}
					break;
				case CMD_RM_BREAKPOINT:
					if (cmd.arg.value.i == -1) {
						breakpoints[c8->pc] = 0;
					} else {
						breakpoints[cmd.arg.value.i] = 0;
					}
					break;
				case CMD_CONTINUE:
					return DEBUG_CONTINUE;
				case CMD_NEXT:
					return DEBUG_STEP;
				case CMD_LOAD:
					load_state(c8, cmd.arg.value.s);
					break;
				case CMD_SAVE:
					save_state(c8, cmd.arg.value.s);
					break;
				case CMD_PRINT:
					print_value(c8, &cmd);
					break;
				case CMD_SET:
					set_value(c8, &cmd);
					break;
				case CMD_HELP:
					print_help();
					break;
				case CMD_QUIT:
					return DEBUG_QUIT;
				case CMD_NONE:
					printf("Invalid command\n");
					break;
			 }
		} else {
			printf("Invalid command\n");
		}
		printf("debug > ");
		getchar(); // Consume newline
	}

	return DEBUG_QUIT; // EOF
}


/**
 * @brief Parse command from string s and store in cmd.
 * 
 * @param cmd where to store the command attributes
 * @return 1 if successful, 0 if not
 */
static int get_command(cmd_t *cmd, char *s) {
	size_t len;
	const char *full;
	int numCmds = (int) sizeof(cmds) / sizeof(cmds[0]);

	/* reset cmd */
	cmd->id = CMD_NONE;
	cmd->arg.value.i = -1;
	cmd->arg.type = ARG_NONE;
	cmd->setValue = -1;

	s = trim(s);
	for (int i = 0; i < numCmds; i++) {
		full = cmds[i];
		len = strlen(full);

		if (!strncmp(s, full, len)) {
			/* Full cmd */
			cmd->id = (enum Command) i;
			if (s[len] == '\0') {
				/* No arg */
				cmd->arg.type = ARG_NONE;
				return 1;
			} else if (isspace(s[len])) {
				/* With arg */
				return parse_arg(cmd, trim(s + len));
			}
		}

		if (strlen(s) == 1 && s[0] == full[0]) {
			/* Shorthand with no arg */
			cmd->id = (enum Command) i;
			cmd->arg.type = ARG_NONE;
			return 1;
		}

		if (s[0] == full[0] && isspace(s[1])) {
			/* Shorthand with arg */
			cmd->id = (enum Command) i;
			return parse_arg(cmd, trim(s + 1));
		}
	}

	return 0; // Unknown command
}

/**
 * @brief Check if breakpoint exists at address pc
 * 
 * @param pc address to check for breakpoint at
 * @return 1 if yes, 0 if no
 */
int has_breakpoint(uint16_t pc) {
	return breakpoints[pc];
}

static int load_state(chip8_t *c8, const char *addr) {
	// TODO implement
	printf("Unimplemented\n");
	return 0;
}

/**
 * Load a file path string into cmd. This does not check
 * whether the file exists or can be read from.
 * 
 * @param cmd where to store the path
 * @param arg the argument to store
 * 
 * @return 1
 */
static int load_file_arg(cmd_t *cmd, char *arg) {
	cmd->arg.type = ARG_FILE;
	cmd->arg.value.s = trim(arg);
	return 1;
}

/**
 * @brief Parse arguments.
 * 
 * @param cmd where to store the argument (cmd->id must be correct)
 * @param s arg string (user input after command)
 */
static int parse_arg(cmd_t *cmd, char *s) {
	struct arg_s *arg = &cmd->arg;
	char *value;
	s = trim(s);

	if (cmd->id == CMD_SET) {
		/* Split attribute to set and value to set it to */
		value = s;
		while (!isspace(*value)) {
			value++;
		}

		if (*value == '\0') {
			return 0; // No value given
		}

		*value = '\0';
		value++;
		trim(value);

		cmd->setValue = parse_int(value);
	}


	if (cmd->id == CMD_LOAD || cmd->id == CMD_SAVE) {
		return load_file_arg(cmd, s);
	}

	if (s[0] == 'V') { // register
		if (strlen(s) > 1) {
			if (s[1] == 'K') { // VK
				arg->type = ARG_VK;
				return 1;
			} else { // Vx
				arg->type = ARG_V;
				arg->value.i = hex_to_int(s[1]);
				return 1;
			}
		} else { // V
			arg->type = ARG_V;
			arg->value.i = ARG_NONE;
			return 1;
		}
	} else if (s[0] == '$') { // address
		arg->type = ARG_ADDR;
		arg->value.i = parse_int(s);
		return arg->value.i > 0;
	} else { // other value
		for (int i = 0; i < (int) (sizeof(args) / sizeof(args[0])); i++) {
			if (!strcmp(s, args[i])) {
				arg->type = (enum Argument) i;
				arg->value.i = -1;
				return 1;
			}
		}
	}

	return 0; // Failed to load arg
}

/**
 * @brief Print the help string.
 */
static void print_help(void) {
	printf("%s\n", DEBUG_HELP_STRING);
}

/**
 * @brief Print all V registers (V0-Vf).
 * 
 * @param c8 the current CHIP-8 state
 */
static void print_v_registers(chip8_t *c8) {
	for (int i = 0; i < 8; i++) {
		printf("V%01x: 0x%03x\t\t", i, c8->V[i]);
		printf("V%01x: 0x%03x\n", i + 8, c8->V[i + 8]);
	}
}

/**
 * @brief Print all elements of the stack.
 * 
 * @param c8 the current CHIP-8 state
 */
static void print_stack(chip8_t *c8) {
	for (int i = 0; i < 8; i++) {
		printf("0x%01x: 0x%03x\t\t", i, c8->stack[i]);
		printf("0x%01x: 0x%03x\n", i + 8, c8->stack[i + 8]);
	}
}

/**
 * @brief Print the value specified by the arg in cmd.
 * 
 * @param c8 the current CHIP-8 state
 * @param cmd the command structure to get the arg from
 */
static void print_value(chip8_t *c8, cmd_t *cmd) {
	uint16_t pc;
	uint16_t ins;
	int addr;
	switch (cmd->arg.type)  {
		case ARG_NONE:
			pc = c8->pc;
			ins = (((uint16_t) c8->mem[pc]) << 8) | c8->mem[pc + 1];

			printf("0x%03x: 0x%04x\t%s\n", pc, ins, decode_instruction(ins, NULL));
			printf("PC: 0x%03x\t\tSP: 0x%03x\n", c8->pc, c8->sp);
			printf("DT: 0x%03x\t\tST: 0x%03x\n", c8->dt, c8->st);
			printf("I:  0x%03x\t\tK:  V%01x\n", c8->I, c8->VK);
			print_v_registers(c8);
			printf("Stack:\n");
			print_stack(c8);
			break;
		case ARG_SP:
			printf("SP: 0x%03x\n", c8->sp);
			break;
		case ARG_V:
			if (cmd->arg.value.i == -1) {
				print_v_registers(c8);
			} else {
				printf("V%x: 0x%03x\n", cmd->arg.value.i, c8->V[cmd->arg.value.i]);
			}
			break;
		case ARG_PC:
			printf("PC: 0x%03x\n", c8->pc);
			break;
		case ARG_DT:
			printf("DT: 0x%03x\n", c8->dt);
			break;
		case ARG_ST:
			printf("ST: 0x%03x\n", c8->st);
			break;
		case ARG_I:
			printf("I:  0x%03x\n", c8->I);
			break;
		case ARG_VK:
			printf("VK: V%03x\n", c8->VK);
			break;
		case ARG_STACK:
			print_stack(c8);
			break;
		case ARG_ADDR:
			addr = cmd->arg.value.i;
			printf("$%03x: 0x%03x\t%s\n", addr, c8->mem[addr], decode_instruction(c8->mem[addr], NULL));
			break;
		case ARG_FILE:
			break; // Should not be reached
	}
}

static int save_state(chip8_t *c8, const char *addr) {
	// TODO implement
	printf("Unimplemented\n");
	return 0;
}

/**
 * @brief Set the value at `cmd->arg.type` to `cmd->setValue
 * 
 * This assumes the arguments are correctly formatted (e.g. no `ARG_V` type
 * without a value).
 * 
 * @param c8 the current CHIP-8 state
 * @param cmd the command structure
 */
static int set_value(chip8_t *c8, cmd_t *cmd) {
	switch (cmd->arg.type) {
		case ARG_NONE: 
			return 0;
		case ARG_ADDR:
			c8->mem[cmd->arg.value.i] = cmd->setValue;
			return 1;
		case ARG_DT:
			c8->dt = cmd->setValue;
			return 1;
		case ARG_I:
			c8->I = cmd->setValue;
			return 1;
		case ARG_PC:
			c8->pc = cmd->setValue;
			return 1;
		case ARG_SP:
			c8->sp = cmd->setValue;
			return 1;
		case ARG_ST:
			c8->st = cmd->setValue;
			return 1;
		case ARG_V:
			c8->V[cmd->arg.value.i] = cmd->setValue;
			return 1;
		case ARG_VK:
			c8->V[c8->VK] = cmd->setValue;
			return 1;
		default:
			return 0;
	}
}
