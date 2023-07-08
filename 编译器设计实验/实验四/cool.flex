%{
#include <cool-parse.h>
#include <stringtab.h>
#include <utilities.h>

#define yylval cool_yylval
#define yylex  cool_yylex

#define MAX_STR_CONST 1025
#define YY_NO_UNPUT

extern FILE *fin;

#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
	if ( (result = fread( (char*)buf, sizeof(char), max_size, fin)) < 0) \
		YY_FATAL_ERROR( "read() in flex scanner failed");

char string_buf[MAX_STR_CONST];
char *string_buf_ptr;

extern int curr_lineno;
extern int verbose_flag;

extern YYSTYPE cool_yylval;

#include <string.h>

int uniqueIndex = 1;
int comment_num = 0;
int string_len = 0;
bool null_flag = false;
bool long_flag = false;

extern int if_fi;
extern int loop_pool;
extern int case_esac;
extern int pare1;
extern int pare2;
%}

%x MutiCom STRING

%%

--.*$               { /* do nothing */ }

"(*"                { comment_num++; BEGIN MutiCom; }
"*)"                { strcpy(cool_yylval.error_msg, "Unmatched *)"); return ERROR; }
<MutiCom>"(*"       { if (comment_num >= 0x7fffffff) {strcpy(cool_yylval.error_msg, "Too many comments");return ERROR;} comment_num++; }
<MutiCom>"*)"       { comment_num--; if (comment_num == 0) BEGIN 0; }
<MutiCom>\n         { curr_lineno++; }
<MutiCom><<EOF>>    { strcpy(cool_yylval.error_msg, "EOF in comment"); BEGIN 0; return ERROR; }
<MutiCom>.          { /* do nothing */ }

\n                  { curr_lineno++; }

[ \t\r\v\f\b]+      { /* do nothing */ }

"=>"                { return DARROW; }
"<-"                { return ASSIGN; }
"<="                { return LE; }

"("                 { pare1++; return '('; }
")"                 { pare1--; if (pare1 < 0) {pare1++;strcpy(cool_yylval.error_msg, "')' : Lack of '('");return ERROR;} return ')'; }
"{"                 { pare2++; return '{'; }
"}"                 { pare2--; if (pare2 < 0) {pare2++;strcpy(cool_yylval.error_msg, "'}' : Lack of '{'");return ERROR;} return '}'; }
":"                 { return ':'; }
";"                 { return ';'; }
"="                 { return '='; }
"<"                 { return '<'; }
"."                 { return '.'; }
","                 { return ','; }
"~"                 { return '~'; }
"+"                 { return '+'; }
"-"                 { return '-'; }
"*"                 { return '*'; }
"/"                 { return '/'; }
"@"                 { return '@'; }

(?i:class)          { return CLASS; }
(?i:else)           { return ELSE; }
(?i:fi)             { if_fi--; if (if_fi < 0) {if_fi++;strcpy(cool_yylval.error_msg, "FI : Lack of IF");return ERROR;} return FI; }
(?i:if)             { if_fi++; return IF; }
(?i:in)             { return IN; }
(?i:inherits)       { return INHERITS; }
(?i:let)            { return LET; }
(?i:loop)           { loop_pool++; return LOOP; }
(?i:pool)           { loop_pool--; if (loop_pool < 0) {loop_pool++;strcpy(cool_yylval.error_msg, "POOL : Lack of LOOP");return ERROR;} return POOL; }
(?i:then)           { return THEN; }
(?i:while)          { return WHILE; }
(?i:case)           { case_esac++; return CASE; }
(?i:esac)           { case_esac--; if (case_esac < 0) {case_esac++;strcpy(cool_yylval.error_msg, "ESAC : Lack of CASE");return ERROR;} return ESAC; }
(?i:of)             { return OF; }
(?i:new)            { return NEW; }
(?i:isvoid)         { return ISVOID; }
(?i:not)            { return NOT; }

t(?i:rue)           { cool_yylval.boolean = 1; return BOOL_CONST; }
f(?i:alse)          { cool_yylval.boolean = 0; return BOOL_CONST; }

"SELF_TYPE"         { cool_yylval.symbol = new IdEntry(yytext,strlen(yytext),uniqueIndex++); return TYPEID; }

"self"              { cool_yylval.symbol = new IdEntry(yytext,strlen(yytext),uniqueIndex++); return OBJECTID; }

[A-Z][A-Za-z0-9_]*  { cool_yylval.symbol = new IdEntry(yytext,strlen(yytext),uniqueIndex++); return TYPEID; }

[a-z][A-Za-z0-9_]*  { cool_yylval.symbol = new IdEntry(yytext,strlen(yytext),uniqueIndex++); return OBJECTID; }

[0-9]+              { cool_yylval.symbol = new IntEntry(yytext,strlen(yytext),uniqueIndex++); return INT_CONST; }

\"                  { string_buf_ptr = string_buf;
                      string_len = 0;
                      null_flag = false;
                      long_flag = false;
                      string_buf[string_len++] = '\"';
                      BEGIN STRING; }
<STRING>\n          { curr_lineno++;
                      strcpy(cool_yylval.error_msg, "Unterminated string constant");
                      BEGIN 0;
                      return ERROR; }
<STRING>\0          { null_flag = true; }
<STRING><<EOF>>     { strcpy(cool_yylval.error_msg, "EOF in string constant");
                      BEGIN 0;
                      return ERROR; }
<STRING>\\\n        { curr_lineno++; 
                      if (string_len >= MAX_STR_CONST - 1)
                          long_flag = true;
                      else
                          string_buf[string_len++] = '\n'; }
<STRING>\\.         { if (string_len >= MAX_STR_CONST - 1)
                          long_flag = true;
                      else
                          switch(yytext[1]) {
                              case '\"': string_buf[string_len++] = '\"'; break;
                              case '\\': string_buf[string_len++] = '\\'; break;
                              case 'b': string_buf[string_len++] = '\b'; break;
                              case 'f': string_buf[string_len++] = '\f'; break;
                              case 'n': string_buf[string_len++] = '\n'; break;
                              case 't': string_buf[string_len++] = '\t'; break;
                              default: string_buf[string_len++] = yytext[1];
                          } }
<STRING>\"          { BEGIN 0;
                      if (null_flag) {
                          strcpy(cool_yylval.error_msg, "String contains escaped null character");
                          return ERROR;
                      }
                      if (long_flag){
                          strcpy(cool_yylval.error_msg, "String constant too long");
                          return ERROR;
                      }
                      string_buf[string_len++] = '\"';
                      cool_yylval.symbol = new StringEntry(string_buf,string_len,uniqueIndex++);
                      return STR_CONST; }
<STRING>.           { if (string_len >= MAX_STR_CONST - 1)
                          long_flag = true;
                      else
                          string_buf[string_len++] = yytext[0]; }

.                   { cool_yylval.error_msg = yytext; return ERROR; }

%%

int yywrap()
{
   return 1;
}
