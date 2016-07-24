%{
#include "decl_header.h"
#include "mangle.tab.h"
%}
%%
"char"  {return CHAR;}
"const" {return CONST;}
"double" {return DOUBLE;}
"float" {return FLOAT;}
"int" {return INT;}
"long" {return LONG;}
"short" {return SHORT;}
"unsigned" {return UNSIGNED;}
"void" {return VOID;}
"volatile" {return VOLATILE;}

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


[ \t\n]   ;
.   ;
%%
