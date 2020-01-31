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
        #datetime are always handled as UTC
        from datetime import datetime
        some_events = some_event_reader[datetime(2013,9,1), datetime(2013,9,30)]

    """
    def __init__(self, obj = None):
        cdef event_reader other_er
        if isinstance(obj, event_reader):
            other_er = obj
            self.er = other_er.er
            return

        path = obj
        if path:
            self.load(path)
            return

    def __copy__(self):
        return event_reader(self)

    def __len__(self):
        return self.er.numberOfEvents()

    def __getitem__(self, i):
        cdef int ii, start, stop
        cdef event e
        if type(i)==slice:

            if i.start==None:
                start = 0
            elif type(i.start)==datetime.datetime:
                start = date_to_timestamp(i.start)
                start = self.er.firstOlderThan(start)
            else:
                start = i.start
                start = start if start>=0 else self.er.numberOfEvents()+start
            if start >= self.er.numberOfEvents():
                raise IndexError

            if i.stop==None:
                stop = self.er.numberOfEvents()
            elif type(i.stop)==datetime.datetime:
                stop = date_to_timestamp(i.stop)
                stop = self.er.firstOlderThan(stop)
            else:
                stop = i.stop
                stop = stop if stop>=0 else self.er.numberOfEvents()+stop
            if stop >= self.er.numberOfEvents():
                raise IndexError

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
            if ii >= self.er.numberOfEvents() or ii<0:
                raise IndexError
            e = event()
            e.set(self.er.item(ii))
            return e

    def __iter__(self):
        "Simple implementation, set index and returns itself. It doesn't make sense to make nested loops over single reader and it should be avoided."
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
        "Load events from file. This delete all current events and tries to load events from file. Also tries to guess station by file name (finding station name in filename)."
        if path_to_file == '':
            raise IOError
        if path_to_file[0]=='~':
            path_to_file = expanduser('~')+path_to_file[1:]

        #try to auto get station name from filename
        st_id, st_index = 0, -1
        cdef p_Station st
        cdef vector[p_Station] stations = getStations()
        for st in stations:
            try:
                d = path_to_file.lower().index(st.name().decode(system_encoding))
            except ValueError:
                continue
            if d>st_index:
                st_index = d
                st_id = st.id()
        if st_id!=0:
            self.er.setStation(st_id)

        cdef bytes bytes_path = path_to_file.encode(system_encoding)
        if bytes_path[-4:].lower()==b".txt":
            if self.er.loadTxtFile(bytes_path):
                raise IOError("can't open or read file: "+path_to_file)
        elif bytes_path[-4:].lower()==b".dat":
            if self.er.loadDatFile(bytes_path):
                raise IOError("can't open or read file: "+path_to_file)
        else:
            raise NotImplementedError("path must be a file with .txt or .dat")

    cpdef save(self, path_to_file, bint x_events = True):
        "Save events to file. Can save txt or dat(binary files same as from website)."
        bytes_path = path_to_file.encode(system_encoding)
        if bytes_path[-4:].lower()==b".txt":
            if self.er.saveTxtFile(bytes_path, x_events):
                raise IOError("can't write file: "+path_to_file)
        elif bytes_path[-4:].lower()==b".dat":
            if self.er.saveDatFile(bytes_path):
                raise IOError("can't write file: "+path_to_file)
        else:
            raise NotImplementedError("path must be a file with .txt or .dat")

    cpdef station get_station(self):
        "Returns station, which was set for reader (either automatically or manually). Invidual events can have set different station manually."
        return station(self.er.getStation())

    cpdef int flux(self, _from, to):
        "Returns number of events between first and second timestamp or datetime"
        cdef int __from = date_to_timestamp(_from)
        cdef int _to = date_to_timestamp(to)

        val = False
        for r in self.runs():
            if __from < r[0].timestamp:
                break
            if _to <= r[-1].timestamp:
                val = True
                break
        if val:
            return self.er.flux(__from, _to)

    cpdef set_station(self, object st):
        "Set station for event_reader. Station is also set for all current events."
        cdef int _id
        if type(st)==int:
            _id = st
        elif type(st)==str:
            _id = station(st).id
        elif type(st)==station:
            _id = st.id
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

    cpdef int measure_length(self):
        "Return sum of times in invidual runs, so this is effective measure time."
        return self.er.measurelength()

    cdef Event c_item(self, int i):
        return self.er.item(i)

    cpdef event item(self, int i):
        "indexes events"
        e = event()
        e.set(self.er.item(i))
        return e

    cpdef int measure_time(self):
        "Return sum of times in invidual runs, so this is effective measure time."
        return self.er.measurelength()

    #filters
    cpdef int filter(self, filter_func):
        "Custom-filter function. As parameter give a function, which is ready to be called with parameter event object, which return True if you want remove event and False if you want let event in event_reader. Implementation is quite raw, so exceptions inside function are not handled properly!"
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
        self.i += 1
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
            for ii from start <= ii < stop by step:
                runs.append(event_reader_run(self.er, ii))
            return runs
        else:
            ii = i
            runs = self.er.er.numberOfRuns()
            if ii<0:
                ii += runs
            if ii >= runs or ii < 0:
                raise IndexError
            return event_reader_run(self.er, ii)


cdef class event_reader_run:
    "wrapper class for indexing invidual run"
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
        cdef int ii, start, stop
        if type(i)==slice:
            if i.step != 0:
                raise NotImplementedError
            start = 0 if not i.start else i.start if i.start>=0 else self.er.er.numberOfEvents(self._run_id)+i.start
            stop = self.er.er.numberOfEvents(self._run_id) if not i.stop else i.stop if i.stop>=0 else self.er.er.numberOfEvents(self._run_id)+i.stop
            es = []
            for ii in range(start,stop):
                e = event()
                e.set(self.er.er.item(self._run_id, ii))
                es.append(e)
            return es
        else:
            ii = i
            events = self.er.er.numberOfEvents(self._run_id)
            if ii < 0:
                ii += events
            if ii < 0 or ii >= events:
                raise IndexError
            e = event()
            e.set(self.er.er.item(self._run_id, ii))
            return e

    cpdef int run_id(self):
        return self._run_id


    cpdef int begin_index(self):
        return self.er.er.runStartIndex(self._run_id)


    cpdef int end_index(self):
        return self.er.er.runEndIndex(self._run_id)


    def average_TDC(self):
        "Method to calculate average TDC of non calibration events in run. Returns tuple(avg_tdc[0], avg_tdc[1], avg_tdc[3], number_of_events)"
        cdef long[3] sum_tdc
        cdef int n
        cdef int i

        for i in range(3):
            sum_tdc[i] = 0

        for e in self:
            if not e.calibration:
                n += 1
                tdc = e.TDC
                for i in range(3):
                    sum_tdc[i] += tdc[i]
        if n == 0:
            return (0,0,0)

        return (1.0*sum_tdc[0]/n, 1.0*sum_tdc[1]/n, 1.0*sum_tdc[2]/n, n)


    def average_diff_TDC(self):
        "Method to calculate average difference TDC of non calibration events in run. Returns tuple(0, avg_tdc[0] - avg_tdc[1], avg_tdc[0] - avg_tdc[2], number_of_events). Can be used as correction"
        avg_tdc = self.average_TDC()
        return (0.0, avg_tdc[0] - avg_tdc[1], avg_tdc[0] - avg_tdc[2], avg_tdc[3])
