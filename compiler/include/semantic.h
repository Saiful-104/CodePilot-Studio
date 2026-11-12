
#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"


int semantic_analyze(ASTNode *root);

int semantic_error_count(void);

void semantic_reset(void);

#endif
