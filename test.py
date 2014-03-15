#!/usr/bin/env python
# vim: set fileencoding=UTF-8 :

#auto rebuild for debuging process
from subprocess import call
assert call('./setup.py build_ext -i',shell=True)==0

import czelta
import datetime
print "\n"*2
er = czelta.event_reader()
er.load("test.dat")
length = er.number_of_events()
assert length==92933
e = er.item(0)
assert e.TDC() == (976, 2509, 3759)
assert e.ADC() == (1026, 707, 884)
assert e.temps() == (9.5, 9.0, 9.5, 24.0)
assert e.timestamp() == 1388856009
assert e.calibration() == False
assert e.datetime() == datetime.datetime(2014, 1, 4, 17, 20, 9)
e = er[len(er)-1]
assert e.TDC() == (1306, 3762, 3461)
assert e.ADC() == (130, 100, 113)
assert e.temps() == (5.5, 5.0, 5.0, 25.0)
assert e.calibration() == False
assert e.timestamp() == 1391126394
calibrations = 0
for i in range(er.number_of_events()):
    if(er.item(i).calibration()):
        calibrations+=1
assert calibrations == 34016
assert er.filter_calibrations() == 34016
assert len(er) == length-calibrations


czelta.station.load()
kladno_sps = czelta.station(5)
assert kladno_sps.name() == "kladno_sps"
assert kladno_sps.exist()
assert kladno_sps.id() == 5
praha_utef = czelta.station(6)
assert praha_utef.name() == "praha_utef"
assert not czelta.station(125).exist()
print "success"
