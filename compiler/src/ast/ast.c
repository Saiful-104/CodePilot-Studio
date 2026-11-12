#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

static char *copy_string(const char *text) {
    if (!text) return NULL;

    char *copy = malloc(strlen(text) + 1);
    if (!copy) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    strcpy(copy, text);
    return copy;
}

ASTNode *ast_create(const char *label, const char *value) {
    ASTNode *node = calloc(1, sizeof(ASTNode));

    if (!node) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    node->label = copy_string(label);
    node->value = copy_string(value);
    return node;
}

void ast_add_child(ASTNode *parent, ASTNode *child) {
    if (!parent || !child) return;

    ASTNode **new_children = realloc(
        parent->children,
        sizeof(ASTNode *) * (parent->child_count + 1)
    );

    if (!new_children) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    parent->children = new_children;
    parent->children[parent->child_count++] = child;
}

void ast_print(ASTNode *node, int depth) {
    if (!node) return;

    for (int i = 0; i < depth; i++)
        printf("  ");

    printf("%s", node->label);

    if (node->value)
        printf(": %s", node->value);

    printf("\n");

    for (int i = 0; i < node->child_count; i++)
        ast_print(node->children[i], depth + 1);
}

void ast_free(ASTNode *node) {
    if (!node) return;

    for (int i = 0; i < node->child_count; i++)
        ast_free(node->children[i]);

    free(node->children);
    free(node->label);
    free(node->value);
    free(node);
}
