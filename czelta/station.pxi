from libc.math cimport sqrt

cdef class station:
    "Class for working with station data. On import it tries to load config_data.JSON (in python lib path and after failture in local directory."


    def __init__(self, station):
        if type(station)==int:
            self.st = &getStation(<int>station)
        else:
            self.st = &getStation(<string>station.encode(system_encoding))
        if self.st.id()==0:
            raise RuntimeError("Station not exist, have you loaded config file?")


    property id:
        "Return `station id`, probably same as on czelta website."
        def __get__(self):
            return self.st.id()


    property name:
        "Return code name of station. Example: ``\'praha_utef\'``, ``\'pardubice_gd\'`` or similar."
        def __get__(self):
            return str((<char*>self.st.name()).decode(system_encoding))


    property detector_position:
        "Return position of detectors in format ``(x1, y1, x2, y2)`` where ``x1`` and ``y1`` are relative position of detector 1 to detector 0. ``x2`` and ``y2`` are relative position of detector 2 to detector 0. All values are in metres. Y+ is targeting to North and X+ is targeting to East."
        def __get__(self):
            cdef float* dp = self.st.detectorPosition()
            return (dp[0], dp[1], dp[2], dp[3])


    property gps_position:
        "Returns GPS position of station. Return ``(latitude, longitude)`` or ``None`` if gps_position is not defined."
        def __get__(self):
            cdef double* gp = self.st.GPSPosition()
            if gp[0]==0 and gp[1]==0:
                return None
            return (gp[0], gp[1], gp[2])


    cpdef distance_to(self, station other_station):
        "Calculate distance to other station using haversine method. The return number is in metres."
        cdef double distance = self.st.distanceTo(other_station.st[0])
        if distance==0:
            return None
        else:
            return distance


    def get_corrections(self, timestamps=False):
        "Returns list of corrections, argument timestamps specify if from values are in timestamp(True) or text date(False)."
        cdef object corrections = []
        cdef int i
        for i in range(self.st.TDCCorrections_size()):
            corrections.append({
                'from': self.st.TDCCorrections()[i]._from if timestamps else timestamp_to_date(self.st.TDCCorrections()[i]._from),
                'corrections': [self.st.TDCCorrections()[i].correction[k]
                for k in range(3)]
            })
        return corrections


    def autofit_corrections(self, event_reader eventreader, float confidence_level = 0.0001, int instability = 25):
        """
        Automatically fits TDC corrections based on EventReader.
        For best effect, data should be downloaded for full measure time of station.

        It works by calculating correction for each run (few days in most cases)
        and decide if it should be merged with previous aggregation based on
        statistical testing if hypotheses that run corrections is same as
        previous aggregation.
        Testing of hypotheses is based on two-sample normal distribution test.

        confidence_level is float between 1 and 0, bigger values indicates, that
        smaller change in means triggers corrections split. Smaller values means,
        that it less sensitive (but because it's based on Student's distribution
        even really small values are enough sensitive). Default value is 0.01% (= 0.0001).

        instability is value, which specifies allowed fluctiations.
        Differences difference in means are lowered by instability
        and then they are processed by statistics. To disable it choose 0.
        Default value is 25.
        """

        cdef int tdc01, tdc02

        cdef long[2] agg_diff_tdc
        cdef long[2] agg_diff_tdc2
        cdef int agg_n

        cdef long[2] curr_diff_tdc
        cdef long[2] curr_diff_tdc2
        cdef int curr_n

        cdef double[2] agg_mean
        cdef double[2] agg_sigma
        cdef double[2] curr_mean
        cdef double[2] curr_sigma
        cdef bint accept

        cdef long _from = 0

        cdef EventReader* er = &(eventreader.er)
        cdef Event* e
        cdef int r, i, k

        from scipy.stats import t

        for i in range(2):
            agg_diff_tdc[i] = 0
            agg_diff_tdc2[i] = 0
        agg_n = 0

        for r in range(er.numberOfRuns()):

            # reset run specific values
            curr_n = 0
            for i in range(2):
                curr_diff_tdc[i] = 0
                curr_diff_tdc2[i] = 0

            # calculate run specific values
            for i in range(er.numberOfEvents(r)):
                e = &er.item(r, i)
                if (e.isCalib()  # calibrations are not from sky, so no fitting
                    or e.TDC0() in (0, 2047) # 0 and 2047 are values which are in most cases just bad recorded for suspicious reason
                    or e.TDC1() in (0, 2047)
                    or e.TDC2() in (0, 2047)):
                    continue
                tdc01 = e.TDC0() - e.TDC1()
                tdc02 = e.TDC0() - e.TDC2()
                curr_n += 1
                curr_diff_tdc[0] += tdc01
                curr_diff_tdc[1] += tdc02
                curr_diff_tdc2[0] += tdc01 * tdc01
                curr_diff_tdc2[1] += tdc02 * tdc02

            # if data are that low, that it could be unstable, just aggregate
            if agg_n < 1000 or curr_n < 500:
                for i in range(2):
                    agg_diff_tdc[i] += curr_diff_tdc[i]
                    agg_diff_tdc2[i] += curr_diff_tdc2[i]
                agg_n += curr_n
                continue

            # calculate means and sigmas according to new values
            for i in range(2):
                agg_mean[i] = agg_diff_tdc[i] / agg_n
                curr_mean[i] = curr_diff_tdc[i] / curr_n
                agg_sigma[i] = sqrt(
                        1.0 * agg_diff_tdc2[i] / agg_n
                        - agg_mean[i] * agg_mean[i]
                )
                curr_sigma[i] = sqrt(
                        1.0 * curr_diff_tdc2[i] / curr_n
                        - curr_mean[i] * curr_mean[i]
                )
                # nan shield
                if not agg_sigma[i] == agg_sigma[i]:
                    agg_sigma[i] = 1.0
                if not curr_sigma[i] == curr_sigma[i]:
                    curr_sigma[i] = 1.0


            # based on simple accepting criteria, could be more complex
            # and more precisious, but this should be enough
            accept = True
            for i in range(2):
                sd = sqrt(curr_sigma[i]**2/curr_n + agg_sigma[i]**2/agg_n)
                nd = sd**4/((curr_sigma[i]**2/curr_n)**2/(curr_n-1) + (agg_sigma[i]**2/agg_n)**2/(agg_n-1))
                T = max(0, abs(curr_mean[i] - agg_mean[i]) - instability)/sd
                critical = t.isf(confidence_level/2, nd)
                accept = accept and T <= critical
                #print(T, critical, sd)

            print('agg', agg_mean, agg_sigma, agg_n)
            if not accept:
                print('------------------')
            print('cur', curr_mean, curr_sigma, curr_n)

            if not accept:
                # there is path, where we know that current run is different
                # we need to push aggregated corrections and reset it

                self.st.pushTDCCorrect(_from, 0, <int>agg_mean[0], <int>agg_mean[1])

                # reset it
                for i in range(2):
                    agg_diff_tdc[i] = 0
                    agg_diff_tdc2[i] = 0
                agg_n = 0

                _from = er.item(r, 0).timestamp()

            # aggregate and continue next run
            for i in range(2):
                agg_diff_tdc[i] += curr_diff_tdc[i]
                agg_diff_tdc2[i] += curr_diff_tdc2[i]
            agg_n += curr_n


        self.st.pushTDCCorrect(_from, 0, <int>agg_mean[0], <int>agg_mean[1])


    cpdef clear_corrections(self):
        "Remove all current corrections for specific station."
        self.st.clearTDCCorrect()


    @staticmethod
    def load(file = None, _format = 'yaml'):
        "Loads configuration file of stations. Must be called if you want to work with ``czelta.station``."
        cdef Station st
        cdef object pos
        if file==None:
            file = open("config_data.JSON")
        if _format == 'yaml':
            cfg = yaml.load(file)
        elif _format == 'json':
            cfg = json.load(file)
        else:
            raise ValueError('Parameter _format must be in (\'yaml\', \'json\'!')
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
                if 'TDCCorrection' in station and len(station['TDCCorrection'])>0:
                    st.clearTDCCorrect()
                    for correction in station['TDCCorrection']:
                        tdc = correction['correction']
                        if not 'from' in correction:
                            correction['from'] = 0
                        if type(correction['from'])==int:
                            st.pushTDCCorrect(<int>correction['from'], <short>tdc[0], <short>tdc[1], <short>tdc[2])
                        else:
                            from_cor = correction['from'].encode(system_encoding)
                            st.pushTDCCorrect(<string>from_cor, <short>tdc[0], <short>tdc[1], <short>tdc[2])
                addStation(st)
            except Warning:
                st_name = (<char*>st.name()).decode(system_encoding)
                print "Station can't be added, bad format of JSON, id: "+str(st.id())+", name: "+st_name
            finally:
                st = Station()

    @staticmethod
    def save(file, _format):
        "Save current configuration in use. Main usage is with fitting TDC. Formatting of original file is not preserved, stations are just ordered by ID."
        dmp_all = {'stations':[]}
        cdef station st
        for station in station.get_stations():
            st = station
            dmp = {}
            dmp_all['stations'].append(dmp)
            dmp['id'] = st.st.id()
            if st.st.name():
                dmp['name'] = str(st.st.name())

            dmp['GPSposition'] = [st.st.GPSPosition()[i] for i in range(3)]
            if dmp['GPSposition'] == [0.0, 0.0, 0.0]:
                del dmp['GPSposition']

            dmp['detectorsPos'] = [st.st.detectorPosition()[i] for i in range(4)]
            if dmp['detectorsPos'] == [0.0, 0.0, 0.0, 0.0]:
                del dmp['detectorsPos']

            dmp['TDCCorrection'] = [
                {
                    'from': st.st.TDCCorrections()[i]._from,
                    'correction': [
                        st.st.TDCCorrections()[i].correction[j]
                        for j in range(3)
                    ],
                } for i in range(st.st.TDCCorrections_size())
            ]

        if _format == 'json':
            raise NotImplementedError
        elif _format == 'yaml':
            raise NotImplementedError
        else:
            raise ValueError('Parameter _format must be in (\'yaml\', \'json\'!')


    @staticmethod
    def get_stations():
        "Return list of all available stations."
        cdef vector[p_Station] stations = getStations()
        rtn = []
        for st in stations:
            rtn.append(station(st.id()))
        return rtn


    @staticmethod
    def clear_config():
        "Removes complete configuration."
        cdef station st
        for station in station.get_stations():
            st = station
            st.st.delet()
