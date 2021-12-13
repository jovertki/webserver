#!/usr/local/bin/python3

import cgi

data = cgi.FieldStorage() 
print(data.getfirst('wonderful_data'))