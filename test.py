#!/usr/bin/env python3
# vim: set fileencoding=UTF-8 :

#auto rebuild for debuging process
import sys
if len(sys.argv):
    from subprocess import call
    assert call('./setup.py build_ext -i',shell=True)==0
import czelta
import datetime
from time import time
print("\n"*2)

start = time()
b = time()
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

l = []
for run in er.runs():
    local_l = 0
    for event in run:
        local_l+=1
    l.append(local_l)
assert l==[13693, 19615, 27063, 19198, 4065, 9299]


assert er[0].timestamp == er.runs()[0][0].timestamp
assert er.runs()[0][-1].timestamp == 1389163699
e = er[0]
assert e.TDC == (976, 2509, 3759)
assert e.ADC == (1026, 707, 884)
assert e.temps_detector == (9.5, 9.0, 9.5)
assert e.temp_crate == 24.0
assert e.timestamp == 1388856009
assert e.calibration == False
assert e.datetime == datetime.datetime(2014, 1, 4, 17, 20, 9)
e = er[len(er)-1]
assert e.TDC == (1306, 3762, 3461)
assert e.ADC == (130, 100, 113)
assert e.temps_detector == (5.5, 5.0, 5.0)
assert e.temp_crate == 25.0
assert e.calibration == False
assert e.timestamp == 1391126394
assert str(er[157]) == "c 2014 01 04 18 12 02 764.0 4095 1755 3773 908 862 661 10.5 10.0 10.5 24.0",str(er[157])
calibrations = 0
for event in er:
    if(event.calibration):
        calibrations+=1
assert calibrations == 34016
calibrations = er.filter_calibrations()
assert 34016 == calibrations, "Calibrations: %i"%calibrations
assert len(er) == length-calibrations

#test that runs are allright after filter
l = []
for run in er.runs():
    local_l = 0
    for event in run:
        local_l+=1
    l.append(local_l)
assert l==[8576, 12239, 17336, 12167, 2641, 5958]

czelta.station.load()
kladno_sps = czelta.station(5)
assert kladno_sps.name == "kladno_sps"
assert kladno_sps.id == 5
assert kladno_sps.gps_position == (50.1404958, 14.1009331, 446.18)
praha_utef = czelta.station(6)
assert praha_utef.name == "praha_utef"
assert kladno_sps.distance_to(praha_utef) == 24.34704590484944
assert czelta.station("pardubice_gd").id == 2 
try:
    assert not czelta.station(125)
except RuntimeError:
    pass
stations = czelta.station.get_stations()
assert stations[0].id == 2
assert stations[0].name == "pardubice_gd"
assert stations[1].id == 3
assert stations[1].name == "opava_mg" 

er.set_station('pardubice_spse')
assert er[0].AH_direction == (120.57520294189453, 63.383602142333984), er[0].AH_direction
assert er[1].AH_direction == (180.72100830078125, 82.62873840332031), er[1].AH_direction
assert er[2].AH_direction == (20.038354873657227, 86.3436050415039), er[2].AH_direction
assert er[4].AH_direction == (281.7551574707031, 69.92161560058594), er[4].AH_direction
assert er[5].AH_direction == (88.83272552490234, 71.3564682006836), er[5].AH_direction
assert er[26].AH_direction == (254.46205139160156, 58.906494140625), er[26].AH_direction
assert er[22].AH_direction == (345.6707458496094, 57.61233139038086), er[22].AH_direction

txt = czelta.event_reader('test.txt')
assert str(txt[1000])=='a 2014 02 18 12 22 02 450020121.2 1712 2927 3776 139 136 145 15.0 13.5 12.5 23.5'
assert str(txt[7999])=='a 2014 02 21 00 17 50 662546970.9 1532 2241 3772 856 479 722 4.5 5.0 5.0 22.5'
try:
    assert str(txt[16000])=="a 2014 02 24 19 58 17 222931733.5 1231 2395 3762 1022 404 770 10.0 9.5 8.5 26.5"
except AssertionError:
    #win32 ...
    assert str(txt[16000])=="a 2014 02 24 19 58 17 222931733.1 1231 2395 3762 1022 404 770 10.0 9.5 8.5 26.5",str(txt[16000])
assert len(txt)==16981
evs = txt[datetime.datetime(2014,2,18,6,13):datetime.datetime(2014,2,18,6,14)]
assert len(evs)==3
assert str(evs[0])=='a 2014 02 18 06 13 03 606212753.5 1704 2650 3769 237 292 139 8.5 0.5 1.0 21.5'
assert str(evs[1])=='a 2014 02 18 06 13 34 148539574.9 3782 3650 4095 41 45 0 8.0 0.5 1.0 21.5'
assert str(evs[2])=='c 2014 02 18 06 13 51 878.0 4095 4095 3742 1044 1094 806 8.0 0.5 1.0 21.5'

assert er.filter_maximum_TDC() == 1842
assert er.filter_maximum_ADC() == 2687
assert er.filter_minimum_ADC() == 9
er = czelta.event_reader('test.dat')

#filter all calibrations
def filter_calibrations(e):
    return e.calibration

