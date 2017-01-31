%{
#include <iostream>
#include <typeinfo>
#include <string>
#include "FuncDecl.h"

#define YYDEBUG 0

void yyerror(const char* s);

// Defined in FLEX
extern "C" int yylex();
extern "C" int yyparse();
extern "C" int yydebug;

FuncDecl* func_decl = nullptr;
%}

%union {
  int ival;
  char* sval;
  FuncDecl* decl;
  ASTNode* ast_node;
  ASTUserType* ast_user_t;
  BuiltinType builtin;
}

%token VOID WCHAR BOOL CHAR SHORT INT
%token UCHAR SCHAR USHORT UINT ULONG
%token INT64 LONG LONGLONG INT128 FLOAT DOUBLE
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
%type<ast_node> functor_decl functor_helper functor_params functor_ptr
%type<ast_user_t> user_def_type
%type<builtin> type_builtin
%type<ival> type_qualifier type_sign

%%

/* Don't throw an error when reading files with multiple
   function signatures, but only return one */
function_decls
 : function_decl
 | function_decls function_decl
 ;

/* Accept static even though functions with internal
   linkage won't generate symbols */
function_decl
 : return_decl             {func_decl = $1;}
 | return_decl ';'         {func_decl = $1;}
 | STATIC return_decl ';'  {func_decl = $2;}
 | STATIC return_decl      {func_decl = $2;}
 ;

return_decl
 : type_declaration named_decl {$$ = $2; $$->return_val = $1;}
 | named_decl                  {$$ = $1; $$->return_val = new ASTBuiltin();}
 ;

named_decl
 : STRING_LITERAL typed_decl {$$ = $2; $$->name = $1;}
 ;

typed_decl
 : '(' parameter_type_list ')' {$$ = $2;}
 | '(' ')'                     {$$ = new FuncDecl();}
 ;

parameter_type_list
 : parameter_list ',' ELLIPSIS {
                                 $$ = $1;
                                 auto param = new ASTBuiltin();
                                 param->type_e = BuiltinType::ELLIPSIS;
                                 $$->params.push_back(param);
                               }
 | parameter_list              {$$ = $1;}
 ;

parameter_list
 : parameter_declaration                    {$$ = new FuncDecl(); $$->params.push_back($1);}
 | parameter_list ',' parameter_declaration {$$ = $1; $$->params.push_back($3);}
 ;

parameter_declaration
 : type_declaration                           {$$ = $1;}
 | type_declaration functor_decl              {
                                                auto fctr = static_cast<ASTFunctor*>($2);
                                                fctr->return_type = $1;
                                                $$ = fctr;
                                              }
 | type_declaration array_type                {
                                                auto p = $2;
                                                while(p->pointee)
                                                  p = p->pointee;
                                                p->pointee = $1;
                                                $$ = $2;
                                              }
 | type_declaration STRING_LITERAL            {$$ = $1;}
 | type_declaration STRING_LITERAL array_type {
                                                auto p = $3;
                                                while(p->pointee)
                                                  p = p->pointee;
                                                p->pointee = $1;
                                                $$ = $3;
                                              }
 ;

functor_decl
  : '(' functor_helper ')' '(' ')' {
                                    auto fctr = new ASTFunctor();
                                    auto voidBuiltin = new ASTBuiltin();
                                    voidBuiltin->type_e = BuiltinType::VOID;
                                    fctr->args.push_back(voidBuiltin);
                                    fctr->pointee = $2;
                                    $$ = fctr;
                                   }
 | '(' functor_helper ')' '(' functor_params ')' {
                                                   auto fctr = static_cast<ASTFunctor*>($5);
                                                   fctr->pointee = $2;
                                                   $$ = fctr;
                                                 }
 ;

functor_helper
 : functor_ptr                           {$$ = $1;}
 | functor_ptr STRING_LITERAL            {$$ = $1;}
 | functor_ptr STRING_LITERAL array_type {
                                           auto p = $3; // ASTReference
                                           while(p->pointee)
                                             p = p->pointee;
                                           p->pointee = $1;
                                           $$ = $3;
                                         }
 ;

/* Ideally would merge this with qualified_abstract_decl, but grammar
   shouldn't accept references. */
functor_ptr
 : '*'                            {$$ = new ASTReference(ASTReference::PTR);}
 | '*' type_qualifier             {
                                    $$ = new ASTReference(ASTReference::PTR);
                                    $$->quals = $$->quals | $2;
                                  }
 | functor_ptr '*'                {
                                    auto r = new ASTReference(ASTReference::PTR);
                                    r->pointee = $1; $$ = r;
                                  }
 | functor_ptr '*' type_qualifier {
                                    auto r = new ASTReference(ASTReference::PTR);
                                    r->quals = r->quals | $3;
                                    r->pointee = $1; $$ = r;
                                  }
 ;

functor_params
 : type_declaration                    {
                                         auto fctr = new ASTFunctor();
                                         fctr->args.push_back($1);
                                         $$ = fctr;
                                       }
 | functor_params ',' type_declaration {
                                         auto fctr = static_cast<ASTFunctor*>($1);
                                         fctr->args.push_back($3);
                                         $$ = fctr;
                                       }
 ;

