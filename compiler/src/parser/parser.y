%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "symbol_table.h"

extern int yylex(void);
extern int yylineno;

void yyerror(const char *message);

ASTNode *root = NULL;

static ASTNode *make_node(const char *label,
                          ASTNode *left,
                          ASTNode *right) {
    ASTNode *node = ast_create(label, NULL);
    ast_add_child(node, left);
    ast_add_child(node, right);
    return node;
}
%}

%define parse.error detailed
%locations

%union {
    char *str;
    ASTNode *node;
}

%token INT FLOAT BOOL IF ELSE WHILE PRINT
%token TRUE FALSE
%token <str> IDENTIFIER INTEGER_LITERAL FLOAT_LITERAL
%token INVALID

%type <node> program statement_list statement
%type <node> declaration assignment print_statement
%type <node> if_statement while_statement block expression
%type <str> type

%left OR
%left AND
%left EQ NE
%left '<' '>' LE GE
%left '+' '-'
%left '*' '/' '%'
%right NOT
%right UMINUS
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%

program:
    statement_list
    {
        root = $1;
        $$ = $1;
    }
    ;

statement_list:
    statement_list statement
    {
        ast_add_child($1, $2);
        $$ = $1;
    }
    | %empty
    {
        $$ = ast_create("Program", NULL);
    }
    ;

statement:
    declaration       { $$ = $1; }
    | assignment      { $$ = $1; }
    | print_statement { $$ = $1; }
    | if_statement    { $$ = $1; }
    | while_statement { $$ = $1; }
    | block           { $$ = $1; }
    ;

declaration:
    type IDENTIFIER ';'
    {
        symbol_declare($2, $1, @2.first_line);

        $$ = ast_create("Declaration", NULL);
        ast_add_child($$, ast_create("Type", $1));
        ast_add_child($$, ast_create("Identifier", $2));

        free($1);
        free($2);
    }
    | type IDENTIFIER '=' expression ';'
    {
        symbol_declare($2, $1, @2.first_line);

        $$ = ast_create("Declaration", NULL);
        ast_add_child($$, ast_create("Type", $1));
        ast_add_child($$, ast_create("Identifier", $2));
        ast_add_child($$, $4);

        free($1);
        free($2);
    }
    ;

type:
    INT   { $$ = strdup("int"); }
    | FLOAT { $$ = strdup("float"); }
    | BOOL  { $$ = strdup("bool"); }
    ;

assignment:
    IDENTIFIER '=' expression ';'
    {
        $$ = ast_create("Assignment", NULL);
        ast_add_child($$, ast_create("Identifier", $1));
        ast_add_child($$, $3);
        free($1);
    }
    ;

print_statement:
    PRINT '(' expression ')' ';'
    {
        $$ = ast_create("Print", NULL);
        ast_add_child($$, $3);
    }
    ;

if_statement:
    IF '(' expression ')' statement %prec LOWER_THAN_ELSE
    {
        $$ = ast_create("If", NULL);
        ast_add_child($$, $3);
        ast_add_child($$, $5);
    }
    | IF '(' expression ')' statement ELSE statement
    {
        $$ = ast_create("If-Else", NULL);
        ast_add_child($$, $3);
        ast_add_child($$, $5);
        ast_add_child($$, $7);
    }
    ;

while_statement:
    WHILE '(' expression ')' statement
    {
        $$ = ast_create("While", NULL);
        ast_add_child($$, $3);
        ast_add_child($$, $5);
    }
    ;

block:
    '{' { scope_enter(); } statement_list '}'
    {
        $$ = ast_create("Block", NULL);
        ast_add_child($$, $3);
        scope_exit();
    }
    ;

expression:
    INTEGER_LITERAL
    {
        $$ = ast_create("Integer", $1);
        free($1);
    }
    | FLOAT_LITERAL
    {
        $$ = ast_create("Float", $1);
        free($1);
    }
    | TRUE
    {
        $$ = ast_create("Boolean", "true");
    }
    | FALSE
    {
        $$ = ast_create("Boolean", "false");
    }
    | IDENTIFIER
    {
        $$ = ast_create("Identifier", $1);
        free($1);
    }
    | '(' expression ')' { $$ = $2; }
    | expression '+' expression { $$ = make_node("+", $1, $3); }
    | expression '-' expression { $$ = make_node("-", $1, $3); }
    | expression '*' expression { $$ = make_node("*", $1, $3); }
    | expression '/' expression { $$ = make_node("/", $1, $3); }
    | expression '%' expression { $$ = make_node("%", $1, $3); }
    | expression '<' expression { $$ = make_node("<", $1, $3); }
    | expression '>' expression { $$ = make_node(">", $1, $3); }
    | expression LE expression  { $$ = make_node("<=", $1, $3); }
    | expression GE expression  { $$ = make_node(">=", $1, $3); }
    | expression EQ expression  { $$ = make_node("==", $1, $3); }
    | expression NE expression  { $$ = make_node("!=", $1, $3); }
    | expression AND expression { $$ = make_node("&&", $1, $3); }
    | expression OR expression  { $$ = make_node("||", $1, $3); }
    | NOT expression
    {
        $$ = ast_create("!", NULL);
        ast_add_child($$, $2);
    }
    | '-' expression %prec UMINUS
    {
        $$ = ast_create("UnaryMinus", NULL);
        ast_add_child($$, $2);
    }
    ;

%%

void yyerror(const char *message) {
    fprintf(stderr, "Syntax Error at line %d: %s\n", yylineno, message);
}
