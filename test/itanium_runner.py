#!/usr/bin/env python

import argparse
import subprocess

def main():
  parser = argparse.ArgumentParser(description=
                                   'Test runner for MrMangler using Linux c++filt to verify manglings.')
  parser.add_argument('filename', help='Input file containing function signatures to test. One signature per line.')
  args = parser.parse_args()

  with open(args.filename) as f:
    for line in f:
      line = line.rstrip('\n')
      ps = subprocess.Popen(('echo','-n', line), stdout=subprocess.PIPE)
      ps2 = subprocess.Popen('../build/MrMangler', stdin=ps.stdout, stdout=subprocess.PIPE)
      output = subprocess.check_output(('c++filt'), stdin=ps.stdout)
      ps.wait()
      ps2.wait()
      output = output.rstrip('\n')
      print(output, line)

if __name__ == '__main__':
  main()
