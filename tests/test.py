#!/usr/bin/env python3

import pytest
import czelta

import datetime
from copy import copy


@pytest.fixture
def basic_er():
    return czelta.event_reader('tests/test.dat')


@pytest.fixture
def first_event(basic_er):
    return basic_er[0]


@pytest.fixture
def last_event(basic_er):
    return basic_er[-1]


def test_length_basic_er(basic_er):
    length = basic_er.number_of_events()
    assert length == 92933


def test_length_of_runs_basic_er(basic_er):
    lens = []
    for run in basic_er.runs():
        local_l = 0
        for event in run:
            local_l += 1
        lens.append(local_l)
    assert lens == [13693, 19615, 27063, 19198, 4065, 9299]


def test_total_length_and_runs_length(basic_er):
    ln = 0
    for i in range(er.number_of_runs()):
        ln += er.number_of_events(i)
    assert ln == len(er)


def test_first_last_timestamp(basic_er):
    assert basic_er.runs()[0][-1].timestamp == 1389163699
    assert basic_er.runs()[0][-1].timestamp == 1389163699


def test_first_event(first_event):
    e = first_event
    assert e.TDC == (976, 2509, 3759)
    assert e.ADC == (1026, 707, 884)
    assert e.temps_detector == (9.5, 9.0, 9.5)
    assert e.temp_crate == 24.0
    assert e.timestamp == 1388856009
    assert not e.calibration
    assert e.datetime == datetime.datetime(2014, 1, 4, 17, 20, 9)


def test_last_event(last_event):
    e = last_event
    assert e.TDC == (1306, 3762, 3461)
    assert e.ADC == (130, 100, 113)
    assert e.temps_detector == (5.5, 5.0, 5.0)
    assert e.temp_crate == 25.0
    assert not e.calibration
    assert e.timestamp == 1391126394
    assert str(er[157]) == "c 2014 01 04 18 12 02 764.0 4095 1755 3773 908 862 661 10.5 10.0 10.5 24.0",str(er[157])


def test_event_reader_copy(basic_er):
    copy_er = copy(basic_er)
    assert len(copy_er) == len(basic_er)
    for i in range(len(basic_er)):
        assert basic_er[i] == copy_er[i]


def test_calibrations(basic_er):
    calibrations = 0
    for event in basic_er:
        if(event.calibration):
            calibrations += 1
    assert calibrations == 34016


def test_runs_same_as_non_runs_events(basic_er):
    start = 0
    for run in basic_er.runs():
        for i in range(len(run)):
            assert run[i] == basic_er[start + i]
        start += len(run)


def test_filter_calibrations(basic_er):
    copy_er = copy(basic_er)
    calibs = 0
    for e in basic_er:
        if e.calibration:
            calibs += 1

    filtered = copy_er.filter_calibrations()

    assert filtered == calibs
    assert len(basic_er) == calibs + len(copy_er)
    assert calibs == 34016


def test_runs_after_filter(basic_er):
    copy_er = copy(basic_er)
    copy_er.filter_calibrations()
    lens = []
    for run in copy_er.runs():
        local_l = 0
        for event in run:
            local_l += 1
        lens.append(local_l)
    assert lens == [8576, 12239, 17336, 12167, 2641, 5958]


er = czelta.event_reader()
er.load("tests/test.dat")
er.filter_calibrations()


@pytest.fixture
def kladno_sps():
    return czelta.station(5)


@pytest.fixture
def praha_utef():
    return czelta.station(6)


def test_station_kladno(kladno_sps):
    assert kladno_sps.name == "kladno_sps"
    assert kladno_sps.id == 5
    assert kladno_sps.gps_position == (50.1404958, 14.1009331, 446.18)


def test_station_praha(praha_utef):
    assert praha_utef.name == "praha_utef"
    assert czelta.station("pardubice_gd").id == 2


def test_station_distance(kladno_sps, praha_utef):
    assert kladno_sps.distance_to(praha_utef) == 24347.04590484944


def test_nonexisting_station():
    with pytest.raises(RuntimeError):
        czelta.station(125)


def test_er_out_of_range(basic_er):
    with pytest.raises(IndexError):
        er[len(basic_er)]

    with pytest.raises(IndexError):
        er[-len(er)-1]


def test_preloaded_stations():
    stations = czelta.station.get_stations()
    assert stations[0].id == 2
    assert stations[0].name == "pardubice_gd"
    assert stations[1].id == 3
    assert stations[1].name == "opava_mg"


