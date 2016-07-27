#include <cstring>
#include <sstream>

#include "FuncDecl.h"
#include "MrMangler.h"

std::string mangle_itanium(FuncDecl* decl)
{
    std::ostringstream mangled;
    mangled << "_Z" << strlen(decl->name) << decl->name;

    const std::vector<FuncParam *> &params = decl->params;
    if (params.empty())
    {
       mangled << "v";
       return mangled.str();
    }

    return mangled.str();
}
