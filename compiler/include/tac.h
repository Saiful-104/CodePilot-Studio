
#ifndef TAC_H
#define TAC_H

#include "ast.h"

/*
 * Generate Three Address Code (TAC)
 * from the Abstract Syntax Tree.
 */
void tac_generate(ASTNode *root);

/* Release TAC generator resources. */
void tac_cleanup(void);

#endif
