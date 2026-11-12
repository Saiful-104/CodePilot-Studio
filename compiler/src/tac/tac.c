
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tac.h"

static int temp_count = 0;
static int label_count = 0;

/* Generate a new temporary variable. */
static char *new_temp(void) {
    char buffer[32];

    snprintf(buffer, sizeof(buffer), "t%d", ++temp_count);

    return strdup(buffer);
}

/* Generate a new label. */
static char *new_label(void) {
    char buffer[32];

    snprintf(buffer, sizeof(buffer), "L%d", ++label_count);

    return strdup(buffer);
}

/* Generate TAC for an expression and return its result. */
static char *generate_expression(ASTNode *node) {

    if (node == NULL) {
        return strdup("0");
    }

    /* Literal values */
    if (strcmp(node->label, "Integer") == 0 ||
        strcmp(node->label, "Float") == 0 ||
        strcmp(node->label, "Boolean") == 0 ||
        strcmp(node->label, "Identifier") == 0) {

        return strdup(node->value);
    }

    /* Unary minus and logical NOT */
    if (strcmp(node->label, "UnaryMinus") == 0 ||
        strcmp(node->label, "!") == 0) {

        char *operand = generate_expression(node->children[0]);
        char *result = new_temp();

        printf("%s = %s%s\n",
               result,
               strcmp(node->label, "UnaryMinus") == 0 ? "-" : "!",
               operand);

        free(operand);

        return result;
    }

    /* Binary expressions */
    if (node->child_count == 2) {

        char *left = generate_expression(node->children[0]);
        char *right = generate_expression(node->children[1]);
        char *result = new_temp();

        printf("%s = %s %s %s\n",
               result,
               left,
               node->label,
               right);

        free(left);
        free(right);

        return result;
    }

    return strdup("0");
}

/* Generate TAC for statements. */
static void generate_statement(ASTNode *node) {

    if (node == NULL) {
        return;
    }

    /* Process child statements in a block or program. */
    if (strcmp(node->label, "Program") == 0 ||
        strcmp(node->label, "Block") == 0) {

        for (int i = 0; i < node->child_count; i++) {
            generate_statement(node->children[i]);
        }

        return;
    }

    /* Variable declaration */
    if (strcmp(node->label, "Declaration") == 0) {

        if (node->child_count == 3) {

            char *value = generate_expression(node->children[2]);

            printf("%s = %s\n",
                   node->children[1]->value,
                   value);

            free(value);
        }

        return;
    }

    /* Assignment */
    if (strcmp(node->label, "Assignment") == 0) {

        char *value = generate_expression(node->children[1]);

        printf("%s = %s\n",
               node->children[0]->value,
               value);

        free(value);

        return;
    }

    /* Print statement */
    if (strcmp(node->label, "Print") == 0) {

        char *value = generate_expression(node->children[0]);

        printf("print %s\n", value);

        free(value);

        return;
    }

    /* If statement */
    if (strcmp(node->label, "If") == 0) {

        char *condition = generate_expression(node->children[0]);
        char *end_label = new_label();

        printf("ifFalse %s goto %s\n", condition, end_label);

        generate_statement(node->children[1]);

        printf("%s:\n", end_label);

        free(condition);
        free(end_label);

        return;
    }

    /* If-Else statement */
    if (strcmp(node->label, "If-Else") == 0) {

        char *condition = generate_expression(node->children[0]);
        char *else_label = new_label();
        char *end_label = new_label();

        printf("ifFalse %s goto %s\n", condition, else_label);

        generate_statement(node->children[1]);

        printf("goto %s\n", end_label);
        printf("%s:\n", else_label);

        generate_statement(node->children[2]);

        printf("%s:\n", end_label);

        free(condition);
        free(else_label);
        free(end_label);

        return;
    }

    /* While loop */
    if (strcmp(node->label, "While") == 0) {

        char *start_label = new_label();
        char *end_label = new_label();

        printf("%s:\n", start_label);

        char *condition = generate_expression(node->children[0]);

        printf("ifFalse %s goto %s\n", condition, end_label);

        generate_statement(node->children[1]);

        printf("goto %s\n", start_label);
        printf("%s:\n", end_label);

        free(condition);
        free(start_label);
        free(end_label);
    }
}

/* Public TAC generation function. */
void tac_generate(ASTNode *root) {

    temp_count = 0;
    label_count = 0;

    printf("\n===== THREE ADDRESS CODE =====\n");

    generate_statement(root);
}

/* Reset TAC generator resources. */
void tac_cleanup(void) {

    temp_count = 0;
    label_count = 0;
}
