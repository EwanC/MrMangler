#include <cstring>
#include <iostream>

#include "FuncDecl.h"
#include "MrMangler.h"

void printHelp()
{
  std::cout << "Usage: MrMangler [optional args] <input file>" << std::endl;

  std::cout << "Mangle function declaration from standard input." << std::endl;
  std::cout << "Example: echo \"foo(float a)\" | ./MrMangler" << std::endl;
  std::cout << std::endl;

  std::cout << "Optional args:" << std::endl;
  std::cout << "  [-h|--help]" << std::string(24, ' ') << "Print this message." << std::endl;

  std::cout << "  [-i|--itanium]" << std::string(21, ' ') << "Use itanium mangling."
                                                             "  This is the defualt mangling scheme."
            << std::endl;

  std::cout << "  [-w|--windows]" << std::string(21, ' ') << "Use windows decoration." << std::endl;

  std::cout << "  --cc={cdecl, fastcall, stdcall}" << std::string(4, ' ')
            << "Calling convention used for msvc mangling, defaults to cdecl." << std::endl;
}

std::pair<mangle_fn, CCOption_e> parseArgs(int argc, char** argv)
{
  mangle_fn return_fn = &mangle_itanium;
  CCOption_e return_cc = CCOption_e::Cdecl;
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
      continue;
    }

    if (0 == strcmp(argv[i], "--Itanium") || 0 == strcmp(argv[i], "-i"))
    {
      return_fn = mangle_itanium;
      continue;
    }

    static const char* cc_str = "--cc=";
    static const size_t cc_len = strlen(cc_str);
    if (0 == strncmp(argv[i], cc_str, cc_len))
    {
      const char* cc_val = argv[i] + cc_len;
      if (0 == strcmp(cc_val, "cdecl"))
        return_cc = CCOption_e::Cdecl;
      else if (0 == strcmp(cc_val, "fastcall"))
        return_cc = CCOption_e::fastcall;
      else if (0 == strcmp(cc_val, "stdcall"))
        return_cc = CCOption_e::stdcall;
      else
        std::cout << "Unknown calling convention '" << cc_val << "' defaulting to cdecl." << std::endl;
      continue;
    }

    std::cout << "Ignoring unknown option " << argv[i] << std::endl;
  }

  return {return_fn, return_cc};
}

int main(int argc, char** argv)
{
  mangle_fn target_mangler = &mangle_itanium;
  CCOption_e target_cc = CCOption_e::Cdecl;

  if (argc > 1)
  {
    auto parsed_pair = parseArgs(argc, argv); // structured bindings in future
    target_mangler = parsed_pair.first;
    target_cc = parsed_pair.second;
  }

  const std::shared_ptr<FuncDecl> func_decl(ParseStdin());

  const std::string mangled_str = target_mangler(func_decl, target_cc);
  std::cout << mangled_str << std::endl;

  return 0; // return success
}
