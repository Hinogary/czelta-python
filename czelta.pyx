#author: Martin Quarda

import datetime
import json
import sys
import traceback

__version__ = '0.1'
__all__ = ['station','event','event_reader']
__author__ = 'Martin Quarda <hinogary@gmail.com>'
system_encoding = sys.getfilesystemencoding()



cdef class station:
    "Class for working with station data."
    def __init__(self, station):
        if type(station)==int:
            self.st = &getStation(<int>station)
        else:
            self.st = &getStation(<string>station.encode(system_encoding))
        if self.st.id()==0:
            raise RuntimeError("Station not exist, have you loaded config file?")
    cpdef int id(self):
        "Return `station id`, probably same as on czelta website."
        return self.st.id()
    cpdef name(self):
        "Return code name of station. Example: ``\'praha_utef\'``, ``\'pardubice_gd\'`` or similar."
        return (<char*>self.st.name()).decode(system_encoding)
    cpdef detector_position(self):
        "Return position of detectors in format ``(x1, y1, x2, y2)`` where ``x1`` and ``y1`` are relative position of detector 1 to detector 0. ``x2`` and ``y2`` are relative position of detector 2 to detector 0. All values are in metres."
        cdef double* dp = self.st.detectorPosition()
        return (dp[0], dp[1], dp[2], dp[3])
    cpdef distance_to(self, station other_station):
        "Calculate distance to other station using haversine method. The return number is in kilometres."
        cdef double distance = self.st.distanceTo(other_station.st[0])
        if distance==0:
            return None
        else:
            return distance
    cpdef gps_position(self):
        "Returns GPS position of station. Return ``(latitude, longitude)`` or ``None`` if gps_position is not defined."
        cdef double* gp = self.st.GPSPosition()
        if gp[0]==0 and gp[1]==0:
            return None
        return (gp[0], gp[1], gp[2])
    @staticmethod
    def load(file = None):
        "Loads configuration file of stations. Must be called if you want to work with ``czelta.station``."
        cdef Station st
        cdef object pos
        if file==None:
            file = open("config_data.JSON")
        cfg = json.load(file)
        file.close()
        for station in cfg['stations']:
            try:
                st = Station(int(station['ID']))
                st_name = station['name'].encode(system_encoding)
                st.setName(st_name)
                if 'GPSposition' in station and len(station['GPSposition'])>=2:
                    pos = station['GPSposition']
                    st.setGPSPosition(pos[0],pos[1],pos[2] if len(pos)>2 else 0)
                if 'detectorsPos' in station and len(station['detectorsPos'])==4:
                    pos = station['detectorsPos']
                    st.setDetectorPosition(pos[0], pos[1], pos[2], pos[3])
                if 'file_names' in station:
                    for name in station['file_names']:
                        file_name = name.encode(system_encoding)
                        st.pushFileName(file_name)
                if 'TDCCorrection' in station and len(station['TDCCorrection'])>0:
                    st.clearTDCCorrect(len(station['TDCCorrection']))
                    for correction in station['TDCCorrection']:
                        tdc = correction['correction']
                        if not 'from' in correction:
                            correction['from'] = 0
                        if type(correction['from'])==int:
                            st.pushTDCCorrect(<int>correction['from'], <short>tdc[0], <short>tdc[1], <short>tdc[2])
                        else:
                            from_cor = correction['from'].encode(system_encoding)
                            st.pushTDCCorrect(<string>from_cor, <short>tdc[0], <short>tdc[1], <short>tdc[2])
                if addStation(st):
                    print "Station can't be added, already exist, id: "+str(st.id())+", name: "+st.name()
            except Warning:
                st_name = (<char*>st.name()).decode(system_encoding)
                print "Station can't be added, bad format of JSON, id: "+str(st.id())+", name: "+st_name
                
    @staticmethod
    def get_stations():
        "Return list of all available stations."
        cdef vector[p_Station] stations = getStations()
        rtn = []
        for st in stations:
            rtn.append(station(st.id()))
        return rtn



