#include <vector>

enum class literal {
   CHAR,
   DOUBLE,
   FLOAT,
   INT,
   SHORT,
   BOOL,
   LONG,
   VOID
};

struct FuncParam {
  bool isConst;
  bool isVolatile;
  bool isPtr;
  literal type_e;

  FuncParam(): isConst(false), isVolatile(false), isPtr(false), type_e(literal::VOID)
  {}

  const char* print() {
     if (literal::CHAR == type_e)
        return "char";

     if (literal::DOUBLE == type_e)
        return "double";

     if (literal::FLOAT == type_e)
        return "float";

     if (literal::INT == type_e)
        return "int";

     if (literal::SHORT == type_e)
        return "short";

     if (literal::BOOL == type_e)
        return "bool";

     if (literal::LONG == type_e)
        return "long";

     if (literal::VOID == type_e)
        return "void";

     return "puff";
  }
};

struct FuncDecl{
    const char* name;
    std::vector<FuncParam *> params;
};
