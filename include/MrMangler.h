#ifndef MR_MANGLER_H
#define MR_MANGLER_H

#include <memory>
#include <string>

struct FuncDecl;

typedef std::string (*mangle_fn)(const std::shared_ptr<FuncDecl>);

std::string mangle_itanium(const std::shared_ptr<FuncDecl> decl);
std::string mangle_windows(const std::shared_ptr<FuncDecl> decl);
#endif // MR_MANGLER_H
