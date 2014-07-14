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
            return self.e.time_since_second()
            
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
            
    property AH_direction:
        "Return ``(horizon, azimuth)`` direction of shower. Azimuth is from south clockwise. Both values are in Degres. Must have loaded info about stations and set station for ``event``/``event_reader``"
        def __get__(self):
            cdef float *AH = self.e.calculateDir()
            if AH:
                return (AH[0],AH[1])
                
    property RAD_direction:
        "Return ``(right ascension, declination)`` direction of shower in Degrees. Must have loaded info about stations and set station for ``event``/``event_reader``"
        def __get__(self):
            cdef float *DRA = self.e.calculateEarthDir()
            if DRA:
                return (DRA[0], DRA[1])
                
    property station:
        def __get__(self):
            return station(self.e.getStation())
            
    cpdef set_station(self, station_id):
        "Set station to correct tdc and calculate direction, it is better to change station of entire ``czelta.event_reader``."
        self.e.setStation(<int>station_id)
