from libcpp.string cimport string
from libcpp.vector cimport vector

cdef extern from "station.h" nogil:
    ctypedef struct TDCCorrection:
        int _from
        int* correction
    cppclass Station:
        Station() except +
        Station(int ID) except +
        void delet()
        inline int id()
        inline char* name()
        short* lastTDCCorrect()
        short* TDCCorrect(int timestamp)
        float* detectorPosition()
        double* GPSPosition()

        TDCCorrection* TDCCorrections()
        int TDCCorrections_size()

        double distanceTo(Station& st)

        void setName(char* name)
        void setGPSPosition(double latitude, double longitude, double height)
        void setDetectorPosition(float x1, float y1, float x2, float y2)
        void clearTDCCorrect()
        void pushTDCCorrect(int fr, short tdc0, short tdc1, short tdc2)
        void pushTDCCorrect(string fr, short tdc0, short tdc1, short tdc2)


ctypedef Station* p_Station
cdef extern from "station.h" namespace "Station" nogil:
    void addStation(Station)
    Station& getStation(int)
    Station& getStation(string)
    vector[p_Station] getStations()


cdef extern from "event_reader.h":
    struct Overlap:
        int measureTime
        int normal_events[2]
        int calibration_events[2]
    cppclass EventReader:
        EventReader() nogil except +
        inline int numberOfEvents() nogil
        int measurelength()
        bint loadDatFile(char* filename) nogil
        bint loadTxtFile(char* filename) nogil
        bint saveDatFile(char* filename) nogil
        bint saveTxtFile(char* filename, bint x_events) nogil
        inline Event& item(int index) nogil
        inline Event& item(int run, int index) nogil

        void setStation(int station) nogil
        inline int getStation() nogil

        int firstOlderThan(int timestamp) nogil
        int lastEarlierThan(int timestamp) nogil

        #filters
        inline int filter(bint (*func)(Event&))
        inline int filterCalibs() nogil
        inline int filterMaxTDC() nogil
        inline int filterMaxADC() nogil
        inline int filterMinADC() nogil

        inline int numberOfEvents(int run)
        inline int numberOfRuns()
        inline int runStartIndex(int i)
        inline int runStart(int i)
        inline int runEndIndex(int i)
        inline int runEnd(int i)
        int flux(int _from, int to)

cdef extern from "event_reader.h" namespace "EventReader" nogil:
    void setFilesDirectory(string dir)
    inline string getFilesDirectory()


cdef extern from "event.h" nogil:
    cppclass Event:
        Event() except +
        Event(int timestamp,double last_secod, int TDC0, int TDC1, int TDC2, int ADC0, int ADC1, int ADC2, int t0, int t1, int t2, int tCrateRaw, bint calibration, bint run) except +
        Event(const Event& orig)
        bint operator==(Event&)
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
        short* TDCCorrected()
        short TDC0Corrected()
        short TDC1Corrected()
        short TDC2Corrected()
        float* calculateDir()
        float* calculateDirRadians()
        inline float* calculateEarthDir()
        inline float* calculateEarthDirRadians()
        inline void setStation(int station)
        inline int getStation()
        inline Station& getRStation()

cdef extern from "coincidence.h" nogil:
    cppclass Coincidence:
        Coincidence() except +
        EventReader *readers[2]
        int n
        int stations[2]
        bint events_saved
        vector[double] delta
        vector[Event] *events
        vector[float] dirs
        double limit
        int numberOfCoincidences
        Overlap overlap
        double medium_value
        double chance

        void calc(double limit)
        void calc(double limit, bint save_events)

cdef extern from "common_func.h" nogil:
    enum:
        TIMESTAMP_ZERO_SIDEREAL
    double deltaDirection(double hor1, double az1, double hor2, double az2)
    int string_date(string date)
    int char_date(char*)
    int date(int year, int month, int day, int hour, int minute, int second)
    double lSideRealFromUnix(int unixSecs, float degres_longtitude)
    float* localToGlobalDirection(float* local_direction, double* gps_position, int time)
    float* localToAGlobalDirection(float* local_direction, double* gps_position)

cpdef date_to_timestamp(date)

cdef class station:
    cdef Station* st
    #property id
    #property name
    #property gps_position
    #property detector_position
    #get_corrections(self, timestamps=False)
    cpdef clear_corrections(self)
    #autofit_corrections(self, event_reader er, float confidence_level)
    #static clear_corrections
    cpdef distance_to(self, station other_station)
    #static load(file)
    #static save(file)
    #static get_stations()


cdef class event:
    cdef Event e
    cdef void set(self, Event e)
    #property timestamp
    #property datetime
    #property time_since_second
    #property TDC
    #property TDC_corrected
    #property ADC
    #property temps_detector
    #property temp_crate
    #property calibration
    #property AH_direction
    #property RAD_direction
    #property station
    cpdef set_station(self, int station_id)


cdef class coincidence:
    cdef Coincidence c
    cdef int i
    #def __init__(self, event_readers, max_difference, save_events, stations)
    #property stations
    #property delta
    #property events
    #property max_difference
    #property number_of_coincidences
    #property expected_value
    #property chance
    #property overlap_measeure_time (total measure time)


cdef class event_reader:
    cdef EventReader er
    cdef int i
    cpdef run(self, int run_id)
    cpdef runs(self)
    cpdef load(self, path_to_file)
    cpdef save(self, path_to_file, bint x_events = ?)
    cpdef int number_of_events(self, int run = ?)
    cpdef int number_of_runs(self)
    cpdef int measure_length(self)
    cdef Event c_item(self, int i)
    cpdef event item(self, int i)
    cpdef int measure_time(self)
    cpdef int flux(self, _from, to)

    cpdef station get_station(self)
    cpdef set_station(self, object st)

    #filters
    cpdef int filter(self, filter_func)
    cpdef int filter_calibrations(self)
    cpdef int filter_maximum_TDC(self)
    cpdef int filter_maximum_ADC(self)
    cpdef int filter_minimum_ADC(self)


#func wrapper for custom func
cdef event _filter_func_event
cdef object _filter_func_object
cdef bint _filter_func(Event& e)


cdef class event_reader_runs:
    cdef event_reader er
    cdef int i


cdef class event_reader_run:
    cdef event_reader er
    cdef int _run_id
    cdef int i
    cpdef int run_id(self)
    cpdef int begin_index(self)
    cpdef int end_index(self)
