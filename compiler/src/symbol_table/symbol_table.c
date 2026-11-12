#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"

typedef struct Symbol {
    char *name;
    char *type;
    int scope;
    int line;
    struct Symbol *next;
} Symbol;

static Symbol *head = NULL;
static int current_scope = 0;
static int next_scope = 1;

static int scope_stack[256];
static int scope_depth = 0;

void symbol_table_init(void) {
    head = NULL;
    current_scope = 0;
    next_scope = 1;
    scope_depth = 0;
}

int symbol_declare(const char *name, const char *type, int line) {
    for (Symbol *s = head; s; s = s->next) {
        if (s->scope == current_scope &&
            strcmp(s->name, name) == 0) {
            fprintf(stderr,
                "Semantic Error at line %d: '%s' already declared in this scope\n",
                line, name);
            return 0;
        }
    }

    Symbol *s = malloc(sizeof(Symbol));
    if (!s) exit(EXIT_FAILURE);

    s->name = strdup(name);
    s->type = strdup(type);
    s->scope = current_scope;
    s->line = line;
    s->next = head;
    head = s;

    return 1;
}

void scope_enter(void) {
    if (scope_depth >= 256) {
        fprintf(stderr, "Maximum scope depth exceeded\n");
        exit(EXIT_FAILURE);
    }

    scope_stack[scope_depth++] = current_scope;
    current_scope = next_scope++;
}

void scope_exit(void) {
    if (scope_depth == 0) return;

    current_scope = scope_stack[--scope_depth];
}

void symbol_table_print(void) {
    printf("\n===== SYMBOL TABLE =====\n");
    printf("%-15s %-10s %-10s %-10s\n",
           "Name", "Type", "Scope", "Line");

    for (Symbol *s = head; s; s = s->next) {
        printf("%-15s %-10s %-10d %-10d\n",
               s->name, s->type, s->scope, s->line);
    }
}

void symbol_table_cleanup(void) {
    Symbol *s = head;

    while (s) {
        Symbol *next = s->next;
        free(s->name);
        free(s->type);
        free(s);
        s = next;
    }

    head = NULL;
}
