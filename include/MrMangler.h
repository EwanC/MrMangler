#ifndef MR_MANGLER_H
#define MR_MANGLER_H

#include <string>

class FuncDecl;

typedef std::string (*mangle_fn)(const FuncDecl*);

std::string mangle_itanium(const FuncDecl* decl);
std::string mangle_windows(const FuncDecl* decl);
#endif // MR_MANGLER_H
