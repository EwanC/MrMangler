%{
#include <iostream>
#include <typeinfo>
#include <string>
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
  BuiltinType builtin;
}

%token VOID WCHAR BOOL CHAR SHORT INT
%token UCHAR USHORT UINT ULONG
%token LONG LONGLONG INT128 FLOAT DOUBLE
%token CHAR32 CHAR16 AUTO NULLPTR
%token CONST VOLATILE
%token UNSIGNED SIGNED
%token STRUCT UNION ENUM
%token<sval> STRING_LITERAL NUM_LITERAL
%token ELLIPSIS
%token STATIC

%type<decl> parameter_list parameter_type_list typed_decl named_decl
%type<decl> return_decl function_decl
%type<ast_node> parameter_declaration type_declaration signed_builtin abstract_declarator
%type<ast_node> array_type sized_array_type
%type<ast_node> qualified_user_t  qualified_abstract_decl  qualified_builtin
%type<ival> type_qualifier type_sign
%type<builtin> type_builtin
%type<sval> user_def_type

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
 | named_decl {$$ = $1; $$->return_val= new ASTBuiltin();}
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
 | type_declaration array_type {
                                 auto p = $2;
                                 while(p->pointee)
                                   p = p->pointee;
                                 p->pointee = $1;
                                 $$=$2;
                               }
 | type_declaration STRING_LITERAL {$$ = $1;}
 | type_declaration STRING_LITERAL array_type {
                                                auto p = $3;
                                                while(p->pointee)
                                                  p = p->pointee;
                                                p->pointee = $1;
                                                $$ = $3;
                                              }
 ;

array_type
 : '[' ']' {$$ = new ASTReference(ASTReference::PTR);}
 | '[' ']' sized_array_type {
                               $$ = new ASTArray(0);
                               auto a = static_cast<ASTArray*>($3);
                               a->pointee = $$;
                             }
 |  sized_array_type {$$ = $1;}
 ;

sized_array_type
 : '[' NUM_LITERAL ']' {auto size = std::stoul($2,nullptr); $$=new ASTArray(size);}
 |  sized_array_type '[' NUM_LITERAL ']' {
                                           auto size = std::stoul($3,nullptr);
                                           auto a = new ASTArray(size);
                                           a->pointee = $1;
                                           $$ = a;
                                         }
 ;

type_declaration
 : qualified_user_t  {$$=$1;}
 | qualified_builtin {$$=$1;}
 | qualified_user_t qualified_abstract_decl {
                                               auto r = static_cast<ASTReference*>($2);
                                               while (r->pointee)
                                                 r = static_cast<ASTReference*>(r->pointee);
                                               r->pointee = $1;
                                               $$ = $2;
                                             }

 | qualified_builtin qualified_abstract_decl {
                                               auto r = static_cast<ASTReference*>($2);
                                               while (r->pointee)
                                                 r = static_cast<ASTReference*>(r->pointee);
                                               r->pointee = $1;
                                               $$ = $2;
                                             }
 ;

qualified_abstract_decl
 : abstract_declarator type_qualifier {$$=$1; $$->quals = $$->quals | $2;}
 | abstract_declarator                {$$=$1;}
 ;

abstract_declarator
 : '*' {$$=new ASTReference(ASTReference::PTR);}
 | '&' {$$=new ASTReference(ASTReference::REF);}
 | qualified_abstract_decl '*' {auto r = new ASTReference(ASTReference::PTR);r->pointee = $1; $$ = r;}
 | qualified_abstract_decl '&' {auto r = new ASTReference(ASTReference::REF);r->pointee = $1; $$ = r;}
 ;

qualified_builtin
 : type_qualifier signed_builtin  {$$=$2; $$->quals = $$->quals | $1;}
 | signed_builtin type_qualifier  {$$=$1; $$->quals = $$->quals | $2;}
 | signed_builtin                 {$$ = $1;}
 ;

signed_builtin
 : type_sign type_builtin  {auto b = new ASTBuiltin(); b->type_e = $2; b->mods = b->mods | $1; $$ = b;}
 | type_builtin type_sign  {auto b = new ASTBuiltin(); b->type_e = $1; b->mods = b->mods | $2; $$ = b;}
 | type_builtin            {auto b = new ASTBuiltin(); b->type_e = $1; $$ = b;}
 ;

type_sign
 : UNSIGNED {$$=ASTBuiltin::UNSIGNED;}
 | SIGNED   {$$=ASTBuiltin::SIGNED;}
 ;

qualified_user_t
 : type_qualifier user_def_type  {$$ = new ASTUserType($2); $$->quals = $$->quals | $1;}
 | user_def_type type_qualifier  {$$ = new ASTUserType($1); $$->quals = $$->quals | $2;}
 | user_def_type                 {$$ = new ASTUserType($1);}
 ;

user_def_type
 : STRUCT STRING_LITERAL {$$=$2;}
 | UNION STRING_LITERAL  {$$=$2;}
 | ENUM STRING_LITERAL   {$$=$2;}
 | STRING_LITERAL        {$$=$1;}
 ;

type_qualifier
 : CONST    {$$=ASTNode::CONST;}
 | VOLATILE {$$=ASTNode::VOLATILE;}
 ;

type_builtin
 : VOID       {$$=BuiltinType::VOID;}
 | WCHAR      {$$=BuiltinType::WCHAR;}
 | BOOL       {$$=BuiltinType::BOOL;}
 | CHAR       {$$=BuiltinType::CHAR;}
 | UCHAR      {$$=BuiltinType::UCHAR;}
 | SHORT      {$$=BuiltinType::SHORT;}
 | USHORT     {$$=BuiltinType::USHORT;}
 | INT        {$$=BuiltinType::INT;}
 | UINT       {$$=BuiltinType::UINT;}
 | LONG       {$$=BuiltinType::LONG;}
 | ULONG      {$$=BuiltinType::ULONG;}
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