def test_basic_directions(basic_er):
    er = copy(basic_er)
    er.filter_calibrations()
    er.set_station('pardubice_spse')
    assert er[0].AH_direction == (120.57520294189453, 63.383602142333984), er[0].AH_direction
    assert er[1].AH_direction == (180.72100830078125, 82.62873840332031), er[1].AH_direction
    assert er[2].AH_direction == (20.038354873657227, 86.3436050415039), er[2].AH_direction
    assert er[4].AH_direction == (281.7551574707031, 69.92161560058594), er[4].AH_direction
    assert er[5].AH_direction == (88.83272552490234, 71.3564682006836), er[5].AH_direction
    assert er[26].AH_direction == (254.46205139160156, 58.906494140625), er[26].AH_direction
    assert er[22].AH_direction == (345.6707458496094, 57.61233139038086), er[22].AH_direction


def test_loading_txt_file():
    txt = czelta.event_reader('tests/test.txt')
    assert str(txt[1000]) == 'a 2014 02 18 12 22 02 450020121.2 1712 2927 3776 139 136 145 15.0 13.5 12.5 23.5'
    assert str(txt[7999]) == 'a 2014 02 21 00 17 50 662546970.9 1532 2241 3772 856 479 722 4.5 5.0 5.0 22.5'
    try:
        assert str(txt[16000]) == 'a 2014 02 24 19 58 17 222931733.5 1231 2395 3762 1022 404 770 10.0 9.5 8.5 26.5'
    except AssertionError:
        assert str(txt[16000]) == "a 2014 02 24 19 58 17 222931733.1 1231 2395 3762 1022 404 770 10.0 9.5 8.5 26.5", str(txt[16000])
    assert len(txt) == 16981
    evs = txt[datetime.datetime(2014, 2, 18, 6, 13):datetime.datetime(2014, 2, 18, 6, 14)]
    assert len(evs) == 3
    assert str(evs[0]) == 'a 2014 02 18 06 13 03 606212753.5 1704 2650 3769 237 292 139 8.5 0.5 1.0 21.5'
    assert str(evs[1]) == 'a 2014 02 18 06 13 34 148539574.9 3782 3650 4095 41 45 0 8.0 0.5 1.0 21.5'
    assert str(evs[2]) == 'c 2014 02 18 06 13 51 878.0 4095 4095 3742 1044 1094 806 8.0 0.5 1.0 21.5'
    assert txt.filter_calibrations() == 6391
    assert txt.filter_maximum_TDC() == 281
    assert txt.filter_maximum_ADC() == 439
    assert txt.filter_minimum_ADC() == 1


def test_basic_filters(basic_er):
    er = copy(basic_er)
    assert er.filter_calibrations() == 34016
    assert er.filter_maximum_TDC() == 1842
    assert er.filter_maximum_ADC() == 2687
    assert er.filter_minimum_ADC() == 9

er = czelta.event_reader('tests/test.dat')


def test_basic_custom_filters(basic_er):
    er = copy(basic_er)
    # filter all calibrations
    def filter_calibrations(e):
        return e.calibration

    # filter events with maximum tdc
    def filter_func_maximum_tdc(e):
        tdc = e.TDC
        return tdc[0]==4095 or tdc[1]==4095 or tdc[2]==4095

    assert er.filter(filter_calibrations) == 34016
    assert er.filter(filter_func_maximum_tdc) == 1842


def advanced_filter(e):
    for adc in e.ADC:
        if adc == 0 or adc == 2047 or adc < 500:
            return True
    for tdc in e.TDC:
        if tdc == 4095:
            return True


def test_custom_filter(basic_er):
    er = copy(basic_er)
    assert er.filter(advanced_filter) == 90757


def test_saving_and_loading():
    #test saving
    s_er = czelta.event_reader('tests/test.dat')
    s_er.set_station('pardubice_spse')
    s_er.filter(advanced_filter)
    assert len(s_er) == 2176
    s_er.save('tests/some_test.txt')
    txt = czelta.event_reader('tests/some_test.txt')
    s_er.save('tests/some_test.dat')
    dat = czelta.event_reader('tests/some_test.dat')
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
    for i in range(len(dat)):
        assert dat[i] == txt[i]
        assert dat[i] == s_er[i]


