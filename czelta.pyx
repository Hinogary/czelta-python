from libcpp.string cimport string
from libcpp.vector cimport vector

cdef extern from "station.h" nogil:
    cppclass Station:
        Station() except +
        Station(int ID) except +
        inline int id()
        inline char* name()
        short* lastTDCCorrect()
        short* TDCCorrect(int timestamp)
        double* detectorPosition()
        double* GPSPosition()
        
        double distanceTo(Station& st)
        
        void setName(char* name)
        void setGPSPosition(double latitude, double longitude, double height)
        void setDetectorPosition(double x1, double y1, double x2, double y2)
        void clearTDCCorrect(int capacity)
        void pushTDCCorrect(int fr, short tdc0, short tdc1, short tdc2)
        void pushTDCCorrect(string fr, short tdc0, short tdc1, short tdc2)
        void pushFileName(string name)
        
        
ctypedef Station* p_Station
cdef extern from "station.h" namespace "Station" nogil:
    bint addStation(Station)
    Station& getStation(int)
    Station& getStation(string)
    vector[p_Station] getStations()


cdef extern from "event_reader.h" nogil:
    cppclass EventReader:
        EventReader() except +
        setStation(int id)
        inline int numberOfEvents()
        bint loadDatFile(char* filename)
        bint loadTxtFile(char* filename)
        bint saveDatFile(char* filename)
        bint saveTxtFile(char* filename)
        inline Event& item(int index)
        inline Event& item(int run, int index)
        
        #filters
        int filterCalibs()
        inline int filterMaxTDC()
        inline int filterMaxADC()
        inline int filterMinADC()
        
        inline int numberOfRuns()
        inline int numberOfEvents(int run)
        
cdef extern from "event_reader.h" namespace "EventReader" nogil:
    static void setFilesDirectory(string dir)
    inline static string getFilesDirectory()

        
cdef extern from "event.h" nogil:
    cppclass Event:
        Event() except +
        Event(int timestamp,double last_secod, int TDC0, int TDC1, int TDC2, int ADC0, int ADC1, int ADC2, int t0, int t1, int t2, int tCrateRaw, bint calibration, bint run) except +
        Event(const Event& orig)
        inline int timestamp()
        inline double last_second()
        inline double time_since_second()
        inline long tenthOfNSTimestamp()
        inline short* TDC()
        inline short TDC0()
        inline short TDC1()
        inline short TDC2()
        inline short* ADC()
        inline short ADC0()
        inline short ADC1()
        inline short ADC2()
        inline short* tempsRaw()
        inline float* temps()
        inline short t0raw()
        inline short t1raw()
        inline short t2raw()
        inline float t0()
        inline float t1()
        inline float t2()
        inline short tCrateRaw()
        inline float tCrate()
        inline bint isCalib()
        string toString()



import datetime
import json

cdef class station:
    ""
    cdef Station* st
    def __init__(self, station):
        "Parameter can be `station name` or `station id`. Only look into list of loaded stations with :py:meth:`load`."
        if(type(station)==int):
            self.st = &getStation(<int>station)
        else:
            self.st = &getStation(<string>station)
        if sel.st.id()==0:
            raise RuntimeError("Station not exist, have you loaded config file?")
    cpdef id(self):
        "Return `station id`, probably same as on czelta website."
        return self.st.id()
    cpdef name(self):
        "Return code name of station. Example: ``\'praha_utef\'``, ``\'pardubice_gd\'`` or similar."
        return (<char*>self.st.name()).decode('utf-8')
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
        if file==None:
            file = open("config_data.JSON")
        cfg = json.load(file)
        file.close()
        for station in cfg['stations']:
            try:
                st = Station(int(station['ID']))
                st_name = station['name'].encode('utf-8')
                st.setName(st_name)
                if 'GPSposition' in station and len(station['GPSposition'])>2:
                    pos = station['GPSposition']
                    st.setGPSPosition(pos[0],pos[1],pos[2])
                if 'detectorsPos' in station and len(station['detectorsPos'])==4:
                    pos = station['detectorsPos']
                    st.setDetectorPosition(pos[0], pos[1], pos[2], pos[3])
                if 'file_names' in station:
                    for name in station['file_names']:
                        file_name = name.encode('utf-8')
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
                            from_cor = correction['from'].encode('utf-8')
                            st.pushTDCCorrect(<string>from_cor, <short>tdc[0], <short>tdc[1], <short>tdc[2])
                if addStation(st):
                    print "Station can't be added, already exist, id: "+str(st.id())+", name: "+st.name()
            except Warning:
                st_name = (<char*>st.name()).decode('utf-8')
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
    cdef Event e
    def __init__(self):
        pass
    def __str__(self):
        return self.e.toString().decode('utf-8')
    cdef void set(self, Event e):
        self.e = e
    cpdef timestamp(self):
        return self.e.timestamp()
    cpdef datetime(self):
        return datetime.datetime.utcfromtimestamp(self.e.timestamp())
    cpdef time_since_second(self):
        ""
        return self.e.time_since_second();
    cpdef TDC(self):
        ""
        cdef short* tdc = self.e.TDC()
        return (tdc[0], tdc[1], tdc[2])
    cpdef ADC(self):
        "Relative energy absorbed in each detector. Probably not comparable along different stations. Minimum value is 0 and Maximum is 2047. If it is 2047 it shloud be more."
        cdef short* adc = self.e.ADC()
        return (adc[0], adc[1], adc[2])
    cpdef temps(self):     
        "First three temps are temperature value from detectors, fourth is from crate. Temperature are in Celsius. Minimum step is 0.5."
        cdef float* temps = self.e.temps()
        return (temps[0], temps[1], temps[2], temps[3])
    cpdef temps_raw(self):
        "First three temps are temperature value from detectors, fourth is from crate. Temperature are have to be divided by 2 to get them in Celsius. Data type is int."
        cdef short* temps_raw = self.e.tempsRaw()
        return (temps_raw[0], temps_raw[1], temps_raw[2], temps_raw[3])
    cpdef calibration(self):
        "Calibration events are events actived by LED diod in each detectors."
        return self.e.isCalib()

