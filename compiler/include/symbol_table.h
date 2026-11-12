#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

void symbol_table_init(void);
int symbol_declare(const char *name, const char *type, int line);
void scope_enter(void);
void scope_exit(void);
void symbol_table_print(void);
void symbol_table_cleanup(void);

#endif
