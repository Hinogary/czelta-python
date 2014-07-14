cdef class coincidence:
    """Class for calculate coincidences of more stations. Currently supported is double and triple coincidences. With triple coincidences is also posible to calculate direction of coincidence.
    
Constructor have format: ``coincidence(event_readers in list or tuple, save_events = True, stations = auto)``
if you have giant limits on double coincidences, it is sometimes better to don't save events. Stations are by default got from event readers.

coincidence object is also iterable, more in examples.
"""
    def __init__(self,event_readers,float max_difference, bint save_events = True, stations = None):
        cdef int st_id, st
        if not len(event_readers) in [2,3]:
            raise TypeError
        for i in range(len(event_readers)):
            if not isinstance(event_readers[i], event_reader):
                raise TypeError
            self.c.readers[i] = &(<event_reader>event_readers[i]).er
        self.c.n = len(event_readers)
        if stations!= None and len(stations)==len(event_readers):
            for st in range(len(stations)):
                st_id = 0
                if type(stations[st]) == station:
                    st_id = stations[st].id()
                else:
                    st_id = station(stations[st]).id()
                self.c.stations[st] = st_id
        else:
            for st in range(len(event_readers)):
                st_id = (<event_reader>event_readers[st]).er.getStation();
                self.c.stations[st] = st_id
        self.c.calc(max_difference, save_events)
        
    def __len__(self):
        return self.c.numberOfCoincidences
        
    def __getitem__(self, index):
        cdef int i
        if type(index) != int:
            raise TypeError
        i = index
        if self.c.events_saved:
            if self.c.n == 2:
                rtn = (self.c.delta[i], event(), event())
                (<event>rtn[1]).set(self.c.events[0][i])
                (<event>rtn[2]).set(self.c.events[1][i])
                return rtn
            else:
                if self.c.dirs.size()!=0:
                    AH = (self.c.dirs[4*i], self.c.dirs[4*i+1])
                    if AH[0]==0 and AH[1]==0:
                        AH = None
                        DRA = None
                    else:
                        DRA = (self.c.dirs[4*i+2], self.c.dirs[4*i+3])
                    rtn = (self.c.delta[i], event(), event(), event(), 
                          AH, DRA)
                else:
                    rtn = (self.c.delta[i], event(), event(), event())
                (<event>rtn[1]).set(self.c.events[0][i])
                (<event>rtn[2]).set(self.c.events[1][i])
                (<event>rtn[3]).set(self.c.events[2][i])
                return rtn
        else:
            return (self.c.delta[i],)
            
    def __iter__(self):
        self.i = -1
        return self
        
    def __next__(self):
        self.i += 1
        if self.i < self.c.numberOfCoincidences:
            return self[self.i]
        else:
            raise StopIteration
            
    property delta:
        'Return all deltas of coincidences.'
        def __get__(self):
            return list(self.c.delta)
            
    property stations:
        'Get stations used to calculate direction of triple-coincidence.'
        def __get__(self):
            if self.c.n==2:
                return station(self.c.stations[0]), station(self.c.stations[1])
            else:
                return station(self.c.stations[0]), station(self.c.stations[1]), station(self.c.stations[2])
                
    property events:
        'Get all events.'
        def __get__(self):
            cdef Event e
            cdef event ev
            if not self.c.events_saved:
                raise AttributeError("You have calculated coincidences without events")
            if self.c.n == 2:
                rtn = [],[]
            else:
                rtn = [],[],[]
            for i in range(self.c.n):
                for e in self.c.events[i]:
                    ev = event()
                    ev.set(e)
                    rtn[i].append(ev)
            return rtn
            
    property max_difference:
        "Return used limit between coincidences."
        def __get__(self):
            return self.c.limit
            
    property number_of_coincidences:
        "Get number of coincidences, same effetct have ``len(coincidence)''."
        def __get__(self):
            return self.c.numberOfCoincidences
            
    property expected_value:
        "Number of random coincidences expected."
        def __get__(self):
            return self.c.medium_value
            
    property chance:
        "Chance of finding ``len(coincidence)`` based on ``expected_value``."
        def __get__(self):
            return self.c.chance
            
    property overlap_measure_time:
        "Total time of overlap measure."
        def __get__(self):
            return self.c.overlap.measureTime;
            
    property overlap_normal_events:
        "Number of normal events on invidual stations."
        def __get__(self):
            if self.c.n==2:
                return (self.c.overlap.normal_events[0], self.c.overlap.normal_events[1])
            else:
                return (self.c.overlap.normal_events[0], self.c.overlap.normal_events[1], self.c.overlap.normal_events[2])
                
    property overlap_calibration_events:
        "Number of calibration events on invidual stations."
        def __get__(self):
            if self.c.n==2:
                return (self.c.overlap.calibration_events[0], self.c.overlap.calibration_events[1])
            else:
                return (self.c.overlap.calibration_events[0], self.c.overlap.calibration_events[1], self.c.overlap.calibration_events[2])

