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
%type<param> type_builtin type_modifier type_qualifier type_specifier parameter_declaration

%%

/* don't throw an error when reading files with multiple
   function signatures, but only return one */
function_decls
 : function_decl
 | function_decls function_decl
 ;

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
 : parameter_list ',' ELLIPSIS {$$ = $1;
                                auto param = new FuncParam();
                                param->type_e=BuiltinType::ELLIPSIS;
                                $$->params.push_back(param);}
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
 : type_qualifier {$$=$1;}
 | type_modifier  {$$=$1;}
 | type_builtin   {$$=$1;}
 ;

type_qualifier
 : CONST type_builtin     {$$=$2; $$->quals=FuncParam::CONST;}
 | VOLATILE type_builtin  {$$=$2; $$->quals=FuncParam::VOLATILE;}
 ;

type_modifier
 : UNSIGNED type_builtin {$$=$2; $$->mods=FuncParam::UNSIGNED;}
 | SIGNED type_builtin   {$$=$2; $$->mods=FuncParam::SIGNED;}
 | '*' type_builtin      {$$=$2; $$->mods=FuncParam::PTR;}
 | '&' type_builtin      {$$=$2; $$->mods=FuncParam::REFERENCE;}
 ;

type_builtin
 : VOID       {$$ = new FuncParam(); $$->type_e=BuiltinType::VOID;}
 | WCHAR      {$$ = new FuncParam(); $$->type_e=BuiltinType::WCHAR;}
 | BOOL       {$$ = new FuncParam(); $$->type_e=BuiltinType::BOOL;}
 | CHAR       {$$ = new FuncParam(); $$->type_e=BuiltinType::CHAR;}
 | SHORT      {$$ = new FuncParam(); $$->type_e=BuiltinType::SHORT;}
 | INT        {$$ = new FuncParam(); $$->type_e=BuiltinType::INT;}
 | LONG       {$$ = new FuncParam(); $$->type_e=BuiltinType::LONG;}
 | LONGLONG   {$$ = new FuncParam(); $$->type_e=BuiltinType::LONGLONG;}
 | INT128     {$$ = new FuncParam(); $$->type_e=BuiltinType::INT128;}
 | FLOAT      {$$ = new FuncParam(); $$->type_e=BuiltinType::FLOAT;}
 | DOUBLE     {$$ = new FuncParam(); $$->type_e=BuiltinType::DOUBLE;}
 | CHAR32     {$$ = new FuncParam(); $$->type_e=BuiltinType::CHAR32;}
 | CHAR16     {$$ = new FuncParam(); $$->type_e=BuiltinType::CHAR16;}
 | AUTO       {$$ = new FuncParam(); $$->type_e=BuiltinType::AUTO;}
 | NULLPTR    {$$ = new FuncParam(); $$->type_e=BuiltinType::NULLPTR;}
 ;

%%
FuncDecl* ParseStdin(FILE* file) {
#if YYDEBUG == 1
    yydebug=YYDEBUG;
#endif

    if (file)
      yyin = file;

    do {
      yyparse();
    } while (!feof(yyin));

    return func_decl;
}

void yyerror(const char*s) {
   printf("Parse error %s\n", s);
}
