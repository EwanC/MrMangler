#include <cassert>
#include <cstring>
#include <sstream>

#include "FuncDecl.h"
#include "MrMangler.h"

static std::string mangle_type(const BuiltinType t, const uint8_t mods, const std::string& user_defn)
{
  if (BuiltinType::VOID == t)
    return "v";

  if (BuiltinType::BOOL == t)
    return "b";

  if (BuiltinType::CHAR == t)
  {
    if (FuncParam::UNSIGNED & mods)
      return "h";
    else if (FuncParam::SIGNED & mods)
      return "a";
    else
      return "c";
  }

  if (BuiltinType::SHORT == t)
  {
    if (FuncParam::UNSIGNED & mods)
      return "t";
    return "s";
  }

  if (BuiltinType::INT == t)
  {
    if (FuncParam::UNSIGNED & mods)
      return "j";
    return "i";
  }

  if (BuiltinType::LONG == t)
  {
    if (FuncParam::UNSIGNED & mods)
      return "m";
    return "l";
  }

  if (BuiltinType::LONGLONG == t)
  {
    if (FuncParam::UNSIGNED & mods)
      return "y";
    return "x";
  }

  if (BuiltinType::INT128 == t)
  {
    if (FuncParam::UNSIGNED & mods)
      return "o";
    return "n";
  }

  if (BuiltinType::FLOAT == t)
    return "f";

  if (BuiltinType::DOUBLE == t)
    return "d";

  if (BuiltinType::FLOAT80 == t)
    return "e";

  if (BuiltinType::FLOAT128 == t)
    return "g";

  if (BuiltinType::ELLIPSIS == t)
    return "z";

  if (BuiltinType::IEEE754_16 == t)
    return "Dh";

  if (BuiltinType::IEEE754_32 == t)
    return "Df";

  if (BuiltinType::IEEE754_64 == t)
    return "Dd";

  if (BuiltinType::IEEE754_128 == t)
    return "De";

  if (BuiltinType::CHAR32 == t)
    return "Di";

  if (BuiltinType::CHAR16 == t)
    return "Ds";

  if (BuiltinType::AUTO == t)
    return "Da";

  if (BuiltinType::NULLPTR == t)
    return "Dn";

  if (BuiltinType::USER_DEF == t)
    return std::to_string(user_defn.length()).append(user_defn);

  assert(false && "Unknown type");
}

static std::string mangle_qualifier(const uint8_t qual_bitfield)
{
  std::string mangled;
  if (qual_bitfield & FuncParam::CONST)
    mangled.push_back('K');
  if (qual_bitfield & FuncParam::VOLATILE)
    mangled.push_back('V');

  return mangled;
}

static std::string mangle_modifier(const uint8_t mod_bitfield)
{
  std::string mangled;
  if (mod_bitfield & FuncParam::PTR)
    mangled.push_back('P');
  if (mod_bitfield & FuncParam::REFERENCE)
    mangled.push_back('R');
  if (mod_bitfield & FuncParam::RVALREF)
    mangled.push_back('O');

  return mangled;
}

static std::string mangle_param(const FuncParam* p)
{
  std::string mangled = mangle_modifier(p->mods);
  mangled.append(mangle_qualifier(p->quals));
  mangled.append(mangle_type(p->type_e, p->mods, p->user_def_name));
  return mangled;
}

std::string mangle_itanium(const FuncDecl* decl)
{
  std::ostringstream mangled;
  mangled << "_Z" << strlen(decl->name) << decl->name;

  const std::vector<const FuncParam*>& params = decl->params;
  if (params.empty())
  {
    mangled << "v";
    return mangled.str();
  }

  for (auto p : params)
    mangled << mangle_param(p);

  return mangled.str();
}
