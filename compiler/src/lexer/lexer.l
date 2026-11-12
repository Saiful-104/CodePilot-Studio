%option noyywrap yylineno

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "parser.tab.h"
#define YY_USER_ACTION yylloc.first_line = yylineno; yylloc.last_line = yylineno;
%}

DIGIT       [0-9]
LETTER      [a-zA-Z_]
IDENTIFIER  {LETTER}({LETTER}|{DIGIT})*
INT_NUM     {DIGIT}+
FLOAT_NUM   {DIGIT}+"."{DIGIT}+

%%

"int"        { return INT; }
"float"      { return FLOAT; }
"bool"       { return BOOL; }
"if"         { return IF; }
"else"       { return ELSE; }
"while"      { return WHILE; }
"print"      { return PRINT; }
"true"       { return TRUE; }
"false"      { return FALSE; }

"&&"         { return AND; }
"||"         { return OR; }
"=="         { return EQ; }
"!="         { return NE; }
"<="         { return LE; }
">="         { return GE; }
"!"          { return NOT; }

{FLOAT_NUM}  { yylval.str = strdup(yytext); return FLOAT_LITERAL; }
{INT_NUM}    { yylval.str = strdup(yytext); return INTEGER_LITERAL; }
{IDENTIFIER} { yylval.str = strdup(yytext); return IDENTIFIER; }

[ \t\r]+     ;
\n           ;
"//".*       ;

[(){};=+*/<>%-] { return yytext[0]; }

. {
    fprintf(stderr, "Invalid character '%s' at line %d\n", yytext, yylineno);
    return INVALID;
}

%%