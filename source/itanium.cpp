#include <cstring>
#include <sstream>

#include "FuncDecl.h"
#include "MrMangler.h"

std::string mangle_param(const FuncParam *p) {
    if (BuiltinType::CHAR == p->type_e) {
      return "c";
    }

    if (BuiltinType::DOUBLE == p->type_e){
      return "d";

    }
    if (BuiltinType::FLOAT == p->type_e){
      return "f";
    }

    if (BuiltinType::INT == p->type_e){
       return "i";
    }

    if (BuiltinType::SHORT == p->type_e){
       return "s";
    }

    if (BuiltinType::BOOL == p->type_e){
       return "b";
    }

    if (BuiltinType::LONG == p->type_e){
       return "l";
    }

    if (BuiltinType::VOID == p->type_e){
       return "v";
    }

}

std::string mangle_itanium(const FuncDecl* decl)
{
    std::ostringstream mangled;
    mangled << "_Z" << strlen(decl->name) << decl->name;

    const std::vector<const FuncParam *> &params = decl->params;
    if (params.empty())
    {
       mangled << "v";
       return mangled.str();
    }

    for(auto p : params)
       mangled << mangle_param(p);

    return mangled.str();
}