def test_date_to_timestamp():
    fn = czelta.date_to_timestamp
    assert fn('27.4.2013 17:44:53') == 1367084693
    assert fn(datetime.datetime(2013, 4, 27, 17, 44, 53)) == 1367084693
    assert fn(b'27.4.2013 17:44:53') == 1367084693
    assert fn(1367084693) == 1367084693

    dt = datetime.datetime(2013, 4, 27, 19, 44, 53, tzinfo=datetime.timezone(datetime.timedelta(hours=2)))
    assert fn(dt) == 1367084693

    with pytest.raises(ValueError):
        fn(0.1)  # float is illegal


def test_station_get_correction():
    ps = czelta.station('pardubice_spse')
    assert ps.get_corrections() == [{'from': '01.01.1970 00:00', 'corrections': [0, -1227, -2215]}]
    pg = czelta.station('pardubice_gd')
    assert pg.get_corrections() == [{'from': '01.01.1970 00:00', 'corrections': [0, -2042, -1449]}, {'from': '01.02.2007 15:55', 'corrections': [0, -2657, -1456]}, {'from': '07.01.2008 12:01', 'corrections': [0, 451, -176]}, {'from': '26.06.2008 16:49', 'corrections': [0, 1613, -1012]}]


def test_station_autofitting_corrections(basic_er):
    er = czelta.event_reader('tests/pardubice_gd.dat')
    st = czelta.station('pardubice_gd')
    st.clear_corrections()
    assert st.get_corrections() == []
    st.autofit_corrections(er)
    assert st.get_corrections() == [
           {'corrections': [0, -2000, -1409],
            'from': '01.01.1970 00:00'},
           {'corrections': [0, -2535, -1355],
            'from': '01.02.2007 15:55'},
           {'corrections': [0, 398, -174],
            'from': '07.01.2008 12:01'},
           {'corrections': [0, 790, -1539],
            'from': '20.02.2008 17:46'},
           {'corrections': [0, 979, -1370],
            'from': '26.02.2008 12:45'},
           {'corrections': [0, 14, -27],
            'from': '04.04.2008 11:28'},
           {'corrections': [0, 974, -1421],
            'from': '28.05.2008 15:22'},
           {'corrections': [0, 1473, -999],
            'from': '26.06.2008 16:49'},
           {'corrections': [0, 1386, -1127],
            'from': '09.06.2009 15:25'},
           {'corrections': [0, 1471, -1004],
            'from': '09.06.2009 17:41'},
          ]


