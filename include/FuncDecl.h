#ifndef FUNC_DECL_H
#define FUNC_DECL_H

#include <cstdint>
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

struct FuncParam {
  enum Qualifiers {
    CONST = 0x1 << 0,
    VOLATILE = 0x1 << 1,
  };

  enum Modifiers {
    PTR = 0x1 << 0,
    UNSIGNED = 0x1 << 1,
    SIGNED = 0x1 << 2,
    REFERENCE = 0x1 << 3,
    RVALREF = 0x1 << 4,
  };

  uint8_t quals;
  uint8_t mods;
  BuiltinType type_e;

  FuncParam(): quals(0), mods(0), type_e(BuiltinType::VOID)
  {}

};

struct FuncDecl{
    const char* name;
    std::vector<const FuncParam *> params;
};

FuncDecl* ParseStdin();
#endif // FUNC_DECL_H
