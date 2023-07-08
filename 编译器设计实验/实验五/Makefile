CC = gcc
LEX = flex
YACC = yacc
LIBS = -ll

all: sql

sql: y.tab.c lex.yy.c
	$(CC) -o $@ $^ $(LIBS)

y.tab.c: sql.y
	$(YACC) -d $<

lex.yy.c: sql.l
	$(LEX) $<

clean:
	rm -f sql y.tab.c y.tab.h lex.yy.c