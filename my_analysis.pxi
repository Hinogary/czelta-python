cdef double delta_dir(float az1, float hor1, float az2, float hor2):
    cdef double sum = m.sin(hor1)*m.sin(hor2) + m.cos(hor1)*m.cos(hor2)*m.cos(az1-az2)
    return m.acos(sum)
    
import numpy as np
cimport numpy as np
cimport libc.math as m
import pylab
import sys
from cython.parallel import parallel, prange

def mapa_smeru(double citlivost = 5, path = '~/data/pardubice_spse.dat', int scale = 1, filter = None, bint flux_cor = True):
    scale*=2
    if scale == 0:
        scale = 1
    citlivost*=m.M_PI/180.0
    
    #filtering data
    cdef event_reader er = event_reader(path)
    def f(e):
        dir = e.AH_direction
        tdc = e.TDC
        return not dir or e.AH_direction[1] < 40
    er.filter(f)
    er.filter_calibrations()
    er.filter_maximum_TDC()
    
    if filter:
        er.filter(filter)
    if len(er)==0:
        return None
    
    cdef int measure_time = er.measure_time()
    cdef np.ndarray[np.double_t, ndim=2] data = np.zeros((360/scale+1,720/scale), dtype=np.double)

    cdef vector[float] RA
    cdef vector[float] D
    cdef vector[int] times
    
    cdef float* dir
    cdef event e
    for e in er:
        dir = e.e.calculateEarthDirRadians()
        if dir:
            RA.push_back(dir[0])
            D.push_back(dir[1])
            times.push_back(e.timestamp)
    
    cdef int length = RA.size()
    cdef vector[float] deltas
    deltas.resize(length)
    cdef double maxdelta = 1
    cdef int xx, yy, i
    cdef float x,y
    print()
    for yy in range(360/scale+1):
        y = (scale*yy-180)/360.0*m.M_PI
        maxdelta = scale/360.0*m.M_PI
        for i in range(length):
            if deltas[i]<citlivost and deltas[i]+maxdelta<citlivost and yy!=0:
                deltas[i]+=maxdelta
            elif deltas[i]>citlivost and deltas[i]-maxdelta>citlivost and yy!=0:
                deltas[i]-=maxdelta
            else:
                deltas[i] = delta_dir(0, y, RA[i], D[i])
        maxdelta = delta_dir(0,y,scale/360.0*m.M_PI,y)
        #remove last progress and replace with new
        sys.stdout.write("\033[F")
        sys.stdout.write("\033[K")
        print("%i/%i"%(yy+1,360/scale+1))
        for xx in range(720/scale):
            x = scale*xx/360.0*m.M_PI
            for i in range(length):
                if deltas[i]<citlivost and deltas[i]+maxdelta<citlivost:
                    deltas[i]+=maxdelta
                elif deltas[i]>citlivost and deltas[i]-maxdelta>citlivost:
                    deltas[i]-=maxdelta
                else:
                    deltas[i] = delta_dir(x, y, RA[i], D[i])
                if deltas[i]<citlivost:
                    data[yy, xx]+=1
    
    #calc of "source" array for calculating expected values
    cdef np.ndarray[np.double_t, ndim=2] source = np.zeros((360/scale+1,720/scale), dtype=np.double)
    
    cdef object pos = list(er.get_station().gps_position())
    
    RA.clear()
    D.clear()
    for e in er:
        dir = e.e.calculateDirRadians()
        if dir:
            dir = localToAGlobalDirection(dir, e.e.getRStation().GPSPosition())
            RA.push_back(dir[0])
            D.push_back(dir[1])
    print()
    for yy in range(360/scale+1):
        y = (scale*yy-180)/360.0*m.M_PI
        maxdelta = scale/360.0*m.M_PI
        for i in range(length):
            if deltas[i]<citlivost and deltas[i]+maxdelta<citlivost and yy!=0:
                deltas[i]+=maxdelta
            elif deltas[i]>citlivost and deltas[i]-maxdelta>citlivost and yy!=0:
                deltas[i]-=maxdelta
            else:
                deltas[i] = delta_dir(0, y, RA[i], D[i])
        maxdelta = delta_dir(0,y,scale/360.0*m.M_PI,y)
        sys.stdout.write("\033[F")
        sys.stdout.write("\033[K")
        print("%i/%i"%(yy+1,360/scale+1)) #printing progress
        for xx in range(720/scale):
            x = scale*xx/360.0*m.M_PI
            for i in range(length):
                if deltas[i]<citlivost and deltas[i]+maxdelta<citlivost:
                    deltas[i]+=maxdelta
                elif deltas[i]>citlivost and deltas[i]-maxdelta>citlivost:
                    deltas[i]-=maxdelta
                else:
                    deltas[i] = delta_dir(x, y, RA[i], D[i])
                if deltas[i]<citlivost:
                    source[yy, xx]+=1
    
    
    #for each second of SIDEREAL time is calculated how many time it is (for longtitude 0)
    cdef vector[int] measuredt
    measuredt.resize(86400)
    
    cdef int t
    if flux_cor:
        for t in times:
            t = 86400-int(lSideRealFromUnix(t,0)/2/m.M_PI*86400)
            measuredt[t] += 1 
    else:
        for run in er.runs():
            begin = run[0].timestamp
            end = run[-1].timestamp
            for t in range(begin, end):
                t = 86400-int(lSideRealFromUnix(t,0)/2/m.M_PI*86400)
                measuredt[t] += 1
            
    #converting measuredt array to multiply array, which parse data into final expecting values
    cdef vector[double] multiplyarray
    multiplyarray.resize(720/scale)
    
    cdef double ratio
    for i in range(86400):
        ratio = i%(86400/720*scale)/(86400.0/720*scale)
        xx = i*720/scale/86400+1
        xx = xx if xx!=360 else 0
        if flux_cor:
            multiplyarray[i*720/scale/86400]+=(1-ratio)*measuredt[i]/len(er)
            multiplyarray[xx]+=ratio*measuredt[i]/len(er)
        else:
            multiplyarray[i*720/scale/86400]+=(1-ratio)*measuredt[i]/measure_time
            multiplyarray[xx]+=ratio*measuredt[i]/measure_time
            
        
    #apply multiplyarray into expected
    cdef np.ndarray[np.double_t, ndim=2] expected = np.zeros((360/scale+1,720/scale), dtype=np.double)
    cdef np.ndarray[np.double_t, ndim=2] expected_error = np.zeros((360/scale+1,720/scale), dtype=np.double)
    
    for i in range(720/scale):
        for xx in range(720/scale):
            for yy in range(360/scale+1):
                expected[yy,xx] += multiplyarray[i] * source[yy,xx+i if xx+i<720/scale else xx+i-720/scale]
                expected_error[yy,xx] += multiplyarray[i] * m.sqrt(source[yy,xx+i if xx+i<720/scale else xx+i-720/scale])
                
    #result array is data / expected values
    cdef np.ndarray[np.double_t, ndim=2] result = np.zeros((360/scale+1,720/scale), dtype=np.double)
    for xx in range(720/scale):
        for yy in range(360/scale+1):
            if expected[yy,xx]!=0:
                result[yy, xx] = data[yy, xx]/expected[yy, xx]
    
    #draw plot
    extent = [0, 360, -90, 90]
    cdef object y2 = expected * expected
    cdef object x2 = data*data
    cdef object y4 = y2*y2
    cdef object dy2 = expected_error
    cdef object data_error = np.sqrt(data)
    return {
      "data": data,
      "data_error": data_error,
      "source": source,
      "expected": expected,
      "expected_error": expected_error,
      "result": result,
      "result_error": data_error*np.sqrt((1+data/y2*dy2)/y2),
      "result_sigma": (result-1)/(data_error*np.sqrt((1+data/y2*dy2)/y2)),
      "extent": extent
    }
    
    #example of show image
    pylab.imshow(source,extent=extent,interpolation='nearest',origin='lower')
    pylab.colorbar()
    pylab.show()

