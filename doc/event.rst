`czelta.event`
==============

.. module:: czelta

.. autoclass:: event

    .. data:: timestamp
    
       Unix timestamp of event, fastest way to get time of event.
       
    .. data:: datetime
    
       Return python `datetime <http://docs.python.org/2/library/datetime.html>`_ object. All times with Czelta is in UTC.
       
    .. data:: time_since_second
    
       Return time elapsed since last second (0-0.999999... sec).
    
    .. data:: ADC
    
       Relative energy absorbed in each detector. Probably not comparable along different stations. Minimum value is 0 and Maximum is 2047. If it is 2047 it shloud be more.
    
    .. data:: TDC
    
       Relative time of activation each detector. TDC*25/1e12 = sec. Format: ``(TDC0, TDC1, TDC2)``.
    
    .. data:: TDC_corrected
    
       Relative time of activation each detector. Corrected and can be used to calculate diraction. Correction options are in `config_data.JSON`. TDC*25/1e12 = sec.

    .. data:: temps_detector
    
       Return 3 temps of each detector in time of event.
       
    .. data:: temp_crate
    
       Return Temperature in crate in time of event.
       
    .. data:: HA_direction
    
       Return ``(horizon, azimuth)`` direction of shower. Azimuth is from south clockwise. Both values are in Degres. Must have loaded info about stations and set station for ``event``/``event_reader``
       
    .. automethod:: set_station(station_id)
