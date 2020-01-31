`czelta.event_reader`
=====================

.. module:: czelta

.. autoclass:: event_reader

   .. automethod:: load(path_to_file)
   .. automethod:: save(path_to_file)
   .. automethod:: runs()
   .. automethod:: run(run_index)
   .. automethod:: set_station(station)

      `station` can be string (``'pardubice_gd'``) or int (station id) or station object itself.

   .. automethod:: filter(filter_func)

      filtering all calibrations::

        def filter_func(e):
            return e.calibration
        some_event_reader.filter(filter_func)

      filtering all except calibrations::

        def filter_func(e):
            return not e.calibration
        some_event_reader.filter(filter_func)

      filtering all events with horizont below 40 degres::

        def filter_func(e):
            if e.HA_direction[0]<40:
                return True
        # must have loaded config_data.JSON: czelta.station.load()
        # and set station with: some_event_reader.set_station("some_station")
        some_event_reader.filter(filter_func)

   .. automethod:: filter_calibrations()
   .. automethod:: filter_maximum_TDC()
   .. automethod:: filter_maximum_ADC()
   .. automethod:: filter_minimum_ADC()
   .. automethod:: flux(_from, to)
   .. automethod:: number_of_events(run)
   .. automethod:: number_of_runs()
   .. automethod:: item(i)
   .. automethod:: measure_time()

.. autoclass:: event_reader_runs

.. autoclass:: event_reader_run
   .. automethod:: average_TDC()
   .. automethod:: average_diff_TDC()
