#include "FuncDecl.h"
#include "MrMangler.h"

std::string mangle_windows(const FuncDecl *p)
{
    return p->name;
}
