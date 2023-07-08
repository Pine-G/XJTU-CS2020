%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
int yyerror(const char *, ...);
extern int yylex();
extern int yyparse();
%}

%union {
    int intval;
    char * strval;
}
%token SELECT FROM WHERE INSERT INTO VALUES UPDATE SET DELETE EOL
%token <intval> INTEGER
%token <strval> STRING
%token <strval> IDENTIFIER
%token <strval> OP
%left AND OR
%nonassoc NOT

%%

sql: /* nothing */
    | sql select_statement EOL { printf("> "); }
    | sql insert_statement EOL { printf("> "); }
    | sql update_statement EOL { printf("> "); }
    | sql delete_statement EOL { printf("> "); }
    | sql EOL { printf("> "); }
;

select_statement: select_clause from_clause where_clause
    { printf("Valid SELECT statement.\n"); }
;

select_clause: SELECT '*' { printf("SELECT:\nField: All\n"); }
    | SELECT IDENTIFIER { printf("SELECT:\nField: %s\n", $2); }
    | select_clause ',' IDENTIFIER { printf("Field: %s\n", $3); }
;

from_clause: FROM IDENTIFIER { printf("FROM:\nTable: %s\n", $2); }
    | from_clause ',' IDENTIFIER { printf("Table: %s\n", $3); }
;

where_clause: /* empty */ { printf("No WHERE clause.\n"); }
    | WHERE condition
;

condition: expr
    | NOT condition %prec NOT { printf("Condition: NOT\n"); }
    | condition AND condition { printf("Condition: AND\n"); }
    | condition OR condition { printf("Condition: OR\n"); }
;

expr: IDENTIFIER OP INTEGER { printf("Condition: %s %s %d\n", $1, $2, $3); }
    | IDENTIFIER OP STRING { printf("Condition: %s %s %s\n", $1, $2, $3); }
    | IDENTIFIER OP IDENTIFIER { printf("Condition: %s %s %s\n", $1, $2, $3); }
    | IDENTIFIER '=' INTEGER { printf("Condition: %s = %d\n", $1, $3); }
    | IDENTIFIER '=' STRING { printf("Condition: %s = %s\n", $1, $3); }
    | IDENTIFIER '=' IDENTIFIER { printf("Condition: %s = %s\n", $1, $3); }
;

insert_statement: insert_clause '(' field_list ')' VALUES '(' value_list ')'
    { printf("Valid INSERT statement.\n"); }
;

insert_clause: INSERT INTO IDENTIFIER
    { printf("INSERT:\nTable: %s\n", $3); }
;

field_list: IDENTIFIER { printf("Field: %s\n", $1); }
    | field_list ',' IDENTIFIER { printf("Field: %s\n", $3); }
;

value_list: INTEGER { printf("Value: %d\n", $1); }
    | STRING { printf("Value: %s\n", $1); }
    | value_list ',' INTEGER { printf("Value: %d\n", $3); }
    | value_list ',' STRING { printf("Value: %s\n", $3); }
;

update_statement: update_clause set_clause where_clause
    { printf("Valid UPDATE statement.\n"); }
;

update_clause: UPDATE IDENTIFIER
    { printf("UPDATE:\nTable: %s\n", $2); }
;

set_clause: SET IDENTIFIER '=' INTEGER { printf("Set: %s = %d\n", $2, $4); }
    | SET IDENTIFIER '=' STRING { printf("Set: %s = %s\n", $2, $4); }
    | set_clause ',' IDENTIFIER '=' INTEGER { printf("Set: %s = %d\n", $3, $5); }
    | set_clause ',' IDENTIFIER '=' STRING { printf("Set: %s = %s\n", $3, $5); }
;

delete_statement: delete_clause where_clause
    { printf("Valid DELETE statement.\n"); }
;

delete_clause: DELETE FROM IDENTIFIER
    { printf("DELETE:\nTable: %s\n", $3); }
;

%%

int main() {
    while(1) {
        printf("> ");
        yyparse();
        printf("\n");
    }
    return 0;
}

int yyerror(const char *s, ...)
{
    int ret;
    va_list va;
    va_start(va, s);
    ret = vfprintf(stderr, s, va);
    va_end(va);
    return ret;
}