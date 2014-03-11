#!/usr/bin/env python
# vim: set fileencoding=UTF-8 :
#import os, sys
from subprocess import call
call('./setup.py build_ext -i',shell=True)

import czelta
print "\n"*2
a = czelta.eventreader()
a.load("test.dat")
print a.number_of_events()
