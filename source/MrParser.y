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

%token VOID WCHAR BOOL CHAR SHORT INT
%token LONG LONGLONG INT128 FLOAT DOUBLE
%token CHAR32 CHAR16 AUTO NULLPTR
%token CONST VOLATILE
%token UNSIGNED SIGNED
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
 : VOID   {$$ = new FuncParam(); $$->type_e=BuiltinType::VOID;}
 | CHAR {$$ = new FuncParam(); $$->type_e=BuiltinType::CHAR;}
 | SHORT {$$ = new FuncParam(); $$->type_e=BuiltinType::SHORT;}
 | INT    {$$ = new FuncParam(); $$->type_e=BuiltinType::INT;}
 | LONG {$$ = new FuncParam(); $$->type_e=BuiltinType::LONG;}
 | FLOAT {$$ = new FuncParam(); $$->type_e=BuiltinType::FLOAT;}
 | DOUBLE {$$ = new FuncParam(); $$->type_e=BuiltinType::DOUBLE;}
 | UNSIGNED {$$ = new FuncParam(); $$->type_e=BuiltinType::INT;}
 | BOOL     {$$ = new FuncParam(); $$->type_e=BuiltinType::BOOL;}
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
