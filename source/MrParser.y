%{
#include <iostream>
#include <typeinfo>
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
  ASTNode* ast_node;
  ASTUserType* ast_user;
  ASTBuiltin* ast_builtin;
  BuiltinType builtin;
}

%token VOID WCHAR BOOL CHAR SHORT INT
%token LONG LONGLONG INT128 FLOAT DOUBLE
%token CHAR32 CHAR16 AUTO NULLPTR
%token CONST VOLATILE
%token UNSIGNED SIGNED
%token STRUCT UNION ENUM
%token<sval> STRING_LITERAL
%token ELLIPSIS
%token STATIC

%type<decl> parameter_list parameter_type_list typed_decl named_decl
%type<decl> return_decl function_decl
%type<ast_node> parameter_declaration type_declaration
%type<ival> type_qualifier type_sign abstract_declarator
%type<builtin> type_builtin
%type<ast_user> user_def_type

%%

/* don't throw an error when reading files with multiple
   function signatures, but only return one */
function_decls
 : function_decl
 | function_decls function_decl
 ;

/* Accept static even though functions with internal
   linkage won't have symbols */
function_decl
 : return_decl             {func_decl = $1;}
 | return_decl ';'         {func_decl = $1;}
 | STATIC return_decl ';'  {func_decl = $2;}
 | STATIC return_decl      {func_decl = $2;}
 ;

return_decl
 : type_declaration named_decl {$$ = $2; $$->return_val=$1;}
 | named_decl {$$ = $1; $$->return_val= new ASTNode();}
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
                                auto param = new ASTBuiltin();
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
 : type_declaration  {$$ = $1;}
 | type_declaration STRING_LITERAL {$$ = $1;}
 ;

type_declaration
 : type_sign type_declaration {
    if(typeid($2) == typeid(ASTBuiltin))
    {
      ASTBuiltin* b = static_cast<ASTBuiltin*>($2);
      b->mods = b->mods | $1;
    }
 }
 | abstract_declarator type_declaration {auto r = new ASTReference((ASTReference::Indirection)$1); r->pointee = $2; $$ = r;}
 | type_qualifier type_declaration {$$=$2; $$->quals = $$->quals | $1;}
 | type_builtin {auto b = new ASTBuiltin(); b->type_e = $1; $$=  b;}
 | user_def_type {$$=$1;}
 ;

abstract_declarator
 : '*'                                    {$$=ASTReference::PTR;}
 | '&'                                    {$$=ASTReference::REF;}
 ;

type_sign
 : UNSIGNED {$$=ASTBuiltin::UNSIGNED;}
 | SIGNED   {$$=ASTBuiltin::SIGNED;}
 ;

type_qualifier
 : CONST    {$$=ASTNode::CONST;}
 | VOLATILE {$$=ASTNode::VOLATILE;}
 ;

user_def_type
 : STRUCT STRING_LITERAL {$$=new ASTUserType($2);}
 | UNION STRING_LITERAL  {$$=new ASTUserType($2);}
 | ENUM STRING_LITERAL   {$$=new ASTUserType($2);}
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
