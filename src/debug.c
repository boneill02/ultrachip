#include "debug.h"

#include "chip8.h"
#include "decode.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


enum {
    CMD_ADD_BREAKPOINT,
    CMD_RM_BREAKPOINT,
    CMD_CONTINUE,
    CMD_NEXT,
    CMD_LOAD,
    CMD_SAVE,
    CMD_PRINT,
    CMD_HELP,
    CMD_QUIT,
    ARG_ADDR,
    ARG_V,
    ARG_SP,
    ARG_DT,
    ARG_ST,
    ARG_STACK,
    ARG_PC,
    ARG_I,
    ARG_VK,
    ARG_FILE,
    ARG_LIST,
};

typedef struct cmd_s {
    int id;
    int argid;
    union Arg {
        char *strValue;
        int intValue;
    } arg;
} cmd_t;

int get_command(cmd_t *, char *);
int load_state(chip8_t *, const char *);
int save_state(chip8_t *, const char *);
int parse_arg(cmd_t *, char *);
int parse_int(char *);
void print_help(void);
void print_value(chip8_t *, cmd_t *);
void set_value(chip8_t *, const char *);
char *trim(char *);

int breakpoints[MEMSIZE];

const char *cmds[] = {
    "break", // 0
    "rmbreak", // 1
    "continue", // 2
    "next", // 3
    "load", // 4, needs arg
    "save", // 5, needs arg
    "print", // 6
    "help", // 7
    "quit", // 8
};

int debug_repl(chip8_t *c8) {
    char buf[64];
    cmd_t cmd;

    printf("debug > ");
    while(scanf("%63s", buf)) {
        if (get_command(&cmd, buf)) {
            switch(cmd.id) {
                case CMD_ADD_BREAKPOINT: // add breakpoint
                    breakpoints[cmd.arg.intValue] = 1;
                    break;
                case CMD_RM_BREAKPOINT:
                    breakpoints[cmd.arg.intValue] = 0;
                    break;
                case CMD_CONTINUE:
                    return 1; // continue
                case CMD_NEXT:
                    return 2; // step
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
                    return 0;
             }
        } else {
            printf("Unknown command\n");
        }
        printf("debug > ");
    }
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
                parse_arg(&cmd, trim(s + len));
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
            parse_arg(&cmd, trim(s + 2));
            return 1;
        }
    }

    return 0;
}

int load_state(chip8_t *c8, const char *addr) {
    // TODO implement
}

int parse_arg(cmd_t *cmd, char *arg) {
    int id = cmd->id;
    if (cmd->id == CMD_LOAD || cmd->id == CMD_SAVE) {
        /* file */
        cmd->argid = ARG_FILE;
        cmd->arg.strValue = trim(arg);
    } else if (cmd->id == CMD_ADD_BREAKPOINT || cmd->id == CMD_RM_BREAKPOINT || cmd->id == CMD_PRINT) {
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
        // TODO use map here
        } else if (!strcmp(arg, "SP")) {
            cmd->argid = ARG_SP;
        } else if (!strcmp(arg, "PC")) {
            cmd->argid = ARG_PC;
        } else if (!strcmp(arg, "stack")) {
            cmd->argid = ARG_STACK;
        } else if (!strcmp(arg, "DT")) {
            cmd->argid = ARG_DT;
        } else if (!strcmp(arg, "ST")) {
            cmd->argid = ARG_ST;
        } else if (!strcmp(arg, "PC")) {
            cmd->argid = ARG_PC;
        } else if (!strcmp(arg, "I")) {
            cmd->argid = ARG_I;
        } else if (arg[0] == 'x') {
            cmd->argid = ARG_ADDR;
            cmd->arg.intValue = parse_int(arg);
        }
    }
    return 0;
}

int parse_int(char *s) {
    if (s[0] == 'x') {
        return strtol(s+1, NULL, 16);
    } else {
        return strtol(s, NULL, 10);
    }
}

void print_help(void) {
    // TODO implement
}

void print_value(chip8_t *c8, cmd_t *cmd) {
    int addr;
    switch (cmd->argid)  {
        case -1:
            addr = cmd->arg.intValue;
            printf("%03x: %03x\t%s\n", addr, c8->mem[addr], decode_instruction(c8->mem[addr], NULL));
            printf("PC: %03x\t\tSP: %03x\n", c8->pc, c8->sp);
            printf("DT: %03x\t\tST: %03x\n", c8->dt, c8->st);
            printf("I:  %03x\t\tK:  V%01x\n", c8->I, c8->VK);
            for (int i = 0; i < 16; i += 2) {
                printf("V%01x: %03x\t\t", i, c8->V[i]);
                printf("V%01x: %03x\n", i + 1, c8->V[i + 1]);
            }
            printf("Stack:\n");
            for (int i = 0; i < 16; i += 2) {
                printf("0x%01x: %03x\t\t", i, c8->stack[i]);
                printf("0x%01x: %03x\n", i + 1, c8->stack[i + 1]);
            }
            break;
        case ARG_SP:
            printf("SP: %03x\n", c8->sp);
            break;
        case ARG_V:
            printf("V%x: %03x\n", cmd->arg.intValue, c8->V[cmd->arg.intValue]);
            break;
        case ARG_PC:
            printf("PC: %03x", c8->pc);
            break;
        case ARG_DT:
            printf("DT: %03x", c8->dt);
            break;
        case ARG_ST:
            printf("ST: %03x", c8->st);
            break;
        case ARG_I:
            printf("I: %03x", c8->I);
            break;
        case ARG_VK:
            printf("K: V%03x", c8->VK);
            break;
        case ARG_STACK:
            for (int i = 0; i < STACK_SIZE; i++) {
                printf("%03x: %03x\n", i, c8->stack[i]);
            }
            break;
        case ARG_ADDR:
            int addr = cmd->arg.intValue;
            printf("%03x: %03x\t%s\n", addr, c8->mem[addr], decode_instruction(c8->mem[addr], NULL));
            break;
    }
}

int save_state(chip8_t *c8, const char *addr) {
    // TODO implement
}

char *trim(char *s) {
    char *end;

    while (isspace(s)) {
        s++;
    }

    end = s + strlen(s) - 1;
    while (end > s && isspace(end)) {
        end--;
    }

    *(end + 1) = '\0';
	return s;
}
