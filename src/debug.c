#include "debug.h"

#include "chip8.h"
#include "decode.h"
#include "util.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct cmd_s {
    int id;
    int argid;
    union Arg {
        char *strValue;
        int intValue;
    } arg;
} cmd_t;

enum {
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

enum {
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
    ARG_LIST,
};

bool breakpoints[MEMSIZE];

int get_command(cmd_t *, char *);
int load_state(chip8_t *, const char *);
int save_state(chip8_t *, const char *);
int parse_arg(cmd_t *, char *);
int parse_int(char *);
void print_help(void);
void print_value(chip8_t *, cmd_t *);
void set_value(chip8_t *, const char *);
char *trim(char *);


const char *args[] = {
    "SP",
    "DT",
    "ST",
    "PC",
    "I",
    "VK",
};

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

int debug_repl(chip8_t *c8) {
    char buf[64];
    cmd_t cmd;

    printf("debug > ");
    while(scanf("%63[^\n]", buf)) {
        if (get_command(&cmd, buf)) {
            switch(cmd.id) {
                case CMD_ADD_BREAKPOINT:
                    breakpoints[cmd.arg.intValue] = true;
                    break;
                case CMD_RM_BREAKPOINT:
                    breakpoints[cmd.arg.intValue] = false;
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
             }
        } else {
            printf("Unknown command\n");
        }
        printf("debug > ");
        getchar(); // Consume newline
    }

    return DEBUG_QUIT; // EOF
}

int get_command(cmd_t *cmd, char *s) {
    size_t len;
    const char *full;
    size_t numCmds = sizeof(cmds) / sizeof(cmds[0]);

    s = trim(s);
    for (int i = 0; i < numCmds; i++) {
        full = cmds[i];
        len = strlen(full);

        if (!strncmp(s, full, len)) {
            /* Full cmd */
            cmd->id = i;
            if (s[len] == '\0') {
                /* No arg */
                cmd->argid = -1;
            } else if (isspace(s[len])) {
                /* With arg */
                parse_arg(cmd, trim(s + len));
            }
            return 1;
        }

        if (strlen(s) == 1 && s[0] == full[0]) {
            /* Shorthand with no arg */
            cmd->id = i;
            cmd->argid = -1;
            return 1;
        }

        if (s[0] == full[0] && isspace(s[1])) {
            /* Shorthand with arg */
            cmd->id = i;
            parse_arg(cmd, trim(s + 2));
            return 1;
        }
    }

    return 0;
}

bool has_breakpoint(uint16_t pc) {
    return breakpoints[pc];
}

int load_state(chip8_t *c8, const char *addr) {
    // TODO implement
}

int parse_arg(cmd_t *cmd, char *arg) {
    int id = cmd->id;
    if (id == CMD_LOAD || id == CMD_SAVE) {
        /* file */
        cmd->argid = ARG_FILE;
        cmd->arg.strValue = trim(arg);
    } else if (id == CMD_ADD_BREAKPOINT || id == CMD_RM_BREAKPOINT || id == CMD_PRINT) {
        if (arg[0] == 'x') {
            /* address */
            cmd->argid = ARG_ADDR;
            cmd->arg.intValue = parse_int(arg);
            return 1;
        }
    } else if (cmd->id == CMD_PRINT) {
        if (arg[0] == 'V') {
            if (arg[1] == 'K') {
                cmd->argid = ARG_VK;
            } else {
                cmd->argid = ARG_V;
                cmd->arg.intValue = strtol(arg, NULL, 16);
            }
            return 1;
        } else if (arg[0] == 'x') {
            cmd->argid = ARG_ADDR;
            cmd->arg.intValue = parse_int(arg);
            return 1;
        } else {
            for (int i = 0; i < (sizeof(args) / sizeof(args[0])); i++) {
                if (!strcmp(arg, args[i])) {
                    cmd->argid = i;
                    return 1;
                }
            }
        }
    }
    return 0;
}

void print_help(void) {
    // TODO implement
}

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
            for (int i = 0; i < 8; i++) {
                printf("V%01x: 0x%03x\t\t", i, c8->V[i]);
                printf("V%01x: 0x%03x\n", i + 8, c8->V[i + 8]);
            }
            printf("Stack:\n");
            for (int i = 0; i < 8; i++) {
                printf("0x%01x: 0x%03x\t\t", i, c8->stack[i]);
                printf("0x%01x: 0x%03x\n", i + 8, c8->stack[i + 8]);
            }
            break;
        case ARG_SP:
            printf("SP: 0x%03x\n", c8->sp);
            break;
        case ARG_V:
            printf("V%x: 0x%03x\n", cmd->arg.intValue, c8->V[cmd->arg.intValue]);
            break;
        case ARG_PC:
            printf("PC: 0x%03x", c8->pc);
            break;
        case ARG_DT:
            printf("DT: 0x%03x", c8->dt);
            break;
        case ARG_ST:
            printf("ST: 0x%03x", c8->st);
            break;
        case ARG_I:
            printf("I:  0x%03x", c8->I);
            break;
        case ARG_VK:
            printf("K: V%03x", c8->VK);
            break;
        case ARG_STACK:
            for (int i = 0; i < STACK_SIZE; i++) {
                printf("0x%02x: 0x%03x\n", i, c8->stack[i]);
            }
            break;
        case ARG_ADDR:
            addr = cmd->arg.intValue;
            printf("0x%03x: 0x%03x\t%s\n", addr, c8->mem[addr], decode_instruction(c8->mem[addr], NULL));
            break;
    }
}

int save_state(chip8_t *c8, const char *addr) {
    // TODO implement
}

