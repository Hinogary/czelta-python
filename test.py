#!/usr/bin/env python
# vim: set fileencoding=UTF-8 :

#auto rebuild for debuging process
from subprocess import call
assert call('./setup.py build_ext -i',shell=True)==0
import czelta
reload(czelta)
import datetime
print "\n"*2


er = czelta.event_reader()
er.load("test.dat")

assert er.number_of_runs()==6
assert er.number_of_events(0)==13693
assert er.number_of_events(5)==9299
l = 0
for i in range(er.number_of_runs()):
    l+= er.number_of_events(i)
assert l==len(er)
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
assert str(er[157]) == "c 2014 01 04 18 12 02 764.0 4095 1755 3773 908 862 661 10.5 10.0 10.5 24.0"
calibrations = 0
for event in er:
    if(event.calibration()):
        calibrations+=1
assert calibrations == 34016
assert er.filter_calibrations() == calibrations
assert len(er) == length-calibrations


czelta.station.load()
kladno_sps = czelta.station(5)
assert kladno_sps.name() == "kladno_sps"
assert kladno_sps.exist()
assert kladno_sps.id() == 5
assert kladno_sps.gps_position() == (50.1404958, 14.1009331, 446.18)
praha_utef = czelta.station(6)
assert praha_utef.name() == "praha_utef"
assert kladno_sps.distance_to(praha_utef) == 24.34704590484944
assert not czelta.station(125).exist()
stations = czelta.station.get_stations()
assert stations[0].id() == 2
assert stations[0].name() == "pardubice_gd"
assert stations[1].id() == 3
assert stations[1].name() == "opava_mg" 

txt = czelta.event_reader('test.txt')
assert str(txt[16000])=="a 2014 02 24 19 58 17 222931733.5 1231 2395 3762 1022 404 770 10.0 9.5 8.5 26.5"
assert len(txt)==16981
assert txt.filter_calibrations() == 6391
print "success"