cdef class event:
    "Basic czelta class for holding information about events. This time is imposible to create own event"
    def __init__(self):
        pass
    def __str__(self):
        return self.e.toString().decode(system_encoding)
    cdef void set(self, Event e):
        self.e = e
    property timestamp:
        "timestamp of event, fastest way to get time of event."
        def __get__(self):
            return self.e.timestamp()
    property datetime:
        "Return python `datetime <http://docs.python.org/2/library/datetime.html>`_ object. All times with Czelta is in UTC."
        def __get__(self):
            return datetime.datetime.utcfromtimestamp(self.e.timestamp())
    property time_since_second:
        "Return time elapsed since last second (0-0.999999... sec)."
        def __get__(self):
            return self.e.time_since_second();
    property ADC:
        "Relative energy absorbed in each detector. Probably not comparable along different stations. Minimum value is 0 and Maximum is 2047. If it is 2047 it shloud be more."
        def __get__(self):
            cdef short* adc = self.e.ADC()
            return (adc[0], adc[1], adc[2])
    property TDC:
        "Relative time of activation each detector. TDC*25/1e12 = sec. Format: ``(TDC0, TDC1, TDC2)``."
        def __get__(self):
            cdef short* tdc = self.e.TDC()
            return (tdc[0], tdc[1], tdc[2])
    property TDC_corrected:
        "Relative time of activation each detector. Corrected and can be used to calculate diraction. Correction options are in `config_data.JSON`. TDC*25/1e12 = sec."
        def __get__(self):
            cdef short* tdc = self.e.TDCCorrected()
            return (tdc[0], tdc[1], tdc[2])
    property temps_detector:
        "Return 3 temps of each detector in time of event."
        def __get__(self):
            cdef float* temps = self.e.temps()
            return (temps[0], temps[1], temps[2])
    property temp_crate:
        "Return Temperature in crate in time of event."
        def __get__(self):
            return self.e.tCrate()
    property calibration:
        "Calibration events are events actived by LED diod in each detectors."
        def __get__(self):
            return self.e.isCalib()
    property HA_direction:
        "Return ``(horizon, azimuth)`` direction of shower. Azimuth is from south clockwise. Both values are in Degres. Must have loaded info about stations and set station for ``event``/``event_reader``"
        def __get__(self):
            cdef float *HA = self.e.calculateDir()
            if HA[0]==0 and HA[1]==0:
                return None
            else:
                return (HA[0],HA[1])
    cpdef set_station(self, station_id):
        "Set station to correct tdc and calculate direction, it is better to change station of entire ``czelta.event_reader``."
        self.e.setStation(<int>station_id)