def moon_check(double citlivost = 5, path = '~/data/pardubice_spse.dat', filter = None):
    cdef event_reader er = event_reader(path)
    def f(e):
        dir = e.AH_direction
        #return e.datetime.year<2014 or not dir or dir[1] < 40
        return not dir or dir[1] < 40
    er.filter(f)
    er.filter_calibrations()
    er.filter_maximum_TDC()
    if filter:
        er.filter(filter)
    
    cdef vector[double] RA
    cdef vector[double] D
    cdef vector[double] relative_alt
    cdef vector[double] relative_az
    cdef double JD
    cdef ln_equ_posn moon, zenit
    cdef ln_equ_posn e_dir
    cdef ln_hrz_posn ob
    
    cdef ln_lnlat_posn station
    station.lng = er.get_station().gps_position()[0]
    station.lat = er.get_station().gps_position()[1]
    
    cdef vector[int] timestamps
    cdef event e
    cdef object dir
    cdef long int i
    cdef int j, k = 0, x, y, g
    cdef int measure_time = er.measure_time()
    for e in er:
        dir = e.RAD_direction
        D.push_back(dir[1])
        RA.push_back(dir[0])
        timestamps.push_back(e.timestamp)
    
    cdef np.ndarray[np.int_t, ndim=2] data = np.zeros((181,181), dtype=np.int)
    cdef np.ndarray[np.double_t, ndim=2] source = np.zeros((181,181), dtype=np.double)
    cdef np.ndarray[np.double_t, ndim=2] expected = np.zeros((181,181), dtype=np.double)
    
    for e in er:
        dir = e.AH_direction
        x = int(90+(90-dir[1])*m.sin(dir[0]/180*m.M_PI))
        y = int(90+(90-dir[1])*m.cos(dir[0]/180*m.M_PI))
        if x>=0 and x<181 and y>=0 and y<181:
            source[y, x]+=1
    
    for run in er.runs():
        for i in range(run[0].timestamp, run[-1].timestamp, 60):
            JD = ln_get_julian_from_timet(&i)
            ln_get_lunar_equ_coords_prec(JD , &moon, 0.01)
            ln_get_hrz_from_equ(&moon, &station, JD, &ob)
            x = int(90+(90-ob.alt)*m.sin(ob.az/180*m.M_PI))
            y = int(90+(90-ob.alt)*m.cos(ob.az/180*m.M_PI))
            #for j in range(181):
            #    for g in range(181):
            #        if j+y >= 0 and j+y < 181 and g+x >= 0 and g+x < 181:
            #            expected[j+y, g+x] += source[j, g]/measure_time*60
                        
            ln_get_equ_from_hrz(&ob, &station, JD, &zenit)
            if ln_get_angular_separation(&zenit, &moon)>90:
                continue
            
            while timestamps[k]<i+60:
                e_dir.ra = RA[k]/m.M_PI*180
                e_dir.dec = D[k]/m.M_PI*180
                ob.alt = 90-ln_get_angular_separation(&moon, &e_dir)
                ob.az = ln_get_rel_posn_angle(&moon, &e_dir)
                #ln_get_hrz_from_equ(&e_dir, <ln_lnlat_posn*>&moon, JD, &ob)
                if  ob.alt <= 0:
                    k+=1
                    continue
                x = int(90+(90-ob.alt)*m.sin(ob.az/180*m.M_PI))
                y = int(90+(90-ob.alt)*m.cos(ob.az/180*m.M_PI))
                if x>=0 and x<181 and y>=0 and y<181:
                    data[y, x]+=1
                    relative_alt.push_back(ob.alt/180*m.M_PI)
                    relative_az.push_back(ob.az/180*m.M_PI)
                k+=1
            #some expected code
            
            
            
    for i in range(relative_az.size()):
        #calc where should be moon
        #calc horizon
        #calc delta
        pass

    return {
        "data":data,
        "data_error":np.sqrt(data),
        "source":source,
        "source_error":np.sqrt(source),
        "expected":expected
    }

from time import time
def test():
    cdef ln_equ_posn posn1
    cdef ln_equ_posn posn2
    posn2.ra = 1
    posn2.dec = 2
    print(ln_get_angular_separation(&posn1, &posn2))
    print(ln_get_rel_posn_angle(&posn1, &posn2))
    #for i in range()
