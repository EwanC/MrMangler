#ifndef FUNC_DECL_H
#define FUNC_DECL_H

#include <cstdio>
#include <cstdint>
#include <string>
#include <vector>

enum class BuiltinType {
   VOID,
   WCHAR,
   BOOL,
   CHAR,
   SHORT,
   INT,
   LONG,
   LONGLONG,
   INT128,
   FLOAT,
   DOUBLE,
   FLOAT80,
   FLOAT128,
   ELLIPSIS,
   IEEE754_16,
   IEEE754_32,
   IEEE754_64,
   IEEE754_128,
   CHAR32,
   CHAR16,
   AUTO,
   NULLPTR
};

struct ASTNode{
 enum Qualifiers {
    CONST = 0x1 << 0,
    VOLATILE = 0x1 << 1,
  };

  uint8_t quals;
  ASTNode(): quals(0)
  {}
};

struct ASTReference : ASTNode
{
  enum Indirection {PTR, REF, RVALREF};
  ASTNode* pointee; // TODO smert pointer
  const Indirection ref_type;

  ASTReference(Indirection t_) : pointee(nullptr), ref_type(t_)
  {}

};

struct ASTUserType : ASTNode
{
  const std::string name;
  ASTUserType(const char* name_) : name(name_)
  {}
};


struct ASTBuiltin : ASTNode {
 
  enum Modifiers {
    UNSIGNED = 0x1 << 1,
    SIGNED = 0x1 << 2,
  };

  uint8_t mods;
  BuiltinType type_e;

  ASTBuiltin(): mods(0), type_e(BuiltinType::VOID)
  {}

};

struct FuncDecl{
    const char* name;
    std::vector<const ASTNode *> params;
    const ASTNode* return_val;
};

FuncDecl* ParseStdin(FILE* f);
#endif // FUNC_DECL_H
