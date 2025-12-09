%option noyywrap

%{
    #include "ast.h"
    #include "types.h"
    #include "bison.tab.h"
    #include <stdlib.h>
    #include <string.h>
    #include <stdio.h>

    #define DEBUG_LEX 1
%}

DIGIT       [0-9]
LETTER      [a-zA-Z_]
ID          {LETTER}({LETTER}|{DIGIT})*
INT         {DIGIT}+
REAL        {DIGIT}+"."{DIGIT}+
STRING      \"([^\"\n])*\"
LISTA_DECL  {ID}"["{INT}"]"
COMMENT     \{[^}\n]*\}

%%


"PROGRAMA"  { if(DEBUG_LEX) printf("[LEX] PROGRAMA\n"); return PROGRAMA; }
"FIMPROG"   { if(DEBUG_LEX) printf("[LEX] FIMPROG\n"); return FIMPROG; }
"INTEIRO"   { if(DEBUG_LEX) printf("[LEX] INTEIRO\n"); yylval.type = N_INT; return INTEIRO; }
"REAL"      { if(DEBUG_LEX) printf("[LEX] REAL\n"); yylval.type = N_REAL; return REAL; }
"LISTAINT"  { if(DEBUG_LEX) printf("[LEX] LISTAINT\n"); yylval.type = LISTAINT; return LISTAINT; }
"LISTAREAL" { if(DEBUG_LEX) printf("[LEX] LISTAREAL\n"); yylval.type = LISTAREAL; return LISTAREAL; }

"LEIA"      { if(DEBUG_LEX) printf("[LEX] LEIA\n"); return LEIA; }
"ESCREVA"   { if(DEBUG_LEX) printf("[LEX] ESCREVA\n"); return ESCREVA; }

"SE"        { if(DEBUG_LEX) printf("[LEX] SE\n"); return SE; }
"ENTAO"     { if(DEBUG_LEX) printf("[LEX] ENTAO\n"); return ENTAO; }
"SENAO"     { if(DEBUG_LEX) printf("[LEX] SENAO\n"); return SENAO; }
"FIMSE"     { if(DEBUG_LEX) printf("[LEX] FIMSE\n"); return FIMSE; }

"ENQUANTO"  { if(DEBUG_LEX) printf("[LEX] ENQUANTO\n"); return ENQUANTO; }
"FACA"      { if(DEBUG_LEX) printf("[LEX] FACA\n"); return FACA; }
"FIMENQ"    { if(DEBUG_LEX) printf("[LEX] FIMENQ\n"); return FIMENQ; }

".NAO."     { if(DEBUG_LEX) printf("[LEX] NAO\n"); yylval.operand = R_NAO; return NAO; }
".E."       { if(DEBUG_LEX) printf("[LEX] E\n"); yylval.operand = R_E; return E; }
".OU."      { if(DEBUG_LEX) printf("[LEX] OU\n"); yylval.operand = R_OU; return OU; }
".MAQ."     { if(DEBUG_LEX) printf("[LEX] MAQ\n"); yylval.operand = R_MAQ; return MAQ; }
".MAI."     { if(DEBUG_LEX) printf("[LEX] MAI\n"); yylval.operand = R_MAI; return MAI; }
".MEQ."     { if(DEBUG_LEX) printf("[LEX] MEQ\n"); yylval.operand = R_MEQ; return MEQ; }
".MEI."     { if(DEBUG_LEX) printf("[LEX] MEI\n"); yylval.operand = R_MEI; return MEI; }
".IGU."     { if(DEBUG_LEX) printf("[LEX] IGU\n"); yylval.operand = R_IGU; return IGU; }
".DIF."     { if(DEBUG_LEX) printf("[LEX] DIF\n"); yylval.operand = R_DIF; return DIF; }

":="        { if(DEBUG_LEX) printf("[LEX] := \n"); return ATRIB; }
"+"         { if(DEBUG_LEX) printf("[LEX] + \n"); return '+'; }
"-"         { if(DEBUG_LEX) printf("[LEX] - \n"); return '-'; }
"*"         { if(DEBUG_LEX) printf("[LEX] * \n"); return '*'; }
"/"         { if(DEBUG_LEX) printf("[LEX] / \n"); return '/'; }
","         { if(DEBUG_LEX) printf("[LEX] , \n"); return ','; }
"("         { if(DEBUG_LEX) printf("[LEX] ( \n"); return '('; }
")"         { if(DEBUG_LEX) printf("[LEX] ) \n"); return ')'; }

{LISTA_DECL} {
    char *text = strdup(yytext);
    char *name = strtok(text, "[");
    char *size = strtok(NULL, "]");

    yylval.flex.name = strdup(name);
    yylval.flex.length = atoi(size);

    if(DEBUG_LEX)
        printf("[LEX] VAR_NAME (lista) name=%s size=%d\n",
               yylval.flex.name, yylval.flex.length);

    free(text);
    return VAR_NAME;
}

{INT} {
    yylval.integer = atoi(yytext);
    if(DEBUG_LEX) printf("[LEX] N_INT = %d\n", yylval.integer);
    return N_INT;
}

{REAL} {
    yylval.real = atof(yytext);
    if(DEBUG_LEX) printf("[LEX] N_REAL = %lf\n", yylval.real);
    return N_REAL;
}

{STRING} {
    yylval.string = strdup(yytext);
    if(DEBUG_LEX) printf("[LEX] STRING = %s\n", yylval.string);
    return STRING;
}

{ID} {
    yylval.string = strdup(yytext);
    if(DEBUG_LEX) printf("[LEX] VAR_NAME (simples) = %s\n", yylval.string);
    return VAR_NAME;
}

{COMMENT}   ; /* Ignore comments */

[ \t\r\n]+   ; /* Ignore whitespace */

. {
    printf("[LEX ERROR] Caractere inválido: %s\n", yytext);
}

%%