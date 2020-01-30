`czelta.station`
================

.. module:: czelta

.. autoclass:: station

   example::

        >>> import czelta
        >>> #load data about stations, default try to load config_data.JSON
        >>> czelta.station.load(open('config_data.JSON'))
        >>> #get some station
        >>> some_station = czelta.station(5)
        >>> print("station name is %s"%some_station.name())
        station name is kladno_sps
        >>> praha_utef = czelta.station("praha_utef")
        >>> print("id of praha_utef is %i"%praha_utef.id())
        id of praha_utef is 6
        >>> #prints distance between stations
        >>> distance = praha_utef.distance_to(some_station)
        >>> print("distance of %s to praha_utef is %f metres"%(some_station.name(), distance))
        distance of kladno_sps to praha_utef is 24347.04590484944 metres


   .. automethod:: load(file = open("config_data.JSON"))
   .. automethod:: get_stations
   .. automethod:: clear_config
   .. automethod:: id
   .. automethod:: name
   .. automethod:: detector_position
   .. automethod:: gps_position
   .. automethod:: distance_to(other_station)
