#!/usr/bin/env python

import argparse

def main():
  parser = argparse.ArgumentParser(description=
                                   'Test runner for MrMangler using Linux c++filt to verify manglings.')

  # TODO Take filename as input
  args = parser.parse_args()
  
  #####
  # Mad pseudocode
  # Open input file
  # loop over every line
  #   pass line into MrMangler
  #   pipe output into c++filt
  #   verify it matches the input

if __name__ == '__main__':
  main()
