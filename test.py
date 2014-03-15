#!/usr/bin/env python
# vim: set fileencoding=UTF-8 :

#auto rebuild for debuging process
from subprocess import call
assert call('./setup.py build_ext -i',shell=True)==0

import czelta
print "\n"*2
a = czelta.event_reader()
a.load("test.dat")
assert a.number_of_events()==92933
a.test()
print "success"