array_type
 : '[' ']'                  {$$ = new ASTArray(0);}
 | '[' ']' sized_array_type {
                              auto a = new ASTArray(0);
                              a->pointee = $3;
                              $$ = a;
                            }
 |  sized_array_type        {$$ = $1;}
 ;

sized_array_type
 : '[' NUM_LITERAL ']'     {
                             auto size = std::stoul($2,nullptr);
                             $$ = new ASTArray(size);
                           }
 |  sized_array_type '[' NUM_LITERAL ']' {
                                           auto size = std::stoul($3,nullptr);
                                           auto r = static_cast<ASTNode*>($1);
                                           while (r->pointee)
                                              r = r->pointee;
                                           r->pointee = new ASTArray(size);
                                           $$ = $1;
                                         }
 ;

type_declaration
 : qualified_user_t                         {$$ = $1;}
 | qualified_builtin                        {$$ = $1;}
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
 : abstract_declarator type_qualifier {$$ = $1; $$->quals = $$->quals | $2;}
 | abstract_declarator                {$$ = $1;}
 ;

abstract_declarator
 : '*'                         {$$ = new ASTReference(ASTReference::PTR);}
 | '&'                         {$$ = new ASTReference(ASTReference::REF);}
 | qualified_abstract_decl '*' {auto r = new ASTReference(ASTReference::PTR);r->pointee = $1; $$ = r;}
 | qualified_abstract_decl '&' {auto r = new ASTReference(ASTReference::REF);r->pointee = $1; $$ = r;}
 ;

qualified_builtin
 : type_qualifier signed_builtin  {$$ = $2; $$->quals = $$->quals | $1;}
 | signed_builtin type_qualifier  {$$ = $1; $$->quals = $$->quals | $2;}
 | signed_builtin                 {$$ = $1;}
 ;

signed_builtin
 : type_sign type_builtin  {auto b = new ASTBuiltin(); b->type_e = $2; b->mods = b->mods | $1; $$ = b;}
 | type_builtin type_sign  {auto b = new ASTBuiltin(); b->type_e = $1; b->mods = b->mods | $2; $$ = b;}
 | type_builtin            {auto b = new ASTBuiltin(); b->type_e = $1; $$ = b;}
 ;

type_sign
 : UNSIGNED {$$ = ASTBuiltin::UNSIGNED;}
 | SIGNED   {$$ = ASTBuiltin::SIGNED;}
 ;

qualified_user_t
 : type_qualifier user_def_type  {$$ = $2; $$->quals = $$->quals | $1;}
 | user_def_type type_qualifier  {$$ = $1; $$->quals = $$->quals | $2;}
 | user_def_type                 {$$ = $1;}
 ;

user_def_type
 : STRUCT STRING_LITERAL {$$ = new ASTUserType($2); $$->complexType = ASTUserType::Complex_e::STRUCT;}
 | UNION STRING_LITERAL  {$$ = new ASTUserType($2); $$->complexType = ASTUserType::Complex_e::UNION;}
 | ENUM STRING_LITERAL   {$$ = new ASTUserType($2); $$->complexType = ASTUserType::Complex_e::ENUM;}
 | STRING_LITERAL        {$$ = new ASTUserType($1);}
 ;

type_qualifier
 : CONST    {$$ = ASTNode::CONST;}
 | VOLATILE {$$ = ASTNode::VOLATILE;}
 ;

type_builtin
 : VOID       {$$ = BuiltinType::VOID;}
 | WCHAR      {$$ = BuiltinType::WCHAR;}
 | BOOL       {$$ = BuiltinType::BOOL;}
 | CHAR       {$$ = BuiltinType::CHAR;}
 | UCHAR      {$$ = BuiltinType::UCHAR;}
 | SCHAR      {$$ = BuiltinType::SCHAR;}
 | SHORT      {$$ = BuiltinType::SHORT;}
 | USHORT     {$$ = BuiltinType::USHORT;}
 | INT        {$$ = BuiltinType::INT;}
 | UINT       {$$ = BuiltinType::UINT;}
 | LONG       {$$ = BuiltinType::LONG;}
 | INT64      {$$ = BuiltinType::INT64;}
 | ULONG      {$$ = BuiltinType::ULONG;}
 | LONGLONG   {$$ = BuiltinType::LONGLONG;}
 | INT128     {$$ = BuiltinType::INT128;}
 | FLOAT      {$$ = BuiltinType::FLOAT;}
 | DOUBLE     {$$ = BuiltinType::DOUBLE;}
 | CHAR32     {$$ = BuiltinType::CHAR32;}
 | CHAR16     {$$ = BuiltinType::CHAR16;}
 | AUTO       {$$ = BuiltinType::AUTO;}
 | NULLPTR    {$$ = BuiltinType::NULLPTR;}
 ;

%%
FuncDecl* ParseStdin() {
#if YYDEBUG == 1
  yydebug = YYDEBUG;
#endif

  yyparse();
  return func_decl;
}

void yyerror(const char* s) {
  printf("Exiting, parse error '%s'\n", s);
  exit(1);
}
