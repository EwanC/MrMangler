#include <cassert>
#include <cstring>
#include <sstream>
#include <typeinfo>

#include "FuncDecl.h"
#include "MrMangler.h"

static std::string mangle_type(const BuiltinType t, const uint8_t mods)
{
  if (BuiltinType::VOID == t)
    return "v";

  if (BuiltinType::BOOL == t)
    return "b";

  if (BuiltinType::CHAR == t)
  {
    if (ASTBuiltin::UNSIGNED & mods)
      return "h";
    else if (ASTBuiltin::SIGNED & mods)
      return "a";
    else
      return "c";
  }

  if (BuiltinType::UCHAR == t)
    return "h";

  if (BuiltinType::SCHAR == t)
    return "h";

  if (BuiltinType::SHORT == t)
  {
    if (ASTBuiltin::UNSIGNED & mods)
      return "t";
    return "s";
  }

  if (BuiltinType::USHORT == t)
    return "t";

  if (BuiltinType::INT == t)
  {
    if (ASTBuiltin::UNSIGNED & mods)
      return "j";
    return "i";
  }

  if (BuiltinType::UINT == t)
    return "j";

  if (BuiltinType::LONG == t)
  {
    if (ASTBuiltin::UNSIGNED & mods)
      return "m";
    return "l";
  }

  if (BuiltinType::INT64 == t)
    return "l";

  if (BuiltinType::ULONG == t)
    return "m";

  if (BuiltinType::LONGLONG == t)
  {
    if (ASTBuiltin::UNSIGNED & mods)
      return "y";
    return "x";
  }

  if (BuiltinType::INT128 == t)
  {
    if (ASTBuiltin::UNSIGNED & mods)
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

  assert(false && "Unknown type");
  return "";
}

static std::string mangle_qualifier(const uint8_t qual_bitfield)
{
  std::string mangled;
  if (qual_bitfield & ASTNode::CONST)
    mangled.push_back('K');
  if (qual_bitfield & ASTNode::VOLATILE)
    mangled.push_back('V');

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
    else if (r->ref_type == ASTReference::REF)
      mangled.push_back('R');
    else if (r->ref_type == ASTReference::RVALREF)
      mangled.push_back('O');
    if (r->pointee)
      mangled.append(mangle_param(r->pointee)); // recursive call
  }
  else if (typeid(*p) == typeid(ASTArray))
  {
    const ASTArray* r = static_cast<const ASTArray*>(p);
    auto size = r->size;
    if (0 == size)
    {
      mangled.push_back('P');
    }
    else
    {
      mangled.push_back('A');
      mangled.append(std::to_string(size));
      mangled.push_back('_');
    }

    if (r->pointee)
      mangled.append(mangle_param(r->pointee)); // recursive call
  }
  else if (typeid(*p) == typeid(ASTFunctor))
  {
    const ASTFunctor* f = static_cast<const ASTFunctor*>(p);
    const ASTNode* indirection = f->pointee;

    mangled.append(mangle_qualifier(indirection->quals));
    mangled.push_back('P'); // one for each level of indirection
    while (indirection->pointee)
    {
      indirection = indirection->pointee;
      mangled.append(mangle_qualifier(indirection->quals));
      mangled.push_back('P');
    }

    mangled.push_back('F');

    // ref should now be the return type
    assert(f->return_type && "no functor return type");
    mangled.append(mangle_param(f->return_type));

    // functor params
    for (auto arg : f->args)
      mangled.append(mangle_param(arg));

    mangled.push_back('E');
  }
  else
  {
    assert(false && "Unknown parameter type");
  }

  return mangled;
}

std::string mangle_itanium(const std::shared_ptr<FuncDecl> decl, const CCOption_e)
{
  std::ostringstream mangled;
  mangled << "_Z" << strlen(decl->name) << decl->name;

  const std::vector<const ASTNode*>& params = decl->params;
  if (params.empty())
  {
    mangled << "v";
    return mangled.str();
  }

  for (auto p : params)
    mangled << mangle_param(p);

  return mangled.str();
}
