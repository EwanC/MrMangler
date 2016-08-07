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
  char *sval;
  FuncDecl* decl;
  FuncParam* param;
  BuiltinType builtin;
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
%type<builtin> type_builtin
%type<ival> type_modifier type_qualifier

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
 : type_builtin                  {$$=new FuncParam(); $$->type_e = $1;}
 | type_qualifier type_specifier {$$=$2; $$->quals= $$->quals | $1;}
 | type_modifier  type_specifier {$$=$2; $$->mods= $$->mods | $1;}
 ;

type_qualifier
 : CONST    {$$=FuncParam::CONST;}
 | VOLATILE {$$=FuncParam::VOLATILE;}
 ;

type_modifier
 : UNSIGNED {$$=FuncParam::UNSIGNED;}
 | SIGNED   {$$=FuncParam::SIGNED;}
 | '*'      {$$=FuncParam::PTR;}
 | '&'      {$$=FuncParam::REFERENCE;}
 ;

type_builtin
 : VOID       {$$=BuiltinType::VOID;}
 | WCHAR      {$$=BuiltinType::WCHAR;}
 | BOOL       {$$=BuiltinType::BOOL;}
 | CHAR       {$$=BuiltinType::CHAR;}
 | SHORT      {$$=BuiltinType::SHORT;}
 | INT        {$$=BuiltinType::INT;}
 | LONG       {$$=BuiltinType::LONG;}
 | LONGLONG   {$$=BuiltinType::LONGLONG;}
 | INT128     {$$=BuiltinType::INT128;}
 | FLOAT      {$$=BuiltinType::FLOAT;}
 | DOUBLE     {$$=BuiltinType::DOUBLE;}
 | CHAR32     {$$=BuiltinType::CHAR32;}
 | CHAR16     {$$=BuiltinType::CHAR16;}
 | AUTO       {$$=BuiltinType::AUTO;}
 | NULLPTR    {$$=BuiltinType::NULLPTR;}
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
   printf("Exiting, parse error '%s'\n", s);
   exit(1);
}
