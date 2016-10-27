#ifndef MR_MANGLER_H
#define MR_MANGLER_H

#include <memory>
#include <string>

struct FuncDecl;

enum class CCOption_e
{
  cdecl,
  fastcall,
  stdcall
};

typedef std::string (*mangle_fn)(const std::shared_ptr<FuncDecl>, const CCOption_e);

std::string mangle_itanium(const std::shared_ptr<FuncDecl> decl, const CCOption_e);
std::string mangle_windows(const std::shared_ptr<FuncDecl> decl, const CCOption_e);
#endif // MR_MANGLER_H
