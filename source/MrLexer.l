%option noyywrap
%{
#include "FuncDecl.h"
#include "MrParser.h"

#define YY_DECL extern "C" int yylex()
%}
%%
"void"      {return VOID;}
"wchar_t"   {return WCHAR;}
"bool"      {return BOOL;}
"char"      {return CHAR;}
"short"     {return SHORT;}
"int"       {return INT;}
"long long" {return LONGLONG;}
"long"      {return LONG;}
"int128"    {return INT128;}
"float"     {return FLOAT;}
"double"    {return DOUBLE;}
"char32_t"  {return CHAR32;}
"char16_t"  {return CHAR16;}
"auto"      {return AUTO;}
"nullptr_t" {return NULLPTR;}

"const" {return CONST;}
"volatile" {return VOLATILE;}

"unsigned" {return UNSIGNED;}
"signed" {return SIGNED;}

[a-zA-Z0-9]+ {yylval.sval = strdup(yytext); return STRING_LITERAL;}

"..." {return ELLIPSIS;}
"*" { return '*';}
";" { return ';';}
"," { return ',';}
"(" { return '(';}
")" { return ')';}
"[" { return '[';}
"]" { return ']';}
"{" { return '{';}
"}" { return '}';}
"&" { return '&';}


[ \t\n]   ;
.   ;
%%