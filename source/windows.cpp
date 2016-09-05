#include <cassert>
#include <sstream>
#include <typeinfo>

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

static std::string mangle_param(const ASTNode* p)
{
  std::string mangled = mangle_qualifier(p->quals);
  if (typeid(*p) == typeid(ASTBuiltin))
  {
    const ASTBuiltin* b = static_cast<const ASTBuiltin*>(p);
    mangled.append(mangle_type(b->type_e, b->mods));
  }
  else if (typeid(*p) == typeid(ASTUserType))
  {
    const ASTUserType* u = static_cast<const ASTUserType*>(p);
    const std::string& name = u->name;
    mangled.append(std::to_string(name.length()).append(name));
  }
  else if (typeid(*p) == typeid(ASTReference))
  {
    const ASTReference* r = static_cast<const ASTReference*>(p);
    if (r->ref_type == ASTReference::PTR)
      mangled.push_back('P');

    if (r->pointee)
      mangled.append(mangle_param(r->pointee)); // recursive call
  }
  return mangled;

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
