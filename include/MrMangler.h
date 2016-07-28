#ifndef MR_MANGLER_H
#define MR_MANGLER_H

#include <string>

class FuncDecl;

std::string mangle_itanium(const FuncDecl* decl);
#endif // MR_MANGLER_H
