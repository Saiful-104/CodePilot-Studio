
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "semantic.h"

/* Stores information about a declared variable. */
typedef struct Symbol {
    char *name;
    char *type;
    int scope;
    struct Symbol *next;
} Symbol;

/* Stores parent-child relationships between scopes. */
typedef struct Scope {
    int id;
    int parent_id;
    struct Scope *next;
} Scope;

static Symbol *symbol_head = NULL;
static Scope *scope_head = NULL;

static int current_scope = 0;
static int next_scope_id = 1;
static int error_count = 0;

/* Creates a copy of a string. */
static char *copy_string(const char *text) {
    char *copy = malloc(strlen(text) + 1);

    if (copy == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    strcpy(copy, text);
    return copy;
}

/* Finds a scope by its ID. */
static Scope *find_scope(int id) {
    Scope *scope = scope_head;

    while (scope != NULL) {
        if (scope->id == id) {
            return scope;
        }

        scope = scope->next;
    }

    return NULL;
}

/* Enters a new nested scope. */
static void enter_scope(void) {
    Scope *scope = malloc(sizeof(Scope));

    if (scope == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    scope->id = next_scope_id++;
    scope->parent_id = current_scope;

    scope->next = scope_head;
    scope_head = scope;

    current_scope = scope->id;
}

/* Returns to the parent scope. */
static void exit_scope(void) {
    Scope *scope = find_scope(current_scope);

    if (scope != NULL) {
        current_scope = scope->parent_id;
    }
}

/* Finds a variable in the current scope or its parent scopes. */
static Symbol *find_symbol(const char *name) {
    int scope_id = current_scope;

    while (1) {
        Symbol *symbol = symbol_head;

        while (symbol != NULL) {
            if (symbol->scope == scope_id &&
                strcmp(symbol->name, name) == 0) {
                return symbol;
            }

            symbol = symbol->next;
        }

        if (scope_id == 0) {
            break;
        }

        Scope *scope = find_scope(scope_id);

        if (scope == NULL) {
            break;
        }

        scope_id = scope->parent_id;
    }

    return NULL;
}

/* Checks whether two types are compatible for assignment. */
static int is_compatible(const char *target, const char *source) {
    if (strcmp(target, source) == 0) {
        return 1;
    }

    /* An integer value can be assigned to a float variable. */
    if (strcmp(target, "float") == 0 &&
        strcmp(source, "int") == 0) {
        return 1;
    }

    return 0;
}

/* Reports a semantic error. */
static void report_error(const char *message, const char *name) {
    fprintf(stderr, "Semantic Error: %s '%s'\n", message, name);
    error_count++;
}

/* Checks an expression and returns its data type. */
static const char *check_expression(ASTNode *node) {
    if (node == NULL) {
        return "error";
    }

    /* Literal values. */
    if (strcmp(node->label, "Integer") == 0) {
        return "int";
    }

    if (strcmp(node->label, "Float") == 0) {
        return "float";
    }

    if (strcmp(node->label, "Boolean") == 0) {
        return "bool";
    }

    /* Variable usage. */
    if (strcmp(node->label, "Identifier") == 0) {
        Symbol *symbol = find_symbol(node->value);

        if (symbol == NULL) {
            report_error("Undeclared variable:", node->value);
            return "error";
        }

        return symbol->type;
    }

    /* Unary minus. */
    if (strcmp(node->label, "UnaryMinus") == 0) {
        const char *type = check_expression(node->children[0]);

        if (strcmp(type, "int") == 0 ||
            strcmp(type, "float") == 0) {
            return type;
        }

        if (strcmp(type, "error") != 0) {
            report_error("Unary minus requires a numeric value:", node->label);
        }

        return "error";
    }

    /* Logical NOT. */
    if (strcmp(node->label, "!") == 0) {
        const char *type = check_expression(node->children[0]);

        if (strcmp(type, "bool") == 0) {
            return "bool";
        }

        if (strcmp(type, "error") != 0) {
            report_error("Logical NOT requires a boolean value:", node->label);
        }

        return "error";
    }

    /* Binary expressions. */
    if (node->child_count == 2) {
        const char *left_type = check_expression(node->children[0]);
        const char *right_type = check_expression(node->children[1]);

        if (strcmp(left_type, "error") == 0 ||
            strcmp(right_type, "error") == 0) {
            return "error";
        }

        /* Arithmetic operators. */
        if (strcmp(node->label, "+") == 0 ||
            strcmp(node->label, "-") == 0 ||
            strcmp(node->label, "*") == 0 ||
            strcmp(node->label, "/") == 0 ||
            strcmp(node->label, "%") == 0) {

            int left_numeric =
                strcmp(left_type, "int") == 0 ||
                strcmp(left_type, "float") == 0;

            int right_numeric =
                strcmp(right_type, "int") == 0 ||
                strcmp(right_type, "float") == 0;

            if (!left_numeric || !right_numeric) {
                report_error("Invalid arithmetic operation:", node->label);
                return "error";
            }

            if (strcmp(node->label, "%") == 0 &&
                (strcmp(left_type, "int") != 0 ||
                 strcmp(right_type, "int") != 0)) {
                report_error("Modulo requires integer operands:", node->label);
                return "error";
            }

            if (strcmp(left_type, "float") == 0 ||
                strcmp(right_type, "float") == 0) {
                return "float";
            }

            return "int";
        }

        /* Relational operators. */
        if (strcmp(node->label, "<") == 0 ||
            strcmp(node->label, ">") == 0 ||
            strcmp(node->label, "<=") == 0 ||
            strcmp(node->label, ">=") == 0) {

            int left_numeric =
                strcmp(left_type, "int") == 0 ||
                strcmp(left_type, "float") == 0;

            int right_numeric =
                strcmp(right_type, "int") == 0 ||
                strcmp(right_type, "float") == 0;

            if (!left_numeric || !right_numeric) {
                report_error("Invalid relational operation:", node->label);
                return "error";
            }

            return "bool";
        }

        /* Equality operators. */
        if (strcmp(node->label, "==") == 0 ||
            strcmp(node->label, "!=") == 0) {

            if (is_compatible(left_type, right_type) ||
                is_compatible(right_type, left_type)) {
                return "bool";
            }

            report_error("Incompatible equality operands:", node->label);
            return "error";
        }

        /* Logical operators. */
        if (strcmp(node->label, "&&") == 0 ||
            strcmp(node->label, "||") == 0) {

            if (strcmp(left_type, "bool") == 0 &&
                strcmp(right_type, "bool") == 0) {
                return "bool";
            }

            report_error("Logical operators require boolean operands:",
                         node->label);
            return "error";
        }
    }

    return "error";
}

/* Checks statements in the AST. */
static void check_statement(ASTNode *node) {
    if (node == NULL) {
        return;
    }

    /* Variable declaration. */
    if (strcmp(node->label, "Declaration") == 0) {
        const char *type = node->children[0]->value;
        const char *name = node->children[1]->value;

        Symbol *existing = symbol_head;

        while (existing != NULL) {
            if (existing->scope == current_scope &&
                strcmp(existing->name, name) == 0) {
                report_error("Variable already declared:", name);
                return;
            }

            existing = existing->next;
        }

        Symbol *symbol = malloc(sizeof(Symbol));

        if (symbol == NULL) {
            fprintf(stderr, "Memory allocation failed.\n");
            exit(EXIT_FAILURE);
        }

        symbol->name = copy_string(name);
        symbol->type = copy_string(type);
        symbol->scope = current_scope;

        symbol->next = symbol_head;
        symbol_head = symbol;

        /* Check the optional initialization expression. */
        if (node->child_count == 3) {
            const char *value_type =
                check_expression(node->children[2]);

            if (strcmp(value_type, "error") != 0 &&
                !is_compatible(type, value_type)) {
                report_error("Type mismatch in declaration:", name);
            }
        }

        return;
    }

    /* Variable assignment. */
    if (strcmp(node->label, "Assignment") == 0) {
        const char *name = node->children[0]->value;
        Symbol *symbol = find_symbol(name);

        const char *value_type =
            check_expression(node->children[1]);

        if (symbol == NULL) {
            report_error("Undeclared variable:", name);
            return;
        }

        if (strcmp(value_type, "error") != 0 &&
            !is_compatible(symbol->type, value_type)) {
            report_error("Type mismatch in assignment:", name);
        }

        return;
    }

    /* Print statement. */
    if (strcmp(node->label, "Print") == 0) {
        check_expression(node->children[0]);
        return;
    }

    /* If statement. */
    if (strcmp(node->label, "If") == 0) {
        const char *condition =
            check_expression(node->children[0]);

        if (strcmp(condition, "bool") != 0 &&
            strcmp(condition, "error") != 0) {
            report_error("If condition must be boolean:", "if");
        }

        check_statement(node->children[1]);
        return;
    }

    /* If-else statement. */
    if (strcmp(node->label, "If-Else") == 0) {
        const char *condition =
            check_expression(node->children[0]);

        if (strcmp(condition, "bool") != 0 &&
            strcmp(condition, "error") != 0) {
            report_error("If condition must be boolean:", "if");
        }

        check_statement(node->children[1]);
        check_statement(node->children[2]);
        return;
    }

    /* While statement. */
    if (strcmp(node->label, "While") == 0) {
        const char *condition =
            check_expression(node->children[0]);

        if (strcmp(condition, "bool") != 0 &&
            strcmp(condition, "error") != 0) {
            report_error("While condition must be boolean:", "while");
        }

        check_statement(node->children[1]);
        return;
    }

    /* Nested block creates a new scope. */
    if (strcmp(node->label, "Block") == 0) {
        enter_scope();
        check_statement(node->children[0]);
        exit_scope();
        return;
    }

    /* Program or statement list. */
    for (int i = 0; i < node->child_count; i++) {
        check_statement(node->children[i]);
    }
}

/* Frees all semantic analysis data. */
static void cleanup_semantic_data(void) {
    Symbol *symbol = symbol_head;

    while (symbol != NULL) {
        Symbol *next = symbol->next;

        free(symbol->name);
        free(symbol->type);
        free(symbol);

        symbol = next;
    }

    symbol_head = NULL;

    Scope *scope = scope_head;

    while (scope != NULL) {
        Scope *next = scope->next;
        free(scope);
        scope = next;
    }

    scope_head = NULL;
}

/* Resets the semantic analyzer. */
void semantic_reset(void) {
    cleanup_semantic_data();

    current_scope = 0;
    next_scope_id = 1;
    error_count = 0;
}

/* Returns the total number of semantic errors. */
int semantic_error_count(void) {
    return error_count;
}

/* Starts semantic analysis. */
int semantic_analyze(ASTNode *root) {
    semantic_reset();

    if (root == NULL) {
        report_error("Invalid syntax tree:", "NULL");
        return 0;
    }

    printf("\n===== SEMANTIC ANALYSIS =====\n");

    check_statement(root);

    if (error_count == 0) {
        printf("Semantic analysis completed successfully.\n");
        return 1;
    }

    printf("Semantic analysis failed: %d error(s) found.\n",
           error_count);

    return 0;
}
