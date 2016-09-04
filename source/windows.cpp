#include <cassert>
#include <sstream>

#include "FuncDecl.h"
#include "MrMangler.h"

static std::string mangle_type(const BuiltinType t, const uint8_t mods)
{
  if (BuiltinType::VOID == t)
    return "X";

  if (BuiltinType::BOOL == t)
    return "_N";

  if (BuiltinType::CHAR == t)
  {
    if (ASTBuiltin::UNSIGNED & mods)
      return "E";
    else if (ASTBuiltin::SIGNED & mods)
      return "C";
    else
      return "D";
  }

  if (BuiltinType::SHORT == t)
  {
    if (ASTBuiltin::UNSIGNED & mods)
      return "G";
    return "F";
  }

  if (BuiltinType::INT == t)
  {
    if (ASTBuiltin::UNSIGNED & mods)
      return "I";
    return "H";
  }

  if (BuiltinType::LONG == t)
  {
    if (ASTBuiltin::UNSIGNED & mods)
      return "K";
    return "J";
  }

  //  if (BuiltinType::LONGLONG == t){
  //    if (FuncParam::UNSIGNED & mods)
  //        return "y";
  //    return "x";
  //  }

  //  if (BuiltinType::INT128 == t){
  //    if (FuncParam::UNSIGNED & mods)
  //        return "o";
  //    return "n";
  //  }

  if (BuiltinType::FLOAT == t)
    return "M";

  if (BuiltinType::DOUBLE == t)
    return "N";

  //  if (BuiltinType::FLOAT80 == t)
  //    return "e";

  //  if (BuiltinType::FLOAT128 == t)
  //    return "g";

  //  if (BuiltinType::ELLIPSIS == t)
  //    return "Z";

  //  if (BuiltinType::IEEE754_16 == t)
  //    return "Dh";

  //  if (BuiltinType::IEEE754_32 == t)
  //    return "Df";

  //  if (BuiltinType::IEEE754_64 == t)
  //    return "Dd";

  //  if (BuiltinType::IEEE754_128 == t)
  //    return "De";

  //  if (BuiltinType::CHAR32 == t)
  //    return "Di";

  //  if (BuiltinType::CHAR16 == t)
  //    return "Ds";

  //  if (BuiltinType::AUTO == t)
  //    return "Da";

  //  if (BuiltinType::NULLPTR == t)
  //    return "Dn";

  assert(false && "Unknown type");
}

static std::string mangle_qualifier(const uint8_t qual_bitfield)
{
  std::string mangled;
  // if (qual_bitfield & FuncParam::CONST)
  //     mangled.push_back('K');
  // if (qual_bitfield & FuncParam::VOLATILE)
  //     mangled.push_back('V');

  return mangled;
}

static std::string mangle_modifier(const uint8_t mod_bitfield)
{
  std::string mangled;
  if (mod_bitfield & ASTReference::PTR)
    mangled.push_back('P');
  // if (mod_bitfield & FuncParam::REFERENCE)
  //     mangled.push_back('R');
  // if (mod_bitfield & FuncParam::RVALREF)
  //     mangled.push_back('O');

  return mangled;
}

static std::string mangle_param(const ASTNode* p)
{
  // std::string mangled = mangle_modifier(p->mods);
  // mangled.append(mangle_qualifier(p->quals));
  // mangled.append(mangle_type(p->type_e, p->mods));
  // return mangled;
  return "bad";
}

std::string mangle_windows(const FuncDecl* decl)
{
  std::ostringstream mangled;
  mangled << "?" << decl->name << "@";

  // Don't support class names
  mangled << "@Y";

  // TODO check calling convention
  // __cdecl -> "A"
  // __fastcall -> "I"
  // __stdcall -> "G"
  mangled << "A"; // Assume cdecl for now

  // scramble return value
  mangled << mangle_param(decl->return_val);

  const std::vector<const ASTNode*>& params = decl->params;
  if (params.empty())
  {
    mangled << "X";
    return mangled.str();
  }

  for (auto p : params)
    mangled << mangle_param(p);

  mangled << "@Z";

  return mangled.str();
}
