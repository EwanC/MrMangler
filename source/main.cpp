#include <cstring>
#include <iostream>

#include "FuncDecl.h"
#include "MrMangler.h"

void printHelp()
{
  std::cout << "Usage: MrMangler [optional args] <input file>" << std::endl;
  std::cout << std::endl;

  std::cout << "Positional args:" << std::endl;
  std::cout << "<input file>       Input file to read from, otherwise "
               "use stdin."
            << std::endl;
  std::cout << std::endl;

  std::cout << "Optional args:" << std::endl;
  std::cout << "  [-h|--help]      Print this message." << std::endl;

  std::cout << "  [-i|--itanium]   Use itanium mangling. This is the"
               " defualt mangling scheme."
            << std::endl;

  std::cout << "  [-w|--windows]   Use windows decoration." << std::endl;
}

mangle_fn parseArgs(int argc, char** argv, FILE*& input_file)
{
  mangle_fn return_fn = &mangle_itanium;
  for (int i = 1; i < argc; ++i)
  {
    if (0 == strcmp(argv[i], "--help") || 0 == strcmp(argv[i], "-h"))
    {
      printHelp();
      exit(0);
    }

    if (0 == strcmp(argv[i], "--windows") || 0 == strcmp(argv[i], "-w"))
    {
      return_fn = mangle_windows;
    }

    if (0 == strcmp(argv[i], "--Itanium") || 0 == strcmp(argv[i], "-i"))
    {
      return_fn = mangle_itanium;
    }

    if ('-' != argv[i][0])
    {
      input_file = fopen(argv[i], "r");
      if (!input_file)
      {
        std::cout << "Error opening input file " << argv[i] << std::endl;
        exit(0);
      }
    }
  }

  return return_fn;
}

int main(int argc, char** argv)
{
  mangle_fn target_mangler = &mangle_itanium;
  FILE* f = nullptr;
  if (argc > 1)
    target_mangler = parseArgs(argc, argv, f);

  const std::shared_ptr<FuncDecl> func_decl(ParseStdin(f));
  if (f)
    fclose(f);

  const std::string mangled_str = target_mangler(func_decl);
  std::cout << mangled_str << std::endl;

  return 0;
}
