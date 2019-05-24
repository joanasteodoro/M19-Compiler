%option c++ prefix="m19_scanner_" outfile="m19_scanner.cpp"
%option stack noyywrap yylineno 8bit debug
%{ 
// make relevant includes before including the parser's tab file
#include <string>
#include <algorithm>
#include <cmath>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>
#include "m19_parser.tab.h"

double strToFloatComma(std::string value, char t, int power){
    std::replace(value.begin(), value.end(), t, '.');
    double result = stod(value);
    return result * std::pow(10, power);  
}

// don't change this
#define yyerror LexerError
%}
%x X_SPECIALCHAR
%x X_STRING
%x X_IGNORE
%x X_COMMENT
%%

"//".*\n                 ; /* ignore comments */

">="                   return tGE; 
"<="                   return tLE; 
"=="                   return tEQ; 
"!="                   return tNE; 
"&&"                   return tAND;
"||"                   return tOR;


">>@"                  return tBREAK;
"@>>"                  return tCONTINUE;
">@<"                  return tRETURN;
"<<"                   return tINIT; //initial section
">>"                   return tFINAL; //final section
"!!"                   return tPRINTLN; 

[A-Za-z][A-Za-z0-9_]*  yylval.s = new std::string(yytext); return tIDENTIFIER;

\"                     yy_push_state(X_STRING); yylval.s = new std::string("");
<X_STRING>\"           yy_pop_state(); return tSTRING;
<X_STRING>\\           yy_push_state(X_SPECIALCHAR); 
<X_SPECIALCHAR>\"      yy_pop_state(); *yylval.s += "\"";
<X_SPECIALCHAR>"n"     yy_pop_state(); *yylval.s += "\n";
<X_SPECIALCHAR>"t"     yy_pop_state(); *yylval.s += "\t";
<X_SPECIALCHAR>"r"     yy_pop_state(); *yylval.s += "\r";
<X_SPECIALCHAR>0       yy_push_state(X_IGNORE);
<X_SPECIALCHAR>[0-9a-fA-F]{1,2}     yy_pop_state(); *yylval.s += (char)strtol(yytext,NULL,16);
<X_SPECIALCHAR>.       yy_pop_state(); *yylval.s += yytext; 
<X_STRING>.            *yylval.s += yytext;

<X_IGNORE>\"          yy_pop_state(); yy_pop_state(); yy_pop_state(); return tSTRING;
<X_IGNORE>.           ;

"/*"                   yy_push_state(X_COMMENT);
<X_COMMENT>"/*"        yy_push_state(X_COMMENT);
<X_COMMENT>"*/"        yy_pop_state();
<X_COMMENT>.           ; /* ignores whats inside the comment*/

"0x"[0-9a-fA-F]+       yylval.i = strtol(yytext+2,NULL,16); if (errno == ERANGE) yyerror("overflow"); return tINTEGER;

[0-9]+"a"[0-9]+        yylval.d = strToFloatComma(yytext, 'a', -18); return tDOUBLE;
[0-9]+"f"[0-9]+        yylval.d = strToFloatComma(yytext, 'f', -15); return tDOUBLE;
[0-9]+"p"[0-9]+        yylval.d = strToFloatComma(yytext, 'p', -12); return tDOUBLE;
[0-9]+"n"[0-9]+        yylval.d = strToFloatComma(yytext, 'n', -9); return tDOUBLE;
[0-9]+"u"[0-9]+        yylval.d = strToFloatComma(yytext, 'u', -6); return tDOUBLE;
[0-9]+"m"[0-9]+        yylval.d = strToFloatComma(yytext, 'm', -3); return tDOUBLE;
[0-9]+"k"[0-9]+        yylval.d = strToFloatComma(yytext, 'k', 3); return tDOUBLE;
[0-9]+"M"[0-9]+        yylval.d = strToFloatComma(yytext, 'M', 6); return tDOUBLE;
[0-9]+"G"[0-9]+        yylval.d = strToFloatComma(yytext, 'G', 9); return tDOUBLE;
[0-9]+"T"[0-9]+        yylval.d = strToFloatComma(yytext, 'T', 12); return tDOUBLE;
[0-9]+"P"[0-9]+        yylval.d = strToFloatComma(yytext, 'P', 15); return tDOUBLE;
[0-9]+"E"[0-9]+        yylval.d = strToFloatComma(yytext, 'E', 18); return tDOUBLE;
[0-9]+"."[0-9]+        yylval.d = std::stod(yytext); return tDOUBLE;

[0-9]+                 yylval.i = strtol(yytext, nullptr, 10); if (errno == ERANGE) yyerror("Lexical error: overflow") ;return tINTEGER;

[@\-()<>=+*/%;{}.,#:!$?{}\[\]]       return *yytext;

[ \t\n]+               ; /* ignore whitespace */

.                      yyerror("Unknown character");

%%
