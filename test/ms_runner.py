#!/usr/bin/env python
''' Test harness for validating MrMangler binary

Checks windows manglings generated by MrMangler matches
the MSVC manglings generated by clang-cl.
'''

import argparse
import os
import subprocess
import sys
import tempfile


def run_mangler(func_signature, exe):
    ''' Runs MrMangler executable

    Args:
        func_signature  signature to mangle
        exe MrMangler executable

    Returns:
        (return code, output) - return code and stdout from MrMangler execution
    '''
    child_echo = subprocess.Popen(
        ['echo', '-n', func_signature], stdout=subprocess.PIPE)
    child_mangler = subprocess.Popen(
        [exe, '-w'], stdin=child_echo.stdout, stdout=subprocess.PIPE)
    child_echo.stdout.close()
    output = child_mangler.communicate()[0].rstrip(b'\n')
    return (child_mangler.returncode, output.decode())


def run_clang_cl(func_sig):
    ''' Runs clang_cl executable over stub function

    Args:
        func_signature  signature to mangle

    Returns:
        (return code, output) - return code and mangled symbol
    '''
    return_code = None
    symbol = None
    with tempfile.NamedTemporaryFile(delete=False, suffix=".cpp") as temp_fd:
        # Generate stub function in file
        temp_fd.write(b"#include <stdint.h>\n")
        temp_fd.write(b"union U1 {int i; U1(int i_): i(i_){}};\n")
        temp_fd.write(b"struct S0 {int i; S0(int i_): i(i_){}};\n")
        temp_fd.write(func_sig.rstrip(';').encode())
        if func_sig.startswith('void'):
            temp_fd.write(b"\n{\n\n}\n")
        else:
            temp_fd.write(b"\n{\n  return 0;\n}\n")
        temp_fd.close()

        tmp_obj = temp_fd.name[:-3] + "obj"  # output file

        return_code = subprocess.call(['clang-cl', '-o', tmp_obj,
                                       '-m32', '-c', temp_fd.name])

        # Scrape mangled symbol using nm
        child_nm = subprocess.Popen(
          ['nm', tmp_obj], stdout=subprocess.PIPE)

        output = child_nm.communicate()
        return_code = child_nm.returncode
        for line in output[0].split():
            line = line.decode()
            # Avoid struct constructors
            if line[0] == '?' and line[1] != '?':
                symbol = line
                break

    return (return_code, symbol)


def validate_environment(filename, exe):
    '''Checks script arguments and platform, exiting if not suitable

    Args:
        filename - Input file containing func decls to test
        exe - MrMangler executable file to test
    '''
    def is_exe(path):
        ''' Returns True if @path exists and has executable permissions '''
        return os.path.isfile(path) and os.access(path, os.X_OK)

    if os.name != 'posix':
        print('Test script only supports *nix systems')
        sys.exit()

    if not os.path.isfile(filename):
        print('Could not find input file ' + filename)
        sys.exit()

    if not is_exe(exe):
        print('Could not find test executable ' + exe)
        sys.exit()

    for tool in ['clang-cl', 'nm']:
        found = False
        for path in os.environ["PATH"].split(os.pathsep):
            path = path.strip('"')
            exe_file = os.path.join(path, tool)
            if is_exe(exe_file):
                found = True

        if not found:
            print('Could not locate %s in PATH' % (tool))
            sys.exit()


def main():
    '''Script entry point

    Returns(int): number of fails
    '''
    parser = argparse.ArgumentParser(
        description='Test runner for MrMangler using Linux clang-cl '
                    'to verify manglings.')
    parser.add_argument(
        'filename', help='Input file containing function signatures to test. '
                         'One signature per line.')
    parser.add_argument('binary', help='MrMangler binary executable to test.')
    args = parser.parse_args()

    # Exit script if input files don't exist or not running on supported OS
    validate_environment(args.filename, args.binary)

    with open(args.filename) as test_script_fd:
        passes = []  # list containing passing inputs
        fails = []  # list containing tuple of fails '(input, error)'
        for line in test_script_fd:
            line = line.rstrip('\n')
            # Mangle function decl with MrMangler
            (return_code, mr_mangled) = run_mangler(line, args.binary)
            if return_code != 0:
                fails.append((line, mr_mangled, ''))
                continue

            # Mangle function decl with clang-cl
            (return_code, cl_mangled) = run_clang_cl(line)
            if return_code != 0:
                fails.append((line, mr_mangled, cl_mangled))
                continue

            # Pass if manglings match
            if mr_mangled == cl_mangled:
                passes.append(line)
                continue
            else:
                fails.append((line, mr_mangled, cl_mangled))

        # Print test results
        print("Total tests run: {0}".format((len(passes) + len(fails))))
        print("Passes: {0}".format(len(passes)))
        print("Fails: {0}".format(len(fails)))
        for (orig, expect, correct) in fails:
            print('\t{0}: Expected "{1}", was "{2}"'.format(
                orig, expect, correct))
        return len(fails)

if __name__ == '__main__':
    ret_code = main()
    sys.exit(ret_code)
