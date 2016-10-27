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

  if (BuiltinType::UCHAR == t)
    return "E";

  if (BuiltinType::SHORT == t)
  {
    if (ASTBuiltin::UNSIGNED & mods)
      return "G";
    return "F";
  }

  if (BuiltinType::USHORT == t)
    return "G";

  if (BuiltinType::INT == t)
  {
    if (ASTBuiltin::UNSIGNED & mods)
      return "I";
    return "H";
  }

  if (BuiltinType::UINT == t)
    return "I";

  if (BuiltinType::LONG == t)
  {
    if (ASTBuiltin::UNSIGNED & mods)
      return "K";
    return "J";
  }

  if (BuiltinType::ULONG == t)
    return "K";

  if (BuiltinType::LONGLONG == t)
  {
    if (ASTBuiltin::UNSIGNED & mods)
      return "_K";
    return "_J";
  }

  if (BuiltinType::INT128 == t)
  {
    if (ASTBuiltin::UNSIGNED & mods)
      return "_M";
    return "_L";
  }

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
  return "";
}

static std::string mangle_qualifier(const uint8_t qual_bitfield)
{
  std::string mangled;
  if ((qual_bitfield & ASTNode::CONST) && (qual_bitfield & ASTNode::VOLATILE))
    mangled.push_back('D');
  else if (qual_bitfield & ASTNode::CONST)
    mangled.push_back('B');
  else if (qual_bitfield & ASTNode::VOLATILE)
    mangled.push_back('C');

  return mangled;
}

static std::string mangle_param(const ASTNode* p)
{
  std::string mangled;
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
    {
      mangled.push_back('P');
      mangled.push_back('A'); // assume cdecl convention
    }

    if (r->pointee)
      mangled.append(mangle_param(r->pointee)); // recursive call
  }
  else if (typeid(*p) == typeid(ASTArray))
  {
    const ASTArray* r = static_cast<const ASTArray*>(p);

    mangled.push_back('Q');
    auto qual = mangle_qualifier(r->pointee->quals);
    if (qual.empty())
      mangled.push_back('A');
    else
      mangled.append(qual);

    if (r->pointee)
      mangled.append(mangle_param(r->pointee)); // recursive call
  }

  return mangled;
}

std::string mangle_windows(const std::shared_ptr<FuncDecl> decl, const CCOption_e calling_conv)
{
  std::ostringstream mangled;
  mangled << "?" << decl->name << "@";

  // Don't support class names
  mangled << "@Y";

  if (CCOption_e::fastcall == calling_conv)
    mangled << "I"; // __fastcall
  else if (CCOption_e::stdcall == calling_conv)
    mangled << "G"; // __stdcall
  else
    mangled << "A"; // __cdecl

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
