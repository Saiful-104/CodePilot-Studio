
CC = gcc
FLEX = flex
BISON = bison

CFLAGS = -Wall -Wextra -g -Iinclude

TARGET = codepilot

all: $(TARGET)

# Generate parser files using Bison
parser.tab.c parser.tab.h: src/parser/parser.y
	$(BISON) -d -o parser.tab.c src/parser/parser.y

# Generate lexer using Flex
lex.yy.c: src/lexer/lexer.l parser.tab.h
	$(FLEX) -o lex.yy.c src/lexer/lexer.l

# Compile main file
main.o: src/main.c include/ast.h include/symbol_table.h include/semantic.h
	$(CC) $(CFLAGS) -c src/main.c -o main.o

# Compile AST implementation
ast.o: src/ast/ast.c include/ast.h
	$(CC) $(CFLAGS) -c src/ast/ast.c -o ast.o

# Compile symbol table implementation
symbol_table.o: src/symbol_table/symbol_table.c include/symbol_table.h
	$(CC) $(CFLAGS) -c src/symbol_table/symbol_table.c -o symbol_table.o

# Compile semantic analyzer
semantic.o: src/semantic/semantic.c include/semantic.h include/ast.h
	$(CC) $(CFLAGS) -c src/semantic/semantic.c -o semantic.o

# Compile parser
parser.tab.o: parser.tab.c include/ast.h include/symbol_table.h
	$(CC) $(CFLAGS) -c parser.tab.c -o parser.tab.o

# Compile lexer
lex.yy.o: lex.yy.c
	$(CC) $(CFLAGS) -c lex.yy.c -o lex.yy.o

# Link all object files
$(TARGET): main.o ast.o symbol_table.o semantic.o parser.tab.o lex.yy.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o ast.o symbol_table.o semantic.o parser.tab.o lex.yy.o

# Remove generated files
clean:
	rm -f *.o parser.tab.c parser.tab.h lex.yy.c $(TARGET)

.PHONY: all clean
