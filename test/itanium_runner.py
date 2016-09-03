#!/usr/bin/env python

import argparse
import os
import subprocess
import sys


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
        exe, stdin=child_echo.stdout, stdout=subprocess.PIPE)
    child_echo.stdout.close()
    output = child_mangler.communicate()[0].rstrip(b'\n')
    return (child_mangler.returncode, output)


def run_filt(mangled):
    ''' Runs c++filt executable

    Args:
        mangled  mangled symbol to demangle

    Returns:
        (return code, output) - return code and stdout from c++filt execution
    '''
    child_echo = subprocess.Popen(
        ['echo', '-n', mangled], stdout=subprocess.PIPE)
    child_filt = subprocess.Popen(('c++filt'), stdin=child_echo.stdout,
                                  stdout=subprocess.PIPE)
    child_echo.stdout.close()
    output = child_filt.communicate()[0].rstrip(b'\n')
    return (child_filt.returncode, output)


def validate_environment(filename, exe):
    '''Checks script arguments and platform, exiting if not suitable

    Args:
        filename - Input file containing func decls to test
        exe - MrMangler executable file to test
    '''
    def is_exe(path):
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

    # check c++filt exist
    found = False
    for path in os.environ["PATH"].split(os.pathsep):
        path = path.strip('"')
        exe_file = os.path.join(path, 'c++filt')
        if is_exe(exe_file):
            found = True

    if not found:
        print('Could not locate c++filt in PATH')
        sys.exit()


def main():
    '''Script entry point'''
    parser = argparse.ArgumentParser(
        description='Test runner for MrMangler using Linux c++filt '
                    'to verify manglings.')
    parser.add_argument(
        'filename', help='Input file containing function signatures to test. '
                         'One signature per line.')
    parser.add_argument('binary', help='MrMangler binary executable to test.')
    args = parser.parse_args()

    # Exit script if input files don't exist or not running on supported OS
    validate_environment(args.filename, args.binary)

    with open(args.filename) as f:
        passes = []  # list containing passing inputs
        fails = []  # list containing tuple of fails '(input, error)'
        for line in f:
            line = line.rstrip('\n')
            # Mangle function decl
            (rc, mangled) = run_mangler(line, args.binary)
            if rc != 0:
                fails.append((line, mangled))
                continue

            # Demangle our mangling
            (rc, demangled) = run_filt(mangled)
            if rc != 0:
                fails.append((line, mangled))
                continue

            # Check if demangling matches original decl
            if demangled == line:
                passes.append(line)
                continue

            # When demangling and original differ then mangle the demangling,
            # if this matches the original mangling then our mangling was
            # correct.
            (rc, fallback) = run_mangler(demangled, args.binary)
            if mangled is fallback and rc is 0:
                passes.append(line)
            else:
                fails.append((line, mangled))

        # Print test results
        print("Total tests run: {0}".format((len(passes) + len(fails))))
        print("Passes: {0}".format(len(passes)))
        print("Fails: {0}".format(len(fails)))
        for (expected, actual) in fails:
            print('\tExpected "{0}", was "{1}"'.format(expected, actual))

if __name__ == '__main__':
    main()
