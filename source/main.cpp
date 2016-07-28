#include <iostream>

#include "MrMangler.h"
#include "FuncDecl.h"

int main()
{
    const FuncDecl* func_decl = ParseStdin();
    const std::string mangled_str = mangle_itanium(func_decl);
    std::cout << mangled_str << std::endl;

    return 0;
}
