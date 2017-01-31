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

// Base class all the AST nodes derive from
struct ASTNode
{
  enum Qualifiers
  {
    CONST = 0x1 << 0,
    VOLATILE = 0x1 << 1,
  };

  uint8_t quals;
  ASTNode* pointee; // Another AST node referenced

  ASTNode() : quals(0), pointee(nullptr)
  {
  }

  virtual uint8_t getID() const = 0;

  virtual ASTNode* getPointee() const
  {
    return pointee;
  }

  virtual ~ASTNode()
  {
    delete pointee;
  }
};

// AST Node for an indirection to another node, such as
// a pointer or reference
struct ASTReference final : ASTNode
{
  enum Indirection
  {
    PTR = 0x0,    // pointer
    REF = 0x1,    // reference
    RVALREF = 0x2 // r-value reference
  };

  // Defaults to pointer
  ASTReference(Indirection t_) : ref_type(t_)
  {
  }

  virtual uint8_t getID() const override
  {
    return 1;
  }

  const Indirection ref_type;
};

// AST node for a user defined type
struct ASTUserType final : ASTNode
{
  enum Complex_e
  {
    DEFAULT = 0,
    CLASS = 0x1 << 1,  // C++ class
    STRUCT = 0x1 << 2, // struct
    UNION = 0x1 << 3,  // union
    ENUM = 0x1 << 4    // enum
  };

  ASTUserType(const char* name_) : complexType(DEFAULT), name(name_)
  {
  }

  virtual uint8_t getID() const override
  {
    return 2;
  }

  const std::string name;
  Complex_e complexType;
};

// AST Node for a builtin type
struct ASTBuiltin final : ASTNode
{
  enum Modifiers
  {
    UNSIGNED = 0x1 << 1,
    SIGNED = 0x1 << 2,
  };

  uint8_t mods;
  BuiltinType type_e;

  // Default to void type
  ASTBuiltin() : mods(0), type_e(BuiltinType::VOID)
  {
  }

  virtual uint8_t getID() const override
  {
    return 3;
  }
};

// AST node for an array
struct ASTArray final : ASTNode
{
  uint32_t size;

  ASTArray(uint32_t size_) : size(size_)
  {
  }

  virtual uint8_t getID() const override
  {
    return 4;
  }
};

// AST node for a function pointer
struct ASTFunctor final : ASTNode
{
  std::vector<const ASTNode*> args;
  const ASTNode* return_type;

  ASTFunctor() : return_type(nullptr)
  {
  }

  virtual uint8_t getID() const override
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

// Top level definition of function signature returned
// by the parser
struct FuncDecl final
{
  const char* name;
  std::vector<const ASTNode*> params;
  const ASTNode* return_val;

  // Delete all the AST nodes
  ~FuncDecl()
  {
    delete return_val;
    for (auto p : params)
      delete p;
  }
};

// Read input in parser
FuncDecl* ParseStdin();
#endif // FUNC_DECL_H
