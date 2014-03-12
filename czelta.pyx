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
        
cdef extern from "event.h" nogil:
    cppclass Event:
        Event() except +
        Event(int timestamp,double last_secod, int TDC0, int TDC1, int TDC2, int ADC0, int ADC1, int ADC2, int t0, int t1, int t2, int tCrateRaw, int byte) except +
        Event(const Event& orig)
        inline int timestamp()
        inline double last_second()
        inline long tenthOfNSTimestamp()
        inline short* TDC()
        #inline short* correctedTDC(int st)
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
        inline bint isRun()


import datetime

cdef class event:
    cdef Event e
    def __init__(self):
        pass
    cdef void set(self, Event e):
        self.e = e
    cpdef int timestamp(self):
        return self.e.timestamp()
    cpdef datetime(self):
        return datetime.datetime.utcfromtimestamp(self.e.timestamp())
    cpdef TDC(self):
        cdef short* tdc = self.e.TDC()
        return (tdc[0], tdc[1], tdc[2])
    cpdef ADC(self):
        cdef short* adc = self.e.ADC()
        return (adc[0], adc[1], adc[2])
    cpdef temps(self):
        cdef float* temps = self.e.temps()
        return (temps[0], temps[1], temps[2], temps[3])
    cpdef temps_raw(self):
        cdef short* temps_raw = self.e.tempsRaw()
        return (temps_raw[0], temps_raw[1], temps_raw[2], temps_raw[3])
    cpdef calib(self):
        return self.e.isCalib()
    cpdef run(self):
        return self.e.isRun()

cdef class event_reader:
    cdef EventReader er
    def __init__(self):
        pass
    cpdef load(self, bytes path):
        if(path[-4:].lower()==".txt"):
            if(self.er.loadTxtFile(path)):
                raise IOError("can't open or read file: "+path)
        elif(path[-4:].lower()==".dat"):
            if(self.er.loadDatFile(path)):
                raise IOError("can't open or read file: "+path)
    cpdef int number_of_events(self):
        return self.er.numberOfEvents()
    cpdef test(self):
        cdef short* tdc = self.er.item(0).TDC()
        print str(tdc[0])+", "+str(tdc[1])+", "+str(tdc[2])
    cpdef item(self, int index):
        e = event()
        e.set(self.er.item(index))
        return e
        
