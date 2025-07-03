/**
 * @file c8/private/debug.c
 * @note NOT EXPORTED
 *
 * Stuff related to debug mode.
 */

#include "debug.h"

#include "../chip8.h"
#include "..//font.h"
#include "..//decode.h"
#include "exception.h"
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
typedef enum {
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
    CMD_LOADFLAGS,
    CMD_SAVEFLAGS,
} Command;

/**
 * @enum Argument
 * @brief Represents argument types
 *
 * This enumeration defines all possible debug mode arguments.
 */
typedef enum {
    ARG_NONE = -1,
    ARG_SP = 0,
    ARG_DT,
    ARG_ST,
    ARG_PC,
    ARG_I,
    ARG_VK,
    ARG_STACK,
    ARG_BG,
    ARG_FG,
    ARG_SFONT,
    ARG_BFONT,
    ARG_QUIRKS,
    ARG_V,
    ARG_R,
    ARG_ADDR,
    ARG_FILE,
} Argument;

/**
 * @union ArgValue
 * @brief Stores an argument's value (string or int)
 *
 * @param s string value
 * @param i int value
 */
typedef union {
    char* s;
    int i;
} ArgValue;

/**
 * @struct arg_t
 * @brief Represents an argument for a command with a type and value.
 *
 * @param type Argument type
 * @param value Argument value
 */
typedef struct {
    Argument type;
    ArgValue value;
} arg_t;

/**
 * @struct cmd_t
 * @brief Represents a command with an ID, argument ID, and associated argument.
 *
 * @param id command identifier
 * @param arg `arg_t` argument
 * @param setValue value to set `arg.value` to for set commands
 */
typedef struct {
    Command id;
    arg_t arg;
    int setValue;
} cmd_t;

static int get_command(cmd_t*, char*);
static int load_file_arg(cmd_t*, char*);
static void load_flags(c8_t*, const char*);
static void load_state(c8_t*, const char*);
static int parse_arg(cmd_t*, char*);
static void print_help(void);
static void print_r_registers(const c8_t*);
static void print_stack(const c8_t*);
static void print_v_registers(const c8_t*);
static void print_value(c8_t*, cmd_t*);
static void save_flags(const c8_t*, const char*);
static void save_state(c8_t*, const char*);
static int set_value(c8_t*, cmd_t*);

/**
 * These are string values of all possible argument, ordered to match the
 * Argument enumerator.
 */
const char* args[] = {
    "SP",
    "DT",
    "ST",
    "PC",
    "I",
    "VK",
    "stack",
    "bg",
    "fg",
    "sfont",
    "bfont",
    "quirks",
};

/**
 * These are string values of all possible commands, ordered to match the
 * Command enumerator.
 */
const char* cmds[] = {
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
    "loadflags",
    "saveflags",
};

/**
 * @brief Debug command line loop.
 *
 * This function parses user commands from stdin and prints the result until
 * one of the following conditions is met:
 *
 * - continue command is evaluated (return `DEBUG_CONTINUE`)
 *
 * - quit command is evaluated (return `DEBUG_QUIT`)
 *
 * - next command is evaluated (return `DEBUG_STEP`)
 *
 * @param c8 the current CHIP-8 state
 * @return `DEBUG_CONTINUE`, `DEBUG_STEP`, or `DEBUG_QUIT`
 */
int debug_repl(c8_t* c8) {
    char buf[64];
    cmd_t cmd;

    printf("debug > ");
    while (scanf("%63[^\n]", buf) != EOF) {
        if (get_command(&cmd, buf)) {
            switch (cmd.id) {
            case CMD_ADD_BREAKPOINT:
                if (cmd.arg.type == -1) {
                    c8->breakpoints[c8->pc] = 1;
                }
                else {
                    c8->breakpoints[cmd.arg.value.i] = 1;
                }
                break;
            case CMD_RM_BREAKPOINT:
                if (cmd.arg.value.i == -1) {
                    c8->breakpoints[c8->pc] = 0;
                }
                else {
                    c8->breakpoints[cmd.arg.value.i] = 0;
                }
                break;
            case CMD_CONTINUE: return DEBUG_CONTINUE;
            case CMD_NEXT: return DEBUG_STEP;
            case CMD_LOAD: load_state(c8, cmd.arg.value.s); break;
            case CMD_SAVE: save_state(c8, cmd.arg.value.s); break;
            case CMD_PRINT: print_value(c8, &cmd); break;
            case CMD_SET: set_value(c8, &cmd); break;
            case CMD_HELP: print_help(); break;
            case CMD_QUIT: return DEBUG_QUIT;
            case CMD_LOADFLAGS: load_flags(c8, cmd.arg.value.s); break;
            case CMD_SAVEFLAGS: save_flags(c8, cmd.arg.value.s); break;
            case CMD_NONE: printf("Invalid command\n"); break;
            }
        }
        else {
            printf("Invalid command\n");
        }
        printf("debug > ");

        // Consume newline
        if (getchar() == EOF) {
            break;
        }
    }

    return DEBUG_QUIT; // EOF
}