cdef class event_reader:
    cdef EventReader er
    cdef int i
    def __init__(self, str path = ""):
        if(len(path)!=0):
            self.load(path)
    def __len__(self):
        return self.er.numberOfEvents()
    def __getitem__(self, i):
        cdef int ii, start, stop, step
        if type(i)==slice:
            start = 0 if i.start==None else i.start if i.start>=0 else self.er.numberOfEvents()+i.start
            stop = self.er.numberOfEvents() if i.stop==None else i.stop if i.stop>=0 else self.er.numberOfEvents()+i.stop
            step = i.step if i.step else 1
            if step <= 0:
                raise NotImplementedError
            es = []
            #currently not optimized to c loop (17.1.2014)
            #for ii in range(start, stop, step):
            #deprecated but optimized to c loop
            for ii from start <= ii < stop by step:
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
        return event_reader_run(self, run_id)
    cpdef runs(self):
        return event_reader_runs(self)
    cpdef load(self, str path):
        bytes_path = path.encode('utf-8')
        if path[-4:].lower()==".txt":
            if self.er.loadTxtFile(bytes_path):
                raise IOError("can't open or read file: "+path)
        elif path[-4:].lower()==".dat":
            if self.er.loadDatFile(bytes_path):
                raise IOError("can't open or read file: "+path)
        else:
            raise NotImplementedError("path must be a file with .txt or .dat")
    cpdef int number_of_events(self, int run = -1):
        if(run==-1):
            return self.er.numberOfEvents()
        else:
            return self.er.numberOfEvents(run)
    cpdef int number_of_runs(self):
        "Return number of runs. Same result have ``len(event_reader.runs())``.
        return self.er.numberOfRuns()
    cdef Event& c_item(int i):
        return self.er.item(i)
    cpdef event item(int i):
        event e()
        e.set(self.er.item(i))
        return e
    #filters
    cpdef int filter_calibrations(self):
        "filter all events marked as calibration"
        return self.er.filterCalibs()
    cpdef int filter_maximum_TDC():
        "Filter all events which have at least one TDC chanel equal maximum value (4095). Events with maximum value have bad measured TDC and sky direction can't determined righ."
        return self.er.filterMaxTDC()
    cpdef int filter_maximum_ADC():
        "Filter all events which have at least one ADC(energy) channel equal maximum value(2047)." 
        return self.er.filterMaxADC()
    cpdef int filter_minimum_ADC():
        "Filter all events which have at least one ADC(energy) channel equal zero (Not measured)."
        return self.er.filterMinADC()
cdef class event_reader_runs:
    cdef event_reader er
    cdef int i
    def __init__(self, event_reader reader):
        self.er = reader
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
    cdef event_reader er
    cdef int run_id
    cdef int i
    def __init__(self, event_reader reader, int run_id):
        self.er = reader
        self.run_id = run_id
    def __iter__(self):
        self.i = -1
        return self
    def __next__(self):
        self.i+=1
        if self.i < self.er.er.numberOfEvents(self.run_id):
            return self[self.i]
        else:
            raise StopIteration
    def __len__(self):
        return self.er.er.numberOfEvents(self.run_id)
    def __getitem__(self, i):
        cdef int ii, start, stop, step
        if type(i)==slice:
            start = 0 if not i.start else i.start if i.start>=0 else self.er.er.numberOfEvents(self.run_id)+i.start
            stop = self.er.er.numberOfEvents(self.run_id) if not i.stop else i.stop if i.stop>=0 else self.er.er.numberOfEvents(self.run_id)+i.stop
            step = i.step if i.step else 1
            if step <= 0:
                raise NotImplementedError
            es = []
            #currently not optimized to c loop (17.1.2014)
            #for ii in range(start, stop, step):
            #deprecated but optimized to c loop
            for ii from start <= ii < stop by step:
                e = event()
                e.set(self.er.er.item(self.run_id, ii))
                es.append(e)
            return es
        else:
            ii = i
            if ii<0:
                ii+=self.er.er.numberOfEvents(self.run_id)
            e = event()
            e.set(self.er.er.item(self.run_id, ii))
            return e
