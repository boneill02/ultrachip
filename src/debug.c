#include "debug.h"

#include "chip8.h"
#include "decode.h"
#include "util.h"

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
 * `strValue` gets the string value.
 * `intValue` gets the integer value.
 */
union Arg {
    char *strValue;
    int intValue;
};

/**
 * @struct cmd_s
 * @brief Represents a command with an ID, argument ID, and associated argument.
 *
 * This structure defines a command that includes:
 * - the command identifier (`id`),
 * - an argument identifier (`argid`), and
 * - an argument value (`arg`) which can be a string or an integer.
 *
 * The `arg` member is a union that stores the actual argument data.
 */
struct cmd_s {
    enum Command id;
    enum Argument argid;
    union Arg arg;
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

bool breakpoints[MEMSIZE];

bool get_command(cmd_t *, char *);
bool load_address_arg(cmd_t *, char *);
bool load_file_arg(cmd_t *, char *);
bool load_print_arg(cmd_t *, char *);
bool load_state(chip8_t *, const char *);
bool save_state(chip8_t *, const char *);
bool parse_arg(cmd_t *, char *);
void print_help(void);
void print_value(chip8_t *, cmd_t *);
char *trim(char *);

const char *args[] = {
    "SP",
    "DT",
    "ST",
    "PC",
    "I",
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
                    if (cmd.argid == -1) {
                        breakpoints[c8->pc] = true;
                    } else {
                        breakpoints[cmd.arg.intValue] = true;
                    }
                    break;
                case CMD_RM_BREAKPOINT:
                    if (cmd.argid == -1) {
                        breakpoints[c8->pc] = false;
                    } else {
                        breakpoints[cmd.arg.intValue] = false;
                    }
                    break;
                case CMD_CONTINUE:
                    return DEBUG_CONTINUE;
                case CMD_NEXT:
                    return DEBUG_STEP;
                case CMD_LOAD:
                    load_state(c8, cmd.arg.strValue);
                    break;
                case CMD_SAVE:
                    save_state(c8, cmd.arg.strValue);
                    break;
                case CMD_PRINT:
                    print_value(c8, &cmd);
                    break;
                case CMD_HELP:
                    print_help();
                    break;
                case CMD_QUIT:
                    return DEBUG_QUIT;
                default:
                    // Unreachable
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
 * @return true if successful, false if not
 */
bool get_command(cmd_t *cmd, char *s) {
    size_t len;
    const char *full;
    int numCmds = (int) sizeof(cmds) / sizeof(cmds[0]);

    /* reset cmd */
    cmd->id = CMD_NONE;
    cmd->arg.intValue = -1;
    cmd->argid = ARG_NONE;

    s = trim(s);
    for (int i = 0; i < numCmds; i++) {
        full = cmds[i];
        len = strlen(full);

        if (!strncmp(s, full, len)) {
            /* Full cmd */
            cmd->id = (enum Command) i;
            if (s[len] == '\0') {
                /* No arg */
                cmd->argid = ARG_NONE;
                return 1;
            } else if (isspace(s[len])) {
                /* With arg */
                return parse_arg(cmd, trim(s + len));
            }
        }

        if (strlen(s) == 1 && s[0] == full[0]) {
            /* Shorthand with no arg */
            cmd->id = (enum Command) i;
            cmd->argid = ARG_NONE;
            return true;
        }

        if (s[0] == full[0] && isspace(s[1])) {
            /* Shorthand with arg */
            cmd->id = (enum Command) i;
            return parse_arg(cmd, trim(s + 1));
        }
    }

    return false; // Unknown command
}

/**
 * @brief Check if breakpoint exists at address pc
 * 
 * @param pc address to check for breakpoint at
 * @return true if yes, false if no
 */
bool has_breakpoint(uint16_t pc) {
    return breakpoints[pc];
}

/**
 * @brief Load a memory address into cmd
 * 
 * @param cmd where to store the address
 * @param arg argument string
 * 
 * @return true if successful (int can be parsed), false if not
 */
bool load_address_arg(cmd_t *cmd, char *arg) {
    cmd->argid = ARG_ADDR;
    return (cmd->arg.intValue = parse_int(arg));
}

bool load_state(chip8_t *c8, const char *addr) {
    // TODO implement
    printf("Unimplemented\n");
    return false;
}

/**
 * Load a file path string into cmd. This does not check
 * whether the file exists or can be read from.
 * 
 * @param cmd where to store the path
 * @param arg the argument to store
 * 
 * @return true
 */
bool load_file_arg(cmd_t *cmd, char *arg) {
    cmd->argid = ARG_FILE;
    cmd->arg.strValue = trim(arg);
    return true;
}

/**
 * Load a print arg into cmd.
 * 
 * @param cmd where to store the parsed arg
 * @param arg the argument to store
 * 
 * @return true if successfully parsed, false if not
 */
bool load_print_arg(cmd_t *cmd, char *arg) {
    arg = trim(arg);
    if (arg[0] == 'V') { // register
        if (strlen(arg) > 1) {
            if (arg[1] == 'K') { // print VK
                cmd->argid = ARG_VK;
                return true;
            } else { // Print Vx
                cmd->argid = ARG_V;
                cmd->arg.intValue = hex_to_int(arg[1]);
                return cmd->arg.intValue > -1; // return 0 if failed to parse int
            }
        } else { // print all V registers
            cmd->argid = ARG_V;
            cmd->arg.intValue = -1;
            return true;
        }
    } else if (arg[0] == '$') { // address
        cmd->argid = ARG_ADDR;
        cmd->arg.intValue = parse_int(arg);
        return 1;
    } else if (!strcmp(arg, "stack")) { // stack
        cmd->argid = ARG_STACK;
        return true;
    } else { // other value
        for (int i = 0; i < (int) (sizeof(args) / sizeof(args[0])); i++) {
            if (!strcmp(arg, args[i])) {
                cmd->argid = (enum Argument) i;
                return true;
            }
        }
    }
    return false; // invalid argument
}

/**
 * Parse an arg of any type.
 * 
 * @param cmd where to store the argument (cmd->id must be correct)
 * @param arg the argument to store
 */
bool parse_arg(cmd_t *cmd, char *arg) {
    switch (cmd->id) {
        case CMD_LOAD:
        case CMD_SAVE:
            return load_file_arg(cmd, arg);
            break;
        case CMD_ADD_BREAKPOINT:
        case CMD_RM_BREAKPOINT:
            return load_address_arg(cmd, arg);
            break;
        case CMD_PRINT:
            return load_print_arg(cmd, arg);
        default:
            break;
    }
    return 0;
}

/**
 * @brief Print the help string.
 */
void print_help(void) {
    printf("%s\n", DEBUG_HELP_STRING);
}

/**
 * @brief Print all V registers (V0-Vf).
 * 
 * @param c8 the current CHIP-8 state
 */
void print_v_registers(chip8_t *c8) {
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
void print_stack(chip8_t *c8) {
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
void print_value(chip8_t *c8, cmd_t *cmd) {
    uint16_t pc;
    uint16_t ins;
    int addr;
    switch (cmd->argid)  {
        case -1:
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
            if (cmd->arg.intValue == -1) {
                print_v_registers(c8);
            } else {
                printf("V%x: 0x%03x\n", cmd->arg.intValue, c8->V[cmd->arg.intValue]);
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
            addr = cmd->arg.intValue;
            printf("$%03x: 0x%03x\t%s\n", addr, c8->mem[addr], decode_instruction(c8->mem[addr], NULL));
            break;
        default:
            break;
    }
}

bool save_state(chip8_t *c8, const char *addr) {
    // TODO implement
    printf("Unimplemented\n");
    return false;
}
