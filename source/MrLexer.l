%option noyywrap
%option never-interactive
%{
#include "FuncDecl.h"
#include "MrParser.h"

#define YY_NO_UNISTD_H
#define YY_DECL extern "C" int yylex()
%}
%%
"void"      {return VOID;}
"wchar_t"   {return WCHAR;}
"bool"      {return BOOL;}
"char"      {return CHAR;}
"int8_t"    {return SCHAR;}
"uint8_t"   {return UCHAR;}
"short"     {return SHORT;}
"int16_t"   {return SHORT;}
"uint16_t"  {return USHORT;}
"int"       {return INT;}
"int32_t"   {return INT;}
"uint32_t"  {return UINT;}
"long long" {return LONGLONG;}
"long"      {return LONG;}
"int64_t"   {return LONG;}
"uint64_t"  {return ULONG;}
"int128"    {return INT128;}
"float"     {return FLOAT;}
"double"    {return DOUBLE;}
"char32_t"  {return CHAR32;}
"char16_t"  {return CHAR16;}
"auto"      {return AUTO;}
"nullptr_t" {return NULLPTR;}

"const"    {return CONST;}
"volatile" {return VOLATILE;}

"unsigned" {return UNSIGNED;}
"signed"   {return SIGNED;}

"struct" {return STRUCT;}
"union"  {return UNION;}
"static" {return STATIC;}

[0-9]+ {yylval.sval = strdup(yytext); return NUM_LITERAL;}
[a-zA-Z0-9_]+ {yylval.sval = strdup(yytext); return STRING_LITERAL;}

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
