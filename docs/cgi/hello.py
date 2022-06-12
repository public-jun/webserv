#!/usr/bin/python3
import os
import sys

print('----Args----')
args = sys.argv
for arg in args:
    print(arg)

print('----Environ----')
print(os.environ)