/**
 * @brief Check if breakpoint exists at address pc
 *
 * @param c8 `c8_t` to check breakpoints of
 * @param pc address to check for breakpoint at
 * @return 1 if yes, 0 if no
 */
int has_breakpoint(c8_t* c8, uint16_t pc) {
    return c8->breakpoints[pc];
}

/**
 * @brief Parse command from string `s` and store in `cmd`.
 *
 * @param cmd where to store the command attributes
 * @param s command string
 * @return 1 if successful, 0 if not
 */
static int get_command(cmd_t* cmd, char* s) {
    int numCmds = (int)sizeof(cmds) / sizeof(cmds[0]);

    /* reset cmd */
    cmd->id = CMD_NONE;
    cmd->arg.value.i = -1;
    cmd->arg.type = ARG_NONE;
    cmd->setValue = -1;

    s = trim(s);
    for (int i = 0; i < numCmds; i++) {
        const char* full = cmds[i];
        size_t len = strlen(full);

        if (!strncmp(s, full, len)) {
            /* Full cmd */
            cmd->id = (Command)i;
            if (s[len] == '\0') {
                /* No arg */
                cmd->arg.type = ARG_NONE;
                return 1;
            }
            else if (isspace(s[len])) {
                /* With arg */
                return parse_arg(cmd, trim(s + len));
            }
        }
    }

    return 0; // Unknown command
}

/**
 * @brief Load flag registers from file.
 *
 * @param c8 struct to load to
 * @param path path to load from
 */
static void load_flags(c8_t* c8, const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) {
        printf("Invalid file\n");
        return;
    }

    fread(&c8->R, 1, 8, f);
    fclose(f);
}

/**
 * @brief Load `c8_t` from file.
 *
 * @param c8 struct to load to
 * @param path path to load from
 */
static void load_state(c8_t* c8, const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) {
        printf("Invalid file\n");
        return;
    }
    fread(c8, sizeof(c8_t), 1, f);
    fclose(f);

    c8->draw = 1;
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
static int load_file_arg(cmd_t* cmd, char* arg) {
    cmd->arg.type = ARG_FILE;
    cmd->arg.value.s = trim(arg);
    return 1;
}

/**
 * @brief Parse arguments.
 *
 * @param cmd where to store the argument (cmd->id must be correct)
 * @param s arg string (user input after command)
 *
 * @return 1 if success, 0 otherwise
 */
