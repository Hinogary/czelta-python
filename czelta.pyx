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
        int filterCalibs()
        
        inline int numberOfRuns()
        inline int numberOfEvents(int run)
 

        
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
    cdef Station* st
    def __init__(self, id):
        if(type(id)==int):
            self.st = &getStation(<int>id)
        else:
            self.st = &getStation(<string>id)
    cpdef id(self):
        return self.st.id()
    cpdef exist(self):
        return self.st.id()!=0;
    cpdef name(self):
        return self.st.name()
    cpdef detector_position(self):
        cdef double* dp = self.st.detectorPosition()
        return (dp[0], dp[1], dp[2], dp[3])
    cpdef distance_to(self, station st):
        "Calculate distance to other station using haversine method. The return number is in kilometres."
        return self.st.distanceTo(st.st[0])
    cpdef gps_position(self):
        cdef double* gp = self.st.GPSPosition()
        return (gp[0], gp[1], gp[2])
    @staticmethod
    def load(file = None):
        cdef Station st
        if file==None:
            file = open("config_data.JSON")
        cfg = json.load(file)
        file.close()
        for station in cfg['stations']:
            try:
                st = Station(int(station['ID']))
                st.setName(station['name'])
                if 'GPSposition' in station and len(station['GPSposition'])>2:
                    pos = station['GPSposition']
                    st.setGPSPosition(pos[0],pos[1],pos[2])
                if 'detectorsPos' in station and len(station['detectorsPos'])==4:
                    pos = station['detectorsPos']
                    st.setDetectorPosition(pos[0], pos[1], pos[2], pos[3])
                if 'file_names' in station:
                    for name in station['file_names']:
                        st.pushFileName(name)
                if 'TDCCorrection' in station and len(station['TDCCorrection'])>0:
                    st.clearTDCCorrect(len(station['TDCCorrection']))
                    for correction in station['TDCCorrection']:
                        tdc = correction['correction']
                        if not 'from' in correction:
                            correction['from'] = 0
                        if type(correction['from'])==int:
                            st.pushTDCCorrect(<int>correction['from'], <short>tdc[0], <short>tdc[1], <short>tdc[2])
                        else:
                            st.pushTDCCorrect(<string>correction['from'], <short>tdc[0], <short>tdc[1], <short>tdc[2])
                if addStation(st):
                    print "Station can't be added, already exist, id: "+str(st.id())+", name: "+st.name()
            except:
                print "Station can't be added, bad format of JSON, id: "+str(st.id())+", name: "+st.name()
                
    @staticmethod
    def get_stations():
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
        return self.e.toString()
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
    def __init__(self, bytes path = b""):
        if(len(path)!=0):
            self.load(path)
    def __len__(self):
        return self.er.numberOfEvents()
    def __getitem__(self, int i):
        return self.item(i)
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
    cpdef load(self, bytes path):
        if(path[-4:].lower()==".txt"):
            if(self.er.loadTxtFile(path)):
                raise IOError("can't open or read file: "+path)
        elif(path[-4:].lower()==".dat"):
            if(self.er.loadDatFile(path)):
                raise IOError("can't open or read file: "+path)
        else:
            raise NotImplementedError("path must be a file with .txt or .dat")
    cpdef int number_of_events(self, int run = -1):
        if(run==-1):
            return self.er.numberOfEvents()
        else:
            return self.er.numberOfEvents(run)
    cpdef int number_of_runs(self):
        return self.er.numberOfRuns()
    cpdef item(self, int index):
        e = event()
        e.set(self.er.item(index))
        return e
    cpdef filter_calibrations(self):
        return self.er.filterCalibs()
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
    def __getitem__(self, int i):
        return event_reader_run(self.er, i)
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
    def __getitem__(self, int i):
        e = event()
        e.set(self.er.er.item(self.run_id, i))
        return e
