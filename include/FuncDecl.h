#ifndef FUNC_DECL_H
#define FUNC_DECL_H

#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

enum class BuiltinType
{
  VOID,
  WCHAR,
  BOOL,
  CHAR,
  UCHAR,
  SHORT,
  USHORT,
  INT,
  UINT,
  LONG,
  ULONG,
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

struct ASTNode
{
  enum Qualifiers
  {
    CONST = 0x1 << 0,
    VOLATILE = 0x1 << 1,
  };

  uint8_t quals;
  ASTNode() : quals(0)
  {
  }

  virtual uint8_t getID() = 0;
};

struct ASTReference : ASTNode
{
  enum Indirection
  {
    PTR = 0x0,
    REF = 0x1,
    RVALREF = 0x2
  };
  ASTNode* pointee; // TODO smert pointer
  const Indirection ref_type;

  ASTReference(Indirection t_) : pointee(nullptr), ref_type(t_)
  {
  }

  virtual uint8_t getID() override
  {
    return 1;
  }
};

struct ASTUserType : ASTNode
{
  const std::string name;
  ASTUserType(const char* name_) : name(name_)
  {
  }

  virtual uint8_t getID() override
  {
    return 2;
  }
};

struct ASTBuiltin : ASTNode
{

  enum Modifiers
  {
    UNSIGNED = 0x1 << 1,
    SIGNED = 0x1 << 2,
  };

  uint8_t mods;
  BuiltinType type_e;

  ASTBuiltin() : mods(0), type_e(BuiltinType::VOID)
  {
  }
  virtual uint8_t getID() override
  {
    return 3;
  }
};

struct FuncDecl
{
  const char* name;
  std::vector<const ASTNode*> params;
  const ASTNode* return_val;
};

FuncDecl* ParseStdin(FILE* f);
#endif // FUNC_DECL_H