static int parse_arg(cmd_t* cmd, char* s) {
    arg_t* arg = &cmd->arg;
    char* value = NULL;
    int argsCount = sizeof(args) / sizeof(args[0]);

    arg->type = ARG_NONE;

    if (cmd->id == CMD_SET) {
        /* Split attribute to set and value to set it to */
        for (size_t i = 0; i < strlen(s); i++) {
            if (isspace(s[i])) {
                s[i] = '\0';
                value = trim(&s[i + 1]);
            }
        }
    }

    /* Try to match with keywords */
    for (int i = 0; i < argsCount; i++) {
        if (!strcmp(s, args[i])) {
            printf("%s", s);
            arg->type = (Argument)i;
        }
    }

    if (cmd->id == CMD_SET) {
        if (!value || strlen(value) == 0) {
            printf("Not enough arguments.\n");
            return 0;
        }
        switch (arg->type) {
        case ARG_ADDR:
            cmd->arg.value.i = parse_int(s);
            cmd->setValue = parse_int(value);
            return 1;
        case ARG_QUIRKS:
        case ARG_SFONT:
        case ARG_BFONT: cmd->arg.value.s = value; break;
        default: cmd->setValue = parse_int(value); break;
        }
    }

    for (int i = 0; i < argsCount; i++) {
        if (!strcmp(s, args[i])) {
            arg->type = (Argument)i;
        }
    }

    switch (cmd->id) {
    case CMD_LOAD:
    case CMD_SAVE:
    case CMD_LOADFLAGS:
    case CMD_SAVEFLAGS: return load_file_arg(cmd, s);
    default: break;
    }

    switch (s[0]) {
    case 'V':
    case 'R':
        arg->type = s[0] == 'V' ? ARG_V : ARG_R;
        if (strlen(s) > 1) {
            arg->value.i = hex_to_int(s[1]);
        }
        else {
            arg->value.i = ARG_NONE;
        }
        break;
    case '$':
        arg->type = ARG_ADDR;
        arg->value.i = parse_int(s);
        return arg->value.i > 0;
    default: break;
    }

    return 1;
}

/**
 * @brief Print the help string.
 */
static void print_help(void) {
    printf("%s\n", DEBUG_HELP_STRING);
}

/**
 * @brief print quirk identifiers in `flags`
 *
 * @param flags flags to get enabled quirks from
 */
static void print_quirks(int flags) {
    int f = 0;
    printf("Quirks: ");
    if (flags & C8_FLAG_QUIRK_BITWISE) {
        f = 1;
        printf("b");
    }
    if (flags & C8_FLAG_QUIRK_DRAW) {
        f = 1;
        printf("d");
    }
    if (flags & C8_FLAG_QUIRK_JUMP) {
        f = 1;
        printf("j");
    }
    if (flags & C8_FLAG_QUIRK_LOADSTORE) {
        f = 1;
        printf("l");
    }
    if (flags & C8_FLAG_QUIRK_SHIFT) {
        f = 1;
        printf("s");
    }
    if (!f) {
        printf("None");
    }
    printf("\n");
}

/**
 * @brief Print all R (flag) registers.
 *
 * @param c8 the current CHIP-8 state
 */
static void print_r_registers(const c8_t* c8) {
    for (int i = 0; i < 4; i++) {
        printf("R%01x: %02x\t\t", i, c8->R[i]);
        printf("R%01x: %02x\n", i + 4, c8->R[i + 4]);
    }
}

/**
 * @brief Print all V registers (V0-Vf).
 *
 * @param c8 the current CHIP-8 state
 */
static void print_v_registers(const c8_t* c8) {
    for (int i = 0; i < 8; i++) {
        printf("V%01x: %02x\t\t", i, c8->V[i]);
        printf("V%01x: %02x\n", i + 8, c8->V[i + 8]);
    }
}

/**
 * @brief Print all elements of the stack.
 *
 * @param c8 the current CHIP-8 state
 */
static void print_stack(const c8_t* c8) {
    for (int i = 0; i < 8; i++) {
        printf("x%01x: $%03x\t\t", i, c8->stack[i]);
        printf("x%01x: $%03x\n", i + 8, c8->stack[i + 8]);
    }
}

/**
 * @brief Print the value specified by the arg in cmd.
 *
 * @param c8 the current CHIP-8 state
 * @param cmd the command structure to get the arg from
 */
