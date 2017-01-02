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
    return "_K";

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

  assert(false && "Unknown type");
  return "";
}

static char mangle_qualifier(const uint8_t qual_bitfield)
{
  if ((qual_bitfield & ASTNode::CONST) && (qual_bitfield & ASTNode::VOLATILE))
    return 'D';
  else if (qual_bitfield & ASTNode::CONST)
    return 'B';
  else if (qual_bitfield & ASTNode::VOLATILE)
    return 'C';

  return 0;
}

static std::string mangle_param(const ASTNode* p, bool isReturnType)
{
  std::string mangled;
  if (typeid(*p) == typeid(ASTBuiltin))
  {
    const ASTBuiltin* b = static_cast<const ASTBuiltin*>(p);
    if (isReturnType)
    {
      char qual = mangle_qualifier(b->quals);
      if (0 != qual)
      {
        mangled.push_back('?');
        mangled.push_back(qual);
      }
    }
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
      char qual = mangle_qualifier(r->pointee->quals);
      if (0 != qual)
        mangled.push_back(qual);
      else
        mangled.push_back('A'); // assume cdecl convention
    }
    else if (r->ref_type == ASTReference::REF)
    {
      mangled.append("AA");
    }

    if (r->pointee)
      mangled.append(mangle_param(r->pointee, false)); // recursive call
  }
  else if (typeid(*p) == typeid(ASTArray))
  {
    const ASTArray* r = static_cast<const ASTArray*>(p);

    mangled.push_back('Q');
    auto qual = mangle_qualifier(r->pointee->quals);
    if (0 == qual)
      mangled.push_back('A');
    else
      mangled.push_back(qual);

    if (r->pointee)
      mangled.append(mangle_param(r->pointee, false)); // recursive call
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
  mangled << mangle_param(decl->return_val, true);

  const std::vector<const ASTNode*>& params = decl->params;
  bool voidParam = false;
  if (!params.empty())
  {
    const auto first = params[0];
    if (typeid(*first) == typeid(ASTBuiltin))
    {
      const ASTBuiltin* b = static_cast<const ASTBuiltin*>(first);
      voidParam = BuiltinType::VOID == b->type_e;
    }
  }

  if (params.empty())
  {
    mangled << "X";
  }

  for (auto p : params)
    mangled << mangle_param(p, false);

  if (!params.empty() && !voidParam)
  {
    mangled << "@";
  }

  mangled << "Z";

  return mangled.str();
}
