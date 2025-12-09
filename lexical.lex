%option noyywrap

%{
    #include "ast.h"
    #include "types.h"
    #include "bison.tab.h"
    #include <stdlib.h>
    #include <string.h>
    #include <stdio.h>
%}

DIGIT       [0-9]
LETTER      [a-zA-Z_]
ID          {LETTER}({LETTER}|{DIGIT})*
INT         {DIGIT}+
REAL        {DIGIT}+"."{DIGIT}+
STRING      \"([^\"\n])*\"
LISTA_NUM   {ID}"["{INT}"]"
LISTA_VAR   {ID}"["{ID}"]"
COMMENT     \{[^}\n]*\}

%%


"PROGRAMA"  {
    #ifdef DEBUG
        printf("[LEX] PROGRAMA\n");
    #endif
    return PROGRAMA;
}

"FIMPROG"   {
    #ifdef DEBUG
        printf("[LEX] FIMPROG\n");
    #endif
    return FIMPROG;
}

"INTEIRO"   {
    #ifdef DEBUG
        printf("[LEX] INTEIRO\n");
    #endif
    yylval.type = T_INTEIRO;
    return INTEIRO;
}

"REAL"      {
    #ifdef DEBUG
        printf("[LEX] REAL\n");
    #endif
    yylval.type = T_REAL;
    return REAL;
}

"LISTAINT"  {
    #ifdef DEBUG
        printf("[LEX] LISTAINT\n");
    #endif
    yylval.type = T_LISTAINT;
    return LISTAINT;
}

"LISTAREAL" {
    #ifdef DEBUG
        printf("[LEX] LISTAREAL\n");
    #endif
    yylval.type = T_LISTAREAL;
    return LISTAREAL;
}

"LEIA"      {
    #ifdef DEBUG
        printf("[LEX] LEIA\n");
    #endif
    return LEIA;
}

"ESCREVA"   {
    #ifdef DEBUG
        printf("[LEX] ESCREVA\n");
    #endif
    return ESCREVA;
}

"SE"        {
    #ifdef DEBUG
        printf("[LEX] SE\n");
    #endif
    return SE;
}

"ENTAO"     {
    #ifdef DEBUG
        printf("[LEX] ENTAO\n");
    #endif
    return ENTAO;
}

"SENAO"     {
    #ifdef DEBUG
        printf("[LEX] SENAO\n");
    #endif
    return SENAO;
}

"FIMSE"     {
    #ifdef DEBUG
        printf("[LEX] FIMSE\n");
    #endif
    return FIMSE;
}

"ENQUANTO"  {
    #ifdef DEBUG
        printf("[LEX] ENQUANTO\n");
    #endif
    return ENQUANTO;
}

"FACA"      {
    #ifdef DEBUG
        printf("[LEX] FACA\n");
    #endif
    return FACA;
}

"FIMENQ"    {
    #ifdef DEBUG
        printf("[LEX] FIMENQ\n");
    #endif
    return FIMENQ;
}

".NAO."     {
    #ifdef DEBUG
        printf("[LEX] NAO\n");
    #endif
    yylval.operand = R_NAO;
    return NAO;
}

".E."       {
    #ifdef DEBUG
        printf("[LEX] E\n");
    #endif
    yylval.operand = R_E;
    return E;
}

".OU."      {
    #ifdef DEBUG
        printf("[LEX] OU\n");
    #endif
    yylval.operand = R_OU;
    return OU;
}

".MAQ."     {
    #ifdef DEBUG
        printf("[LEX] MAQ\n");
    #endif
    yylval.operand = R_MAQ;
    return MAQ;
}

".MAI."     {
    #ifdef DEBUG
        printf("[LEX] MAI\n");
    #endif
    yylval.operand = R_MAI;
    return MAI;
}

".MEQ."     {
    #ifdef DEBUG
        printf("[LEX] MEQ\n");
    #endif
    yylval.operand = R_MEQ;
    return MEQ;
}

".MEI."     {
    #ifdef DEBUG
        printf("[LEX] MEI\n");
    #endif
    yylval.operand = R_MEI;
    return MEI;
}

".IGU."     {
    #ifdef DEBUG
        printf("[LEX] IGU\n");
    #endif
    yylval.operand = R_IGU;
    return IGU;
}

".DIF."     {
    #ifdef DEBUG
        printf("[LEX] DIF\n");
    #endif
    yylval.operand = R_DIF;
    return DIF;
}

":="        {
    #ifdef DEBUG
        printf("[LEX] := \n");
    #endif
    return ATRIB;
}

"+"         {
    #ifdef DEBUG
        printf("[LEX] + \n");
    #endif
    return '+';
}

"-"         {
    #ifdef DEBUG
        printf("[LEX] - \n");
    #endif
    return '-';
}

"*"         {
    #ifdef DEBUG
        printf("[LEX] * \n");
    #endif
    return '*';
}

"/"         {
    #ifdef DEBUG
        printf("[LEX] / \n");
    #endif
    return '/';
}

","         {
    #ifdef DEBUG
        printf("[LEX] , \n");
    #endif
    return ',';
}

"("         {
    #ifdef DEBUG
        printf("[LEX] ( \n");
    #endif
    return '(';
}

")"         {
    #ifdef DEBUG
        printf("[LEX] ) \n");
    #endif
    return ')';
}

{LISTA_NUM} {
    char *text = strdup(yytext);
    char *name = strtok(text, "[");
    char *size = strtok(NULL, "]");

    yylval.flex.name = strdup(name);
    yylval.flex.length = atoi(size);
    yylval.flex.variable = NULL;

    #ifdef DEBUG
        printf("[LEX] VAR_NAME (lista) name=%s size=%d\n", yylval.flex.name, yylval.flex.length);
    #endif

    free(text);
    return VAR_NAME;
}

{LISTA_VAR} {
    char *text = strdup(yytext);
    char *name = strtok(text, "[");
    char *size_var = strtok(NULL, "]");

    yylval.flex.name = strdup(name);
    yylval.flex.length = 0;
    yylval.flex.variable = strdup(size_var);

    #ifdef DEBUG
        printf("[LEX] VAR_NAME (lista) name=%s size_var=%s\n", yylval.flex.name, yylval.flex.variable);
    #endif

    free(text);
    return VAR_NAME;
}

{INT} {
    yylval.integer = atoi(yytext);
    #ifdef DEBUG
        printf("[LEX] N_INT = %d\n", yylval.integer);
    #endif
    return N_INT;
}

{REAL} {
    yylval.real = atof(yytext);
    #ifdef DEBUG
        printf("[LEX] N_REAL = %lf\n", yylval.real);
    #endif
    return N_REAL;
}

{STRING} {
    yylval.string = strdup(yytext + 1);
    yylval.string[strlen(yytext) - 2] = '\0';
    #ifdef DEBUG
        printf("[LEX] STRING = %s\n", yylval.string);
    #endif
    return STRING;
}

{ID} {
    yylval.flex.name = strdup(yytext);
    yylval.flex.length = 0;
    yylval.flex.variable = NULL;

    #ifdef DEBUG
        printf("[LEX] VAR_NAME (simples) = %s\n", yylval.flex.name);
    #endif
    return VAR_NAME;
}

{COMMENT}   ; /* Ignore comments */

[ \t\r\n]+   ; /* Ignore whitespace */

. {
    printf("[LEX ERROR] Caractere inválido: %s\n", yytext);
}

%%