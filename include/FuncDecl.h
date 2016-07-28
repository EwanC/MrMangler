#include <vector>

enum class BuiltinType {
   VOID,
   WCHAR,
   BOOL,
   CHAR,
   SCHAR,
   UCHAR,
   SHORT,
   USHORT,
   INT,
   UINT,
   LONG,
   ULONG,
   LONGLONG,
   ULONGLONG,
   INT128,
   UINT128,
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
  bool isConst;
  bool isVolatile;
  bool isPtr;
  bool isRef;
  bool isRvalRef;
  BuiltinType type_e;

  FuncParam(): isConst(false), isVolatile(false), isPtr(false), isRef(false), isRvalRef(false), type_e(BuiltinType::VOID)
  {}

};

struct FuncDecl{
    const char* name;
    std::vector<const FuncParam *> params;
};

FuncDecl* ParseStdin();
