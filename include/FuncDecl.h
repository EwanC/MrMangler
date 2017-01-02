#ifndef FUNC_DECL_H
#define FUNC_DECL_H

#include <cstdint>
#include <string>
#include <vector>

enum class BuiltinType
{
  VOID,
  WCHAR,
  BOOL,
  CHAR,
  UCHAR,
  SCHAR,
  SHORT,
  USHORT,
  INT,
  UINT,
  LONG,
  ULONG,
  LONGLONG,
  INT64,
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
  ASTNode* pointee;
  ASTNode() : quals(0), pointee(nullptr)
  {
  }

  virtual uint8_t getID() = 0;

  virtual ASTNode* getPointee()
  {
    return pointee;
  }

  virtual ~ASTNode()
  {
    delete pointee;
  }
};


struct ASTReference final : ASTNode
{
  enum Indirection
  {
    PTR = 0x0,
    REF = 0x1,
    RVALREF = 0x2
  };
  const Indirection ref_type;

  ASTReference(Indirection t_) : ref_type(t_)
  {
  }

  virtual uint8_t getID() override
  {
    return 1;
  }
};

struct ASTUserType final : ASTNode
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

struct ASTBuiltin final : ASTNode
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

struct ASTArray final : ASTNode
{
  uint32_t size;

  ASTArray(uint32_t size_) : size(size_)
  {
  }

  virtual uint8_t getID() override
  {
    return 4;
  }
};

struct ASTFunctor final: ASTNode
{
  std::vector<const ASTNode*> args;
  const ASTNode* return_type;

  ASTFunctor(): return_type(nullptr)
  {
  }

  virtual uint8_t getID() override
  {
    return 5;
  }

  virtual ~ASTFunctor() override
  {
    delete return_type;
    for (auto a : args)
      delete a;
  }
};

struct FuncDecl final
{
  const char* name;
  std::vector<const ASTNode*> params;
  const ASTNode* return_val;

  ~FuncDecl()
  {
    delete return_val;
    for (auto p : params)
      delete p;
  }
};

FuncDecl* ParseStdin();
#endif // FUNC_DECL_H
