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
        
        void setName(char* name)
cdef extern from "station.h" namespace "Station" nogil:
    bint addStation(Station)
    Station& getStation(int)
    Station& getStation(string)
    Station* getStations()
        



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
        #string toString() const;



import datetime
import json

cdef class station:
    cdef Station* st
    def __init__(self, int id):
        self.st = &getStation(id)
    cpdef id(self):
        return self.st.id()
    cpdef exist(self):
        return self.id()!=0;
    cpdef name(self):
        return self.st.name()
    @staticmethod
    def load(file = None):
        cdef Station st
        if(file==None):
            file = open("config_data.JSON")
        cfg = json.load(file)
        file.close()
        for station in cfg['stations']:
            st = Station(int(station['ID']))
            st.setName(station['name'])
            if(not addStation(st)):
                print "Station can't be added, id: "+str(st.id())+", name: "+st.name()

cdef class event:
    cdef Event e
    def __init__(self):
        pass
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
    def __init__(self, bytes path = b""):
        if(len(path)!=0):
            self.load(path)
    def __len__(self):
        return self.er.numberOfEvents()
    def __getitem__(self, int i):
        return self.item(i)
    cpdef load(self, bytes path):
        if(path[-4:].lower()==".txt"):
            if(self.er.loadTxtFile(path)):
                raise IOError("can't open or read file: "+path)
        elif(path[-4:].lower()==".dat"):
            if(self.er.loadDatFile(path)):
                raise IOError("can't open or read file: "+path)
        else:
            raise NotImplementedError("path must be a file with .txt or .dat")
    cpdef int number_of_events(self):
        return self.er.numberOfEvents()
    cpdef item(self, int index):
        e = event()
        e.set(self.er.item(index))
        return e
    cpdef filter_calibrations(self):
        return self.er.filterCalibs()
        
