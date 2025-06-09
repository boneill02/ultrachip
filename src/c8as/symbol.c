#include "symbol.h"

#include "instruction.h"
#include "parse.h"
#include "util/defs.h"
#include "util/util.h"

#include <ctype.h>
#include <string.h>

static const char *identifierStrings[] = {
    "",
    S_DT,
    S_ST,
    S_I,
    S_IP,
    S_K,
    S_F,
    S_B,
    S_DB,
    S_DW,
    NULL,
};

static void reallocate_symbols(symbol_list_t *symbols);

/**
 * @brief Check if the given string is a comment
 * 
 * @param s the string to check
 * @return 1 if true, 0 if false
 */
int is_comment(char *s) {
    int i = 0;
    int len = strlen(s);

    for (; i < len; isspace(*s)) {
        s++;
        i++;
    }

    return i < len && *s == ';';
}

/**
 * @brief Check if given string is a DB identifier
 * 
 * @return 1 if true, 0 if false
 */
int is_db(char *s) {
    return !strcmp(s, S_DB);
}

/**
 * @brief Check if given string is a DW identifier
 * 
 * @return 1 if true, 0 if false
 */
int is_dw(char *s) {
    return !strcmp(s, S_DW);
}

/**
 * @brief Check if the given string is an instruction
 * 
 * @param s the string to check
 * @param len length of the string
 * @return instruction enumerator if true, -1 if false
 */
int is_instruction(char *s) {
    for (int i = 0; instructionStrings[i]; i++) {
        if (!strcmp(s, instructionStrings[i])) {
            return i;
        }
    }

    return -1;
}

/**
 * @brief Check if the given string is a label definition
 * 
 * @param s the string to check
 * @return 1 if true, 0 if false
 */
int is_label_definition(char *s) {
    int len = strlen(s);
    if (len < 2) {
        return 0;
    }

    return s[len-1] == ':';
}

/**
 * @brief Check if given string is a label reference
 * 
 * @param s string to check
 * @return label index if true, -1 otherwise
 */
int is_label(char *s, label_list_t *labels) {
    for (int i = 0; i < labels->len; i++) {
        if (!strcmp(s, labels->l[i].identifier)) {
            return i;
        }
    }

    return -1;
}

/**
 * @brief Check if the given string represents a V register
 * 
 * @param s string to check
 * @return V register number if true, -1 otherwise
 */
int is_register(char *s) {
    if (*s == 'V' || *s == 'v') {
        return parse_int(s);
    }

    return -1;
}

/**
 * @brief Check if given string is a reserved identifier
 * 
 * @param s string to check
 * @return type of identifier if true, -1 otherwise
 */
int is_reserved_identifier(char *s) {
    for (int i = 0; identifierStrings[i]; i++) {
        if (!strcmp(s, identifierStrings[i])) {
            return i;
        }
    }

    return -1;
}

/**
 * @brief Get the next symbol
 * 
 * @return first empty symbol in symbol table
 */
symbol_t *next_symbol(symbol_list_t *symbols) {
    symbols->len++;
    if (symbols->len == symbols->ceil) {
        reallocate_symbols(symbols);
    }

    return &symbols->s[symbols->len];
}

/**
 * @brief Populate label list from lines
 * 
 * @param lines lines to search
 * @param lineCount number of lines to search
 * @param labels label list to populate
 * 
 * @return 1 if success, 0 if too many labels exist in source
 */
int populate_labels(char **lines, int lineCount, label_list_t *labels) {
    for (int i = 0; i < lineCount; i++) {
        if (labels->len == labels->ceil) {
            return 0;
        }

        lines[i] = trim_comment(lines[i]);
        if (is_label_definition(lines[i])) {
            strncpy(labels->l[labels->len].identifier, lines[i], LABEL_IDENTIFIER_SIZE);
            labels->len++;
        }
    }
    return 1;
}

/**
 * @brief Get byte indexes of label definitions from completed symbol table
 * 
 * @param symbols list of symbols
 * @param labels list of labels
 */
void resolve_labels(symbol_list_t *symbols, label_list_t *labels) {
    int byte = PROG_START;
    int labelIdx = 0;
    for (int i = 0; i < symbols->len; i++) {
        if (labelIdx == labels->len) {
            return;
        }

        switch (symbols->s[i].type) {
            case SYM_LABEL_DEFINITION:
                labels->l[labelIdx++].byte = byte;
                break;
            case SYM_DB:
                byte += 1;
                break;
            case SYM_INSTRUCTION:
            case SYM_DW:
                byte += 2;
            default:
                break;
        }
    }
}

/**
 * @brief Substitute label symbols with their corresponding int value
 * 
 * @param symbols symbols to search
 * @param labels labels to search
 */
void substitute_labels(symbol_list_t *symbols, label_list_t *labels) {
    for (int i = 0; i < symbols->len; i++) {
        if (symbols->s[i].type == SYM_LABEL) {
            symbols->s[i].type = SYM_INT;
            symbols->s[i].value = labels->l[symbols->s[i].value].byte;
        }
    }
}
/**
 * @brief Expand symbol list
 * 
 * @param symbols symbol list
 */
static void reallocate_symbols(symbol_list_t *symbols) {
    int newCeiling = symbols->ceil + SYMBOL_CEILING;
    symbol_t *oldsym = symbols->s;
    symbols->s = (symbol_t *) malloc(sizeof(symbol_t) * newCeiling);
    memcpy(symbols->s, oldsym, symbols->ceil);
    symbols->ceil = newCeiling;
    free(oldsym);
}
