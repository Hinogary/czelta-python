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
    void setFilesDirectory(string dir)
    inline string getFilesDirectory()

        
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



cdef class station:
    cdef Station* st
    cpdef int id(self)
    cpdef name(self)
    cpdef distance_to(self, station other_station)
    cpdef gps_position(self)
    cpdef detector_position(self)
        

cdef class event:
    cdef Event e
    cdef void set(self, Event e)
    cpdef timestamp(self)
    cpdef datetime(self)
    cpdef time_since_second(self)
    cpdef TDC(self)
    cpdef ADC(self)
    cpdef temps(self)
    cpdef temps_raw(self)
    cpdef calibration(self)

cdef class event_reader:
    cdef EventReader er
    cdef int i
    cpdef run(self, int run_id)
    cpdef runs(self)
    cpdef load(self, str path)
    cpdef int number_of_events(self, int run = ?)
    cpdef int number_of_runs(self)
    cdef Event c_item(self, int i)
    cpdef event item(self, int i)
    
    #filters
    cpdef int filter_calibrations(self)
    cpdef int filter_maximum_TDC(self)
    cpdef int filter_maximum_ADC(self)
    cpdef int filter_minimum_ADC(self)


cdef class event_reader_runs:
    cdef event_reader er
    cdef int i
    
    
cdef class event_reader_run:
    cdef event_reader er
    cdef int _run_id
    cdef int i
    cpdef int run_id(self)
