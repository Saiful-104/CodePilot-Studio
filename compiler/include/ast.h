#ifndef AST_H
#define AST_H

typedef struct ASTNode {
    char *label;
    char *value;
    struct ASTNode **children;
    int child_count;
} ASTNode;

ASTNode *ast_create(const char *label, const char *value);
void ast_add_child(ASTNode *parent, ASTNode *child);
void ast_print(ASTNode *node, int depth);
void ast_free(ASTNode *node);

#endif