static void print_value(c8_t* c8, cmd_t* cmd) {
    uint16_t pc;
    uint16_t ins;
    int addr;

    switch (cmd->arg.type) {
    case ARG_NONE:
        pc = c8->pc;
        ins = (((uint16_t)c8->mem[pc]) << 8) | c8->mem[pc + 1];

        printf("$%03x: %04x\t%s\n", pc, ins,
            c8_decode_instruction(ins, NULL));
        printf("PC: %03x\t\tSP: %02x\n", c8->pc, c8->sp);
        printf("DT: %02x\t\tST: %02x\n", c8->dt, c8->st);
        printf("I:  %03x\t\tK:  V%01x\n", c8->I, c8->VK);
        printf("BG: %06x\tFG: %06x\n", c8->colors[0], c8->colors[1]);
        print_fonts(c8);
        print_v_registers(c8);
        print_r_registers(c8);
        printf("Stack:\n");
        print_stack(c8);
        print_quirks(c8->flags);
        break;
    case ARG_SP:
        printf("SP: %02x\n", c8->sp);
        break;
    case ARG_V:
        if (cmd->arg.value.i == -1) {
            print_v_registers(c8);
        }
        else {
            printf("V%01x: %02x\n", cmd->arg.value.i, c8->V[cmd->arg.value.i]);
        }
        break;
    case ARG_R:
        if (cmd->arg.value.i == -1) {
            print_r_registers(c8);
        }
        else {
            printf("R%01x: %02x\n", cmd->arg.value.i, c8->R[cmd->arg.value.i]);
        }
        break;
    case ARG_PC: printf("PC: %03x\n", c8->pc); break;
    case ARG_DT: printf("DT: %02x\n", c8->dt); break;
    case ARG_ST: printf("ST: %02x\n", c8->st); break;
    case ARG_I: printf("I:  %03x\n", c8->I); break;
    case ARG_VK: printf("VK: V%01x\n", c8->VK); break;
    case ARG_BG: printf("BG: %06x\n", c8->colors[0]); break;
    case ARG_FG: printf("FG: %06x\n", c8->colors[1]); break;
    case ARG_BFONT: printf("BFONT: %s\n", c8_fontNames[1][c8->fonts[1]]); break;
    case ARG_SFONT: printf("SFONT: %s\n", c8_fontNames[0][c8->fonts[0]]); break;
    case ARG_QUIRKS: print_quirks(c8->flags); break;
    case ARG_STACK: print_stack(c8); break;
    case ARG_ADDR:
        addr = cmd->arg.value.i;
        printf("$%03x: %04x\t%s\n", addr, c8->mem[addr], c8_decode_instruction(c8->mem[addr], NULL));
        break;
    default: break; // Should not be reached
    }
}

/**
 * @brief Save flag registers to file.
 *
 * @param c8 `c8_t` to grab flag registers from
 * @param path path to save to
 */
static void save_flags(const c8_t* c8, const char* path) {
    FILE* f = fopen(path, "wb");
    if (!f) {
        printf("Invalid file\n");
        return;
    }

    fwrite(&c8->R, 1, 8, f);
    fclose(f);
}

/**
 * @brief Save `c8_t` to file.
 *
 * @param c8 `c8_t` to save
 * @param path path to save to
 */
static void save_state(c8_t* c8, const char* path) {
    FILE* f = fopen(path, "wb");
    if (!f) {
        printf("Invalid file\n");
        return;
    }

    fwrite(c8, sizeof(c8_t), 1, f);
    fclose(f);
}

/**
 * @brief Set the value at `cmd->arg.type` to `cmd->setValue`
 *
 * This assumes the arguments are correctly formatted (e.g. no `ARG_V` type
 * without a value).
 *
 * @param c8 the current CHIP-8 state
 * @param cmd the command structure
 */
static int set_value(c8_t* c8, cmd_t* cmd) {
    switch (cmd->arg.type) {
    case ARG_NONE:  return 0;
    case ARG_ADDR: c8->mem[cmd->arg.value.i] = cmd->setValue; return 1;
    case ARG_DT: c8->dt = cmd->arg.value.i; return 1;
    case ARG_I: c8->I = cmd->arg.value.i; return 1;
    case ARG_PC: c8->pc = cmd->arg.value.i; return 1;
    case ARG_SP: c8->sp = cmd->arg.value.i; return 1;
    case ARG_ST: c8->st = cmd->arg.value.i; return 1;
    case ARG_V: c8->V[cmd->arg.value.i] = cmd->setValue; return 1;
    case ARG_VK: c8->VK = cmd->arg.value.i; return 1;
    case ARG_BG: c8->colors[0] = cmd->arg.value.i; return 1;
    case ARG_FG: c8->colors[1] = cmd->arg.value.i; return 1;
    case ARG_QUIRKS: c8_load_quirks(c8, cmd->arg.value.s); return 1;
    case ARG_BFONT: c8_set_big_font(c8, cmd->arg.value.s); return 1;
    case ARG_SFONT: c8_set_small_font(c8, cmd->arg.value.s); return 1;
    default: printf("Invalid argument\n"); return 0;
    }
}
