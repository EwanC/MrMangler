#include <iostream>

#include "FuncDecl.h"

int main()
{
    FuncDecl* func_decl = ParseStdin();
    std::cout << func_decl->name << std::endl;

    return 0;
}
