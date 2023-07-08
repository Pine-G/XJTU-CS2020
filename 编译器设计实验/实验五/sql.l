%{
#include "y.tab.h"
extern int yyerror(const char *, ...);
%}

%%
SELECT                  { return SELECT; }
FROM                    { return FROM; }
WHERE                   { return WHERE; }
AND                     { return AND; }
OR                      { return OR; }
NOT                     { return NOT; }
INSERT                  { return INSERT; }
INTO                    { return INTO; }
VALUES                  { return VALUES; }
UPDATE                  { return UPDATE; }
SET                     { return SET; }
DELETE                  { return DELETE; }
[1-9]+[0-9]*            { yylval.intval = atoi(yytext); return INTEGER; }
[a-zA-Z]+[0-9a-zA-Z_]*  { yylval.strval = strdup(yytext); return IDENTIFIER; }
'[^']*'                 { yylval.strval = strdup(yytext); return STRING; }
"<>"|"<"|"<="|">"|">="  { yylval.strval = strdup(yytext); return OP; }
"="                     { return '='; }
","                     { return ','; }
"*"                     { return '*'; }
"("                     { return '('; }
")"                     { return ')'; }
[ \t]+                  { /* ignore whitespace */ }
\n                      { return EOL; }
.                       { yyerror("Unknown character %c\n", *yytext);  }
%%

int yywrap() { return 1; }