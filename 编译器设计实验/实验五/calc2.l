%{
# include "calc2.tab.h"
extern int yyerror(const char *, ...);
%}

%%
"true"   { return TRUE;  }
"false"  { return FALSE;  }

"||"     {return OR; }
"&&"     {return AND; }
"!"      {return NOT; }

"("      { return OP;  }
")"      { return CP;  }

\n       { return EOL;  }
[ \t]    { /* ignore white space */  }
.        { yyerror("Unknown character %c\n", *yytext);  }
%%

int yywrap() { return 1;  }