def test_coincidence():
    er2 = czelta.event_reader('tests/testb.txt')
    er = czelta.event_reader('tests/test.dat')
    c = czelta.coincidence((er, er2), 2.2e-6)
    assert len(c) == 3
    assert c[0][0] == 6.529e-07
    assert str(c[0][1]) == 'a 2014 01 26 03 41 54 695297052.6 703 3755 3674 152 222 114 9.0 9.0 8.5 25.0'
    assert str(c[0][2]) == 'a 2014 01 26 03 41 54 695297705.5 1835 1619 3765 185 116 162 6.5 2.0 8.0 21.5'
    assert c[1][0] == 3.074e-7
    assert c[2][0] == 1.2084e-06
    assert str(c[2][1]) == 'a 2014 01 26 23 22 54 168319730.5 1045 3763 4095 444 130 0 1.5 0.5 0.5 25.0'
    assert str(c[2][2]) == 'a 2014 01 26 23 22 54 168320938.9 1836 3776 2084 302 151 250 1.0 1.0 1.5 22.0'
    c = czelta.coincidence((er, er2), 0.05, save_events=True)
    assert len(c) == 252
    assert [x[0] for x in c] == [0.0117102349, 0.0445123785, 0.026879145, 0.0341340565, 0.0265390193, 0.01709074, 0.0083446565, 0.0105750367, 0.0378394216, 0.0399981458, 0.0123610337, 0.0122573222, 0.0184334218, 0.0342116597, 0.0052074785, 0.0383564726, 0.0404224403, 0.0246563908, 0.0201876049, 0.0143588383, 0.0263998664, 0.03220545, 0.0326061588, 0.029543123, 0.035396357, 0.0394253804, 0.0022431182, 0.0293818174, 0.0076137034, 0.0268187728, 0.0017470178, 0.0471216235, 0.0447717523, 0.0306211277, 0.0402346252, 0.0459139034, 0.0052949041, 0.0489775789, 0.0017015944, 0.0195749612, 0.0003202613, 0.0432431746, 0.0131019393, 0.0499583316, 0.0175647899, 0.0439216316, 0.0435944149, 0.0490370035, 0.0308218643, 0.0127860794, 0.0217298469, 0.0211764585, 0.0003565159, 0.0125406614, 0.0465947056, 0.0432392739, 0.0273502297, 0.0434273089, 0.012132375, 0.0052076247, 0.0171989055, 0.0071227328, 0.0424498615, 0.0190139951, 0.0224912048, 0.0097316882, 0.0379027571, 0.0496210578, 0.0170557102, 0.0268288315, 0.0279393079, 0.0476068155, 0.0148278178, 0.0298284942, 0.0343189447, 0.0122926072, 0.0389359017, 0.0406783498, 0.0172816184, 0.0275777262, 0.0259777305, 0.0473260656, 0.0043536694, 0.015664037, 0.0064402229, 0.0158923798, 0.0186405369, 0.0156553428, 0.0313689167, 0.0045763289, 0.0447592478, 0.0287936307, 0.0418154172, 0.0244108064, 0.0237870059, 0.0091815497, 0.0013873129, 0.0398865969, 0.0372302027, 0.0059017754, 0.0176463732, 0.0274280243, 0.0164486484, 0.0137589757, 0.0185438308, 0.0147871597, 0.0181796277, 0.0130338607, 0.035799317, 0.0458630073, 0.0072931739, 0.0194873205, 0.0406585944, 0.0399642809, 0.0190261298, 0.001056084, 0.0333413085, 0.036038937, 0.0172320998, 0.0088914186, 0.0045156289, 0.0221562835, 0.0131112048, 0.0095416961, 0.0482835844, 0.0066898317, 0.0282867439, 0.0210685217, 0.032852211, 0.0494162211, 0.0157103199, 0.0315581834, 0.0373937835, 0.0474139122, 0.0297602949, 0.0426718281, 0.0308353127, 0.0099303278, 0.0031860697, 0.0175112537, 0.0495988122, 0.0056731606, 0.0380882899, 0.0032585251, 0.0083078373, 0.0306103322, 0.0119571934, 0.0373496079, 0.0038110098, 0.0325164344, 0.0231314182, 0.0169089608, 0.0142001561, 0.0470827444, 0.0002973863, 0.0341775213, 0.0424980219, 0.0080381953, 0.0204108362, 0.013023241, 0.0358141634, 0.040363858, 0.0205848776, 0.0019488216, 0.000391147, 0.0280000281, 0.0482778827, 0.0092678989, 0.0281487817, 0.0319969031, 0.0443814099, 0.0114612873, 0.049807454, 0.0222622763, 0.046407434, 0.024732158, 0.0062777817, 0.049116879, 0.0441464683, 0.0349707935, 0.0306262113, 0.0456331409, 0.0143367483, 0.0064469361, 0.0278795945, 0.0267307289, 0.045103594, 0.0296522133, 0.0444130089, 0.0204716053, 0.007940002, 0.0126883725, 0.026799297, 0.021382778, 0.0257969825, 0.0224727414, 0.0243460279, 6.529e-07, 0.038361756, 0.0493321691, 0.0071174903, 0.0196842502, 0.0386499095, 0.0217462171, 0.0192482868, 0.0449486363, 0.0043902238, 0.0032857979, 0.0216106306, 3.074e-07, 1.2084e-06, 0.0494736452, 0.0382442583, 0.013184068, 0.0249496256, 0.0021326643, 0.0198218289, 0.0036226989, 0.0327071162, 0.0042712333, 0.0187938069, 0.0490996574, 0.0032926536, 0.0172133696, 0.015105859, 0.0328668443, 0.0285386871, 0.0003268801, 0.0144776712, 0.0469025091, 0.0481503563, 0.0483244014, 0.0049642839, 0.029102447, 0.0223441628, 0.0301566928, 0.0217149937, 0.0041591962, 0.0334477928, 0.0155284452, 0.0284977972, 0.0017501542, 0.0204594741, 0.042497825, 0.0350713617, 0.0388572622, 0.0073797644, 0.002040861, 0.0274277808, 0.0334307559, 0.0085144097, 0.0003269897]
    for n in c.events:
        for e in n:
            assert not e.calibration or e.calibration


def test_triple_coincidence():
    er3 = czelta.event_reader('tests/testb.txt')
    er2 = czelta.event_reader('tests/testb.txt')
    er = czelta.event_reader('tests/test.dat')
    c = czelta.coincidence((er, er2, er3), 0.1, save_events=False)
    assert len(c) == 537