cdef class event_reader:
    """
    Object containing events loaded from file. Have defined len method returning number of events in event_reader.
    
    It is iterable::
        number_of_events = len(some_event_reader)
        
        for event in some_event_reader:
            # do something with event
            # for example print event in format same as is in txt.
            print(str(event))
    
    Getting invidual events::
    
        some_event = some_event_reader[7]
    
    Getting slice of events::
        
        #standart index
        some_events = some_event_reader[7:12]
        
        #getting slice by datetime object
        from datetime import datetime
        some_events = some_event_reader[datetime(2013,9,1), datetime(2013,9,30)]
        
    """
    def __init__(self, str path = ""):
        if(len(path)!=0):
            self.load(path)
    def __len__(self):
        return self.er.numberOfEvents()
    def __getitem__(self, i):
        cdef int ii, start, stop
        if type(i)==slice:
            if i.start==None:
                start = 0
            elif type(i.start)==datetime.datetime:
                d = i.start
                start = date(d.year, d.month, d.day, d.hour, d.minute, d.second) 
                start = self.er.firstOlderThan(start)
            else:
                start = i.start
                start = start if start>=0 else self.er.numberOfEvents()+start
            
            if i.stop==None:
                stop = self.er.numberOfEvents()
            elif type(i.stop)==datetime.datetime:
                d = i.stop
                stop = date(d.year, d.month, d.day, d.hour, d.minute, d.second) 
                stop = self.er.firstOlderThan(stop)
            else:
                stop = i.stop
                stop = stop if stop>=0 else self.er.numberOfEvents()+stop
                
            if i.step != None:
                raise NotImplementedError("step can't be defined")
                
            es = []
            for ii in range(start, stop):
                e = event()
                e.set(self.er.item(ii))
                es.append(e)
            return es
        else:
            ii = i
            if ii<0:
                ii += self.er.numberOfEvents()
            e = event()
            e.set(self.er.item(ii))
            return e
    def __iter__(self):
        self.i = -1
        return self
    def __next__(self):
        self.i+=1
        if self.i < self.er.numberOfEvents():
            return self[self.i]
        else:
            raise StopIteration
    cpdef run(self, int run_id):
        "Return iterable object containing all events in run"
        return event_reader_run(self, run_id)
    cpdef runs(self):
        "Return iterable object containing all runs."
        return event_reader_runs(self)
    cpdef load(self, path_to_file):
        "Load events from file. This delete all current events and tries to load events from file"
        cdef bytes bytes_path = path_to_file.encode(system_encoding)
        if bytes_path[-4:].lower()==b".txt":
            if self.er.loadTxtFile(bytes_path):
                raise IOError("can't open or read file: "+path_to_file)
        elif bytes_path[-4:].lower()==b".dat":
            if self.er.loadDatFile(bytes_path):
                raise IOError("can't open or read file: "+path_to_file)
        else:
            raise NotImplementedError("path must be a file with .txt or .dat")
    cpdef save(self, path_to_file):
        "Save events to file. Can save txt or dat(binary files same as from website)."
        bytes_path = path_to_file.encode(system_encoding)
        if bytes_path[-4:].lower()==b".txt":
            if self.er.saveTxtFile(bytes_path):
                raise IOError("can't write file: "+path_to_file)
        elif bytes_path[-4:].lower()==b".dat":
            if self.er.saveDatFile(bytes_path):
                raise IOError("can't write file: "+path_to_file)
        else:
            raise NotImplementedError("path must be a file with .txt or .dat")
    cpdef set_station(self, object st):
        "Set station for event_reader. Station is also set for all current events."
        cdef int _id
        if type(st)==int:
            _id = st
        elif type(st)==str:
            _id = station(st).id()
        elif type(st)==station:
            _id = st.id()
        else:
            raise ValueError("Unknown type of station")
        self.er.setStation(_id)
    cpdef int number_of_events(self, int run = -1):
        "Return number of events in ``event_reader``. Same result have ``len(event_reader)``."
        if(run==-1):
            return self.er.numberOfEvents()
        else:
            return self.er.numberOfEvents(run)
    cpdef int number_of_runs(self):
        "Return number of runs. Same result have ``len(event_reader.runs())``."
        return self.er.numberOfRuns()
    cdef Event c_item(self, int i):
        return self.er.item(i)
    cpdef event item(self, int i):
        e = event()
        e.set(self.er.item(i))
        return e
    #filters
    cpdef int filter(self, filter_func):
        "Custom-filter function. As parameter give a function, which is ready to be called with parameter event object, which return True if you want remove event and False if you want let event in event_reader."
        global _filter_func_event
        global _filter_func_object
        
        #test
        try:
            e = event()
            e.set(self.er.item(0))
            filter_func(e)
        except TypeError as te:
            raise TypeError("function must have one parameter (czelta.event)")
        except AttributeError as ae:
            raise ae
        #real filtering
        _filter_func_object = filter_func
        _filter_func_event = event()
        return self.er.filter(&_filter_func)
    cpdef int filter_calibrations(self):
        "Predefined fast filter. Filter all events marked as calibration."
        return self.er.filterCalibs()
    cpdef int filter_maximum_TDC(self):
        "Predefined fast filter. Filter all events which have at least one TDC chanel equal maximum value (4095). Events with maximum value have bad measured TDC and sky direction can't be determined right."
        return self.er.filterMaxTDC()
    cpdef int filter_maximum_ADC(self):
        "Predefined fast filter. Filter all events which have at least one ADC(energy) channel equal maximum value(2047)." 
        return self.er.filterMaxADC()
    cpdef int filter_minimum_ADC(self):
        "Predefined fast filter. Filter all events which have at least one ADC(energy) channel equal zero (Not measured)."
        return self.er.filterMinADC()
