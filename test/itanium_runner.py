#!/usr/bin/env python

import argparse
import subprocess

def run_pipeline(expected, exe):
  ps = subprocess.Popen(['echo','-n', expected], stdout=subprocess.PIPE)
  ps2 = subprocess.Popen(exe, stdin=ps.stdout, stdout=subprocess.PIPE)
  ps.stdout.close()
  ps3 = subprocess.Popen(('c++filt'), stdin=ps2.stdout, stdout=subprocess.PIPE)
  ps2.stdout.close()
  output = ps3.communicate()[0].rstrip('\n')
  return output

def main():
  parser = argparse.ArgumentParser(description=
                                   'Test runner for MrMangler using Linux c++filt to verify manglings.')
  parser.add_argument('filename', help='Input file containing function signatures to test. One signature per line.')
  parser.add_argument('binary', help='MrMangler binary executable to test.')
  args = parser.parse_args()

  with open(args.filename) as f:
    passes = []
    fails = []
    for line in f:
      line = line.rstrip('\n')
      result = run_pipeline(line, args.binary)
      if result != line:
        fails.append((line,result))
      else:
        passes.append(line)

    print("Total tests run: {0}".format((len(passes) + len(fails))))
    print("Passes: {0}".format(len(passes)))
    print("Fails: {0}".format(len(fails)))
    for (expected, actual) in fails:
        print('\tExpected "{0}", was "{1}"'.format(expected, actual))

if __name__ == '__main__':
  main()
