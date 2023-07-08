%{

#include <stdio.h>
int nchar = 0, nword = 0, nline = 0;

%}

%%

[ \t]
\n          {++nline;}
[^ \t\n]+   {++nword; nchar += yyleng;}

%%

int main(){
    printf("Press CTRL+d to quit.\n");
    yylex();
    printf("nchar = %d, nword = %d, nline = %d\n", nchar, nword, nline);
    return 0;
}