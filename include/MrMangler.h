#ifndef MR_MANGLER_H
#define MR_MANGLER_H

#include <memory>
#include <string>

struct FuncDecl;

// Calling conventions supported
enum class CCOption_e
{
  Cdecl,
  fastcall,
  stdcall
};

// Function pointer to one of the below mangling systems
typedef std::string (*mangle_fn)(const std::shared_ptr<FuncDecl>, const CCOption_e);

std::string mangle_itanium(const std::shared_ptr<FuncDecl> decl, const CCOption_e);
std::string mangle_windows(const std::shared_ptr<FuncDecl> decl, const CCOption_e);
#endif // MR_MANGLER_H
