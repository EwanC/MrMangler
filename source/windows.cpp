#include <algorithm>
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

  if (BuiltinType::SCHAR == t)
    return "C";

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

  if (BuiltinType::INT64 == t)
    return "_J";

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

static std::string mangle_number(unsigned int numberwang)
{
  std::string mangled;
  if (numberwang == 0)
  {
    mangled = "a@";
  }
  else if (numberwang >= 1 && numberwang <= 10)
  {
    mangled = std::to_string(numberwang - 1);
  }
  else
  {
    // from llvm
    // numbers that are not encoded as decimal digits are represented as nibbles
    // in the range of ascii characters 'a' to 'p'.
    for (; numberwang != 0; numberwang >>= 4)
    {
      mangled.push_back('A' + (numberwang & 0xf));
    }
    std::reverse(mangled.begin(), mangled.end());
  }
  return mangled;
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
    if (isReturnType)
    {
      mangled.append("?A");
    }
    if (u->complexType == ASTUserType::Complex_e::CLASS)
    {
      mangled.push_back('V');
    }
    else if (u->complexType == ASTUserType::Complex_e::UNION)
    {
      mangled.push_back('T');
    }
    else if (u->complexType == ASTUserType::Complex_e::ENUM)
    {
      mangled.push_back('W');
    }
    else
    {
      mangled.push_back('U'); // U for struct
    }
    mangled.append(u->name);
    mangled.append("@@");
  }
  else if (typeid(*p) == typeid(ASTReference))
  {
    const ASTReference* r = static_cast<const ASTReference*>(p);
    if (r->ref_type == ASTReference::PTR)
    {
      if (0 != ASTNode::CONST & r->quals)
      {
        mangled.push_back('Q'); // use array sym for const pointers
      }
      else
      {
        mangled.push_back('P');
      }
      char qual = mangle_qualifier(r->pointee->quals);
      if (0 != qual)
        mangled.push_back(qual);
      else
        mangled.push_back('A'); // assume cdecl convention
    }
    else if (r->ref_type == ASTReference::REF)
    {
      mangled.push_back('A');
      char qual = mangle_qualifier(r->pointee->quals);
      if (0 != qual)
        mangled.push_back(qual);
      else
        mangled.push_back('A');
    }

    if (r->pointee)
      mangled.append(mangle_param(r->pointee, false)); // recursive call
  }
  else if (typeid(*p) == typeid(ASTArray))
  {
    const ASTArray* r = static_cast<const ASTArray*>(p);

    // Lookahead to calculate dimensions
    assert(r->pointee && "array needs a type");
    unsigned int dimensions = 1;
    const ASTArray* ptr = static_cast<const ASTArray*>(r->pointee);
    while (typeid(*ptr) == typeid(ASTArray))
    {
      assert(ptr->pointee);
      ptr = static_cast<const ASTArray*>(ptr->pointee);
      ++dimensions;
    }

    for (unsigned int dim = 1; dim <= dimensions; ++dim)
    {
      if (1 == dim)
      {
        mangled.push_back('Q');
        auto qual = mangle_qualifier(r->pointee->quals);
        if (0 == qual)
          mangled.push_back('A');
        else
          mangled.push_back(qual);
      }
      else if (2 == dim)
      {
        r = static_cast<const ASTArray*>(r->pointee);
        mangled.push_back('Y');
        mangled.append(std::to_string(dimensions - 2));

        mangled.append(mangle_number(r->size));
        mangled.push_back('@');
      }
      else
      {
        r = static_cast<const ASTArray*>(r->pointee);
        mangled.append(mangle_number(r->size));
        mangled.push_back('@');
      }
    }
    mangled.append(mangle_param(r->pointee, false)); // recursive call
  }
  else if (typeid(*p) == typeid(ASTFunctor))
  {
    mangled.append("P6"); // this means function pointer apparently 
    mangled.push_back('A'); // _cdecl TODO use actual callig conv

    const ASTFunctor* f = static_cast<const ASTFunctor*>(p);

    // ref should now be the return type
    assert(f->return_type && "no functor return type");
    mangled.append(mangle_param(f->return_type, true));

    //const ASTNode* indirection = f->pointee;
    //while (indirection->pointee)
    //{
    //  indirection = indirection->pointee;
    //  mangled.append(mangle_qualifier(indirection->quals));
    //}

    // mangled.append(mangle_qualifier(indirection->quals));
    // mangled.push_back('P'); // one for each level of indirection
    
    // mangled.push_back('F');


    bool voidParam = false;
    if (!f->args.empty())
    {
      const auto first = f->args[0];
      if (typeid(*first) == typeid(ASTBuiltin))
      {
        const ASTBuiltin* b = static_cast<const ASTBuiltin*>(first);
        voidParam = BuiltinType::VOID == b->type_e;
      }
    }

    // functor params
    for (auto arg : f->args)
    {
      mangled.append(mangle_param(arg, false));
    }

    if (!f->args.empty() && !voidParam)
    {
      mangled.push_back('@');
    }

    mangled.push_back('Z');
  }
  else
  {
    assert(false && "Unknown parameter type");
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

  // Used for back references
  std::vector<std::string> paramHashes;
  for (auto p : params)
  {
    std::string mangledStr = mangle_param(p, false);
    if (mangledStr.length() == 1)
    {
      mangled << mangledStr;
      continue;
    }

    auto pos = std::find(paramHashes.begin(), paramHashes.end(), mangledStr);
    if (paramHashes.end() != pos)
    {
      unsigned int index = pos - paramHashes.begin();
      mangled << std::to_string(index);
    }
    else
    {
      mangled << mangledStr;
      paramHashes.push_back(mangledStr);
    }
  }

  if (!params.empty() && !voidParam)
  {
    mangled << "@";
  }

  mangled << "Z";

  return mangled.str();
}
