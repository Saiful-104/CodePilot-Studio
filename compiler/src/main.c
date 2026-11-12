
#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "symbol_table.h"
#include "semantic.h"
#include "tac.h"

int yyparse(void);

extern ASTNode *root;

int main(void) {

    /* Initialize the symbol table. */
    symbol_table_init();

    printf("===== CodePilot Studio MiniLang Compiler =====\n");

    /* Start syntax analysis. */
    int parse_result = yyparse();

    if (parse_result != 0 || root == NULL) {

        printf("\nCompilation failed due to syntax errors.\n");

        symbol_table_cleanup();

        return EXIT_FAILURE;
    }

    printf("\nSyntax analysis completed successfully.\n");

    /* Display Abstract Syntax Tree. */
    printf("\n===== ABSTRACT SYNTAX TREE =====\n");

    ast_print(root, 0);

    /* Display Symbol Table. */
    symbol_table_print();

    /* Perform semantic analysis. */
    int semantic_result = semantic_analyze(root);

    /* Stop if semantic errors exist. */
    if (!semantic_result) {

        printf("\nCompilation failed due to semantic errors.\n");

        ast_free(root);
        root = NULL;

        symbol_table_cleanup();

        return EXIT_FAILURE;
    }

    /* Generate Three Address Code. */
    tac_generate(root);

    /* Release allocated memory. */
    tac_cleanup();

    ast_free(root);
    root = NULL;

    symbol_table_cleanup();

    printf("\nCompilation completed successfully.\n");

    return EXIT_SUCCESS;
}