#filter_func wrapper
cdef bint _filter_func(Event& e):
    global _filter_func_event
    global _filter_func_object
    try:
        _filter_func_event.set(e)
        return _filter_func_object(_filter_func_event)
    except:
        traceback.print_exc()
        print "Error in filter func"
        return False

cdef class event_reader_runs:
    "Iteratable class for runs of ``czelta.event_reader``."
    def __init__(self, event_reader reader):
        self.er = reader
    def __str__(self):
        return "<czelta.event_reader_runs object with %i runs from event_reader: %s>"%(len(self),str(self.er))
    def __iter__(self):
        self.i = -1
        return self
    def __next__(self):
        self.i+=1
        if self.i < self.er.er.numberOfRuns():
            return self[self.i]
        else:
            raise StopIteration
    def __len__(self):
        return self.er.er.numberOfRuns()
    def __getitem__(self, i):
        cdef int ii, start, stop, step
        if type(i)==slice:
            start = 0 if not i.start else i.start if i.start>=0 else self.er.er.numberOfRuns()+i.start
            stop = self.er.er.numberOfRuns() if not i.stop else i.stop if i.stop>=0 else self.er.er.numberOfRuns()+i.stop
            step = i.step if i.step else 1
            if step <= 0:
                raise NotImplementedError
            runs = []
            #currently not optimized to c loop (17.1.2014)
            #for ii in range(start, stop, step):
            #deprecated but optimized to c loop
            for ii from start <= ii < stop by step:
                runs.append(event_reader_run(self.er, ii))
            return runs
        else:
            ii = i
            if ii<0:
                ii += self.er.er.numberOfRuns()
            return event_reader_run(self.er, ii)



cdef class event_reader_run:
    def __init__(self, event_reader reader, int run_id):
        self.er = reader
        self._run_id = run_id
    def __iter__(self):
        self.i = -1
        return self
    def __next__(self):
        self.i+=1
        if self.i < self.er.er.numberOfEvents(self._run_id):
            return self[self.i]
        else:
            raise StopIteration
    def __len__(self):
        return self.er.er.numberOfEvents(self._run_id)
    def __getitem__(self, i):
        cdef int ii, start, stop, step
        if type(i)==slice:
            start = 0 if not i.start else i.start if i.start>=0 else self.er.er.numberOfEvents(self._run_id)+i.start
            stop = self.er.er.numberOfEvents(self._run_id) if not i.stop else i.stop if i.stop>=0 else self.er.er.numberOfEvents(self._run_id)+i.stop
            step = i.step if i.step else 1
            if step <= 0:
                raise NotImplementedError
            es = []
            #currently not optimized to c loop (17.1.2014)
            #for ii in range(start, stop, step):
            #deprecated but optimized to c loop
            for ii from start <= ii < stop by step:
                e = event()
                e.set(self.er.er.item(self._run_id, ii))
                es.append(e)
            return es
        else:
            ii = i
            if ii<0:
                ii+=self.er.er.numberOfEvents(self._run_id)
            e = event()
            e.set(self.er.er.item(self._run_id, ii))
            return e
    cpdef int run_id(self):
        return self._run_id
    
    cpdef int begin_index(self):
        return self.er.er.runStartIndex(self._run_id)
    cpdef int end_index(self):
        return self.er.er.runEndIndex(self._run_id)
