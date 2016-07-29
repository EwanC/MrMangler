#include <cstring>
#include <iostream>

#include "MrMangler.h"
#include "FuncDecl.h"

mangle_fn parseArgs(int argc, char** argv)
{
    for(int i=1; i < argc; ++i) {
      if(0 == strcmp(argv[i], "--help") ||
         0 == strcmp(argv[i], "-h")) {
           std::cout << "help:" <<std::endl;
           exit(0);
         }
      if(0 == strcmp(argv[i], "--windows") ||
         0 == strcmp(argv[i], "-w")) {
           return &mangle_windows;
         }
      if(0 == strcmp(argv[i], "--Itanium") ||
         0 == strcmp(argv[i], "-i")) {
          return &mangle_itanium;
         }
    }
}

int main(int argc, char** argv)
{
    mangle_fn target_mangler = &mangle_itanium;
    if (argc > 0)
        target_mangler = parseArgs(argc, argv);

    const FuncDecl* func_decl = ParseStdin();
    const std::string mangled_str = target_mangler(func_decl);
    std::cout << mangled_str << std::endl;

    return 0;
}