#filter events with maximum tdc
def filter_func_maximum_tdc(e):
    tdc = e.TDC
    return tdc[0]==4095 or tdc[1]==4095 or tdc[2]==4095

assert er.filter(filter_calibrations) == 34016
assert er.filter(filter_func_maximum_tdc) == 1842

assert txt.filter_calibrations() == 6391
assert txt.filter_maximum_TDC() == 281
assert txt.filter_maximum_ADC() == 439
assert txt.filter_minimum_ADC() == 1

def advanced_filter(e):
    for adc in e.ADC:
        if adc==0 or adc==2047 or adc<500:
            return True
    for tdc in e.TDC:
        if tdc==4095:
            return True

#test saving
s_er = czelta.event_reader('test.dat')
s_er.set_station('pardubice_spse')
s_er.filter(advanced_filter)
assert len(s_er) == 2176
s_er.save('some_test.txt')
txt = czelta.event_reader('some_test.txt')
s_er.save('some_test.dat')
dat = czelta.event_reader('some_test.dat')
assert len(dat) == len(txt)
assert len(dat) == len(s_er)
for i in range(len(s_er)):
    assert str(dat[i]) == str(txt[i])
    assert str(dat[i]) == str(s_er[i])
for i in range(len(s_er.runs())):
    assert len(s_er.run(i))==len(txt.run(i))
    assert len(s_er.run(i))==len(dat.run(i))
    assert s_er.run(i)[0].timestamp == txt.run(i)[0].timestamp
    assert s_er.run(i)[0].timestamp == dat.run(i)[0].timestamp
    assert s_er.run(i)[-1].timestamp == txt.run(i)[-1].timestamp
    assert s_er.run(i)[-1].timestamp == dat.run(i)[-1].timestamp
    
er2 = czelta.event_reader('testb.txt')
er = czelta.event_reader('test.dat')
c = czelta.coincidence((er, er2), 2.2e-6)
assert len(c)==3
assert c[0][0] == 6.529e-07
assert str(c[0][1])=='a 2014 01 26 03 41 54 695297052.6 703 3755 3674 152 222 114 9.0 9.0 8.5 25.0'
assert str(c[0][2])=='a 2014 01 26 03 41 54 695297705.5 1835 1619 3765 185 116 162 6.5 2.0 8.0 21.5'
assert c[1][0] == 3.074e-7
assert c[2][0] == 1.2084e-06
assert str(c[2][1]) == 'a 2014 01 26 23 22 54 168319730.5 1045 3763 4095 444 130 0 1.5 0.5 0.5 25.0'
assert str(c[2][2]) == 'a 2014 01 26 23 22 54 168320938.9 1836 3776 2084 302 151 250 1.0 1.0 1.5 22.0'
c = czelta.coincidence((er, er2), 0.05, save_events = False)
assert len(c) == 107
assert [x[0] for x in c] == [0.0117102349, 0.0445123785, 0.0083446565, 0.0105750367, 0.0123610337, 0.0122573222, 0.0184334218, 0.0342116597, 0.0143588383, 0.03220545, 0.0326061588, 0.0394253804, 0.0022431182, 0.0293818174, 0.0447717523, 0.0459139034, 0.0017015944, 0.0195749612, 0.0003202613, 0.0432431746, 0.0127860794, 0.0217298469, 0.0003565159, 0.0465947056, 0.0273502297, 0.0434273089, 0.012132375, 0.0052076247, 0.0171989055, 0.0190139951, 0.0224912048, 0.0379027571, 0.0268288315, 0.0279393079, 0.0476068155, 0.0343189447, 0.0122926072, 0.0172816184, 0.0186405369, 0.0045763289, 0.0447592478, 0.0091815497, 0.0013873129, 0.0176463732, 0.035799317, 0.0194873205, 0.0190261298, 0.036038937, 0.0210685217, 0.0494162211, 0.0315581834, 0.0373937835, 0.0297602949, 0.0308353127, 0.0031860697, 0.0056731606, 0.0083078373, 0.0373496079, 0.0038110098, 0.0231314182, 0.0142001561, 0.0470827444, 0.0204108362, 0.0358141634, 0.040363858, 0.0019488216, 0.0319969031, 0.0441464683, 0.0349707935, 0.0306262113, 0.0064469361, 0.0278795945, 0.045103594, 0.0204716053, 0.007940002, 0.021382778, 0.0257969825, 0.0243460279, 6.529e-07, 0.0386499095, 0.0217462171, 0.0043902238, 0.0032857979, 0.0216106306, 3.074e-07, 1.2084e-06, 0.0382442583, 0.0198218289, 0.0036226989, 0.0042712333, 0.0187938069, 0.0490996574, 0.0172133696, 0.0328668443, 0.0003268801, 0.0469025091, 0.0481503563, 0.0483244014, 0.0301566928, 0.0041591962, 0.0334477928, 0.0155284452, 0.0284977972, 0.0017501542, 0.0350713617, 0.0073797644, 0.0003269897]
print("success")
end = time()
print("time: %f s"%(end-start))
