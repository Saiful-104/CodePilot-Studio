
#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "symbol_table.h"
#include "semantic.h"

int yyparse(void);

extern ASTNode *root;

int main(void) {

    /* Initialize the parser's symbol table. */
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

    /* Display the parser's symbol table. */
    symbol_table_print();

    /* Perform semantic analysis. */
    int semantic_result = semantic_analyze(root);

    /* Free AST memory. */
    ast_free(root);
    root = NULL;

    /* Clean up the parser's symbol table. */
    symbol_table_cleanup();

    /* Check final compilation result. */
    if (!semantic_result) {

        printf("\nCompilation failed due to semantic errors.\n");

        return EXIT_FAILURE;
    }

    printf("\nCompilation completed successfully.\n");

    return EXIT_SUCCESS;
}
