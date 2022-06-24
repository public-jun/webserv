#!/usr/bin/python3
# -*- coding: utf-8 -*-
import cgi
import cgitb
cgitb.enable()

print("Content-Type: text/html")
print()

form = cgi.FieldStorage()

value1 = form['value1'].value
value2 = form['value2'].value
print(value1)
print(value2)
