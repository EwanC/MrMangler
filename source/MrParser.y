%{
#include <iostream>
#include "FuncDecl.h"
#define YYDEBUG 0
void yyerror(const char *s);

// Defined in FLEX
extern "C" int yylex();
extern "C" int yyparse();
extern "C" int yydebug;
extern "C" FILE* yyin;

FuncDecl *func_decl = nullptr;
%}

%union {
  int ival;
  float fval;
  char *sval;
  FuncDecl* decl;
  FuncParam* param;
}

%token CHAR CONST VOID VOLATILE
%token DOUBLE FLOAT INT LONG SHORT UNSIGNED BOOL
%token<sval> STRING_LITERAL
%token ELLIPSIS

%type<decl> parameter_list parameter_type_list typed_decl named_decl
%type<decl> return_decl function_decl
%type<param> type_specifier parameter_declaration

%%
function_decl
 : return_decl      {func_decl = $1;}
 | return_decl ';'  {func_decl = $1;}
 ;

return_decl
 : type_specifier named_decl {$$ = $2;}
 | named_decl {$$ = $1;}
 ;

named_decl
 : STRING_LITERAL typed_decl {$$ = $2; $$->name = $1;}
 ;

typed_decl
 : '(' parameter_type_list ')' { $$ = $2;}
 | '(' ')' { $$ = new FuncDecl();}
 ;

parameter_type_list
 : parameter_list ',' ELLIPSIS {$$ = $1;}
 | parameter_list {$$ = $1;}
 ;

parameter_list
 : parameter_declaration {$$ = new FuncDecl(); $$->params.push_back($1);}
 | parameter_list ',' parameter_declaration
    {$$ = $1; $$->params.push_back($3); }
 ;

parameter_declaration
 : type_specifier  {$$ = $1;}
 | type_specifier STRING_LITERAL {$$ = $1;}
 ;

type_specifier
 : VOID   {$$ = new FuncParam(); $$->type_e=literal::VOID;}
 | CHAR {$$ = new FuncParam(); $$->type_e=literal::CHAR;}
 | SHORT {$$ = new FuncParam(); $$->type_e=literal::SHORT;}
 | INT    {$$ = new FuncParam(); $$->type_e=literal::INT;}
 | LONG {$$ = new FuncParam(); $$->type_e=literal::LONG;}
 | FLOAT {$$ = new FuncParam(); $$->type_e=literal::FLOAT;}
 | DOUBLE {$$ = new FuncParam(); $$->type_e=literal::DOUBLE;}
 | UNSIGNED {$$ = new FuncParam(); $$->type_e=literal::INT;}
 | BOOL     {$$ = new FuncParam(); $$->type_e=literal::BOOL;}
 ;

%%
FuncDecl* ParseStdin() {
#if YYDEBUG == 1
    yydebug=YYDEBUG;
#endif
    do {
      yyparse();
    } while (!feof(yyin));

    return func_decl;
}

void yyerror(const char*s) {
   printf("Parse error %s\n", s);
}
