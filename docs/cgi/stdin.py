#!/usr/bin/python3
import sys
import os

print('Content-type: text/html; charset=utf-8')
print('')
print('call stdin.py')
print('CONTENT_LENGTH: ', os.environ['CONTENT_LENGTH'])
print('------CONTENT START-------')
input = sys.stdin.read(int(os.environ['CONTENT_LENGTH']))
print(input)
print('------CONTENT   END-------')