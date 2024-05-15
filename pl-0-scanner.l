/*Gruppenmitglieder Umut Dönmez 80080, Adrian Preisler 80035*/
%{
#include <stdio.h>
#include <string.h>
#include "AsmTreeGen.h"
#include "y.tab.h"

int comment_level = 0;
%}

%option nounput
%option noinput
%%


\(\*[^*]*\*\) {
    if (comment_level == 0) {
        comment_level = 1;
    } else {
        comment_level = 0;
    }
} 


\(\*.*\*\) {
    if (comment_level > 0) {
       /* nix soll ignorieren */
    }
}



[ \t\r\n\f]+               /* Whitespace ignorieren */
PROCEDURE|procedure      { return t_proc;  }
CONST|const              { return t_const; }
VAR|var                  { return t_var;}
BEGIN|begin              { return t_begin; }
IF|if                    { return t_if; }
WHILE|while              { return t_while; }
"?"|read|READ            { return t_read; }
"!"|write|WRITE          { return t_write; }
ODD|odd                  { return t_odd; }
END|end                  { return t_end; }
THEN|then                { return t_then; }
DO|do                    { return t_do; }
CALL|call                { return t_call; }
"."                      { return t_punkt; }
":="                     { return t_assign; }
"<="                     { return t_le; }
">="                     { return t_ge; }
"="                      { return t_eq; }
","                      { return t_komma; }
";"                      { return t_semik; }
"+"                      { return t_plus; }
"-"                      { return t_minus; }
"*"                      { return t_mult; }
"/"                      { return t_div; }
"("                      { return t_bra_o; }
")"                      { return t_bra_c; }
"<"                      { return t_lt; }
">"                      { return t_gt; }
"#"                      { return t_ne; }
[0-9]+                   { yylval.intval = atoi(yytext); 
                            return t_number;}
[a-zA-Z][a-zA-Z0-9]*     {  strcpy(yylval.strval, yytext);return t_ident; }
.                        { return t_error; }


%%

int yywrap()
{
	return 1;
}
