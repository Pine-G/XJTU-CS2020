%{

#include <stdio.h>
#include <string.h>
#define KEYWORD_NUM 17
int line = 0;
enum {CLASS, INHERITS, IF, THEN, ELSE, FI, WHILE, LOOP, POOL, CASE, ESAC, OF, LET, IN, NEW, ISVOID, NOT};
char * key[] = {"class","inherits","if","then","else","fi","while","loop","pool","case","esac","of","let","in","new","isvoid","not"};
int keyword[KEYWORD_NUM] = {0};
void keyword_handle(char *str);
void keyword_show(void);

%}

KEYWORD     "class"|"inherits"|"if"|"then"|"else"|"fi"|"while"|"loop"|"pool"|"case"|"esac"|"of"|"let"|"in"|"new"|"isvoid"|"not"
TYPEID      [A-Z]+[_A-Za-z0-9]*
OBJECTID    [a-z]+[_a-zA-Z0-9]*
STRING      \"[^"\n]*\"
INTEGER     [0-9]+
WHITE       [ \t]+
LINE        \n
OPERATOR    "+"|"-"|"*"|"/"|"="|"<"|"<-"|"."|"~"
DELIMITERS  "("|")"|"{"|"}"|";"|","|":"
ERROR       .
%x MutiCom

%%

"--"[^\n]*              {/* do nothing */}
"(*"                    {BEGIN(MutiCom);}
<MutiCom>[^\n*]*        {/* do nothing */}
<MutiCom>"*"+[^*)\n]*   {/* do nothing */}
<MutiCom>\n             {++line;}
<MutiCom>"*"+")"        {BEGIN(0);}
{KEYWORD}               {keyword_handle(yytext);}
{TYPEID}                {printf("#%d TYPEID %s\n", line+1, yytext);}
{OBJECTID}              {printf("#%d OBJECTID %s\n", line+1, yytext);}
{STRING}                {printf("#%d %s\n", line+1, yytext);}
{INTEGER}               {printf("#%d %s\n", line+1, yytext);}
{WHITE}                 {/* do nothing */}
{LINE}                  {++line;}
{OPERATOR}              {printf("#%d '%s'\n", line+1, yytext);}
{DELIMITERS}            {printf("#%d '%s'\n", line+1, yytext);}
{ERROR}                 {printf("#%d ERROR \"%s\"\n", line+1, yytext);}

%%

void keyword_handle(char *str){
    if(!strcmp(str, "class")){
        printf("#%d CLASS\n", line + 1);
        keyword[CLASS]++;
    }else if(!strcmp(str, "inherits")){
        printf("#%d INHERITS\n", line + 1);
        keyword[INHERITS]++;
    }else if(!strcmp(str, "if")){
        printf("#%d IF\n", line + 1);
        keyword[IF]++;
    }else if(!strcmp(str, "then")){
        printf("#%d THEN\n", line + 1);
        keyword[THEN]++;
    }else if(!strcmp(str, "else")){
        printf("#%d ELSE\n", line + 1);
        keyword[ELSE]++;
    }else if(!strcmp(str, "fi")){
        printf("#%d FI\n", line + 1);
        keyword[FI]++;
    }else if(!strcmp(str, "while")){
        printf("#%d WHILE\n", line + 1);
        keyword[WHILE]++;
    }else if(!strcmp(str, "loop")){
        printf("#%d LOOP\n", line + 1);
        keyword[LOOP]++;
    }else if(!strcmp(str, "pool")){
        printf("#%d POOL\n", line + 1);
        keyword[POOL]++;
    }else if(!strcmp(str, "case")){
        printf("#%d CASE\n", line + 1);
        keyword[CASE]++;
    }else if(!strcmp(str, "esac")){
        printf("#%d ESAC\n", line + 1);
        keyword[ESAC]++;
    }else if(!strcmp(str, "of")){
        printf("#%d OF\n", line + 1);
        keyword[OF]++;
    }else if(!strcmp(str, "let")){
        printf("#%d LET\n", line + 1);
        keyword[LET]++;
    }else if(!strcmp(str, "in")){
        printf("#%d IN\n", line + 1);
        keyword[IN]++;
    }else if(!strcmp(str, "new")){
        printf("#%d NEW\n", line + 1);
        keyword[NEW]++;
    }else if(!strcmp(str, "isvoid")){
        printf("#%d ISVOID\n", line + 1);
        keyword[ISVOID]++;
    }else if(!strcmp(str, "not")){
        printf("#%d NOT\n", line + 1);
        keyword[NOT]++;
    }
}

void keyword_show(void){
    for(int i = 0; i < KEYWORD_NUM; i++)
        if(keyword[i])
            printf("%s: %d\n", key[i], keyword[i]);
}

int main(int argc, char** argv) {
    if (argc == 2) {
        if ((yyin = fopen(argv[1], "r")) == NULL) {
            printf("cannot open %s\n", argv[1]);
            return 0;
        }
    } else {
        printf("usage: %s <file>\n", argv[1]);
        return 0;
    }
    yylex();
    keyword_show();
    return 0;
}