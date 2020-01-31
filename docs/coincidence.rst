`czelta.coincidence`
====================

.. module:: czelta

.. autoclass:: coincidence

   example of double-coincidence::

        >>> import czelta
        >>> ers = []
        >>> ers = czelta.event_reader('pardubice_gd.dat'), czelta.event_reader('pardubice_spse.dat')
        >>> coin = czelta.coincidence(ers, 2.2e-6)
        >>> for c in coin:
        ...     print(c[0])
        2.1643e-06
        1.9975e-06
        9.444e-07
        2.653e-07
        6.76e-07
        ... #and continuing
        >>> coin.expected_value
        0.09488086868552982
        >>> coin.overlap_normal_events #on pardubice_spse is much more events than gd
        (799700, 1242876)

   example of triple-coincidence::

        >>> import czelta
        >>> ers = []
        >>> ers.append(czelta.event_reader('~/data/opava_mg.dat'))
        >>> ers.append(czelta.event_reader('~/data/opava_su.dat'))
        >>> ers.append(czelta.event_reader('~/data/opava_zsbn.dat'))
        >>> coin = czelta.coincidence(ers, 0.001) #limit is 1 ms (0.001 s)
        >>> len(coin) #number of coincidences
        4
        >>> coin[0][0] #time between first and last event in seconds (5.033e-07 is 0.5033 us)
        5.033e-07
        >>> str(coin[0][1]) #event from first station (opava_mg)
        'a 2011 10 20 21 26 55 880465000.4 1257 3751 2631 203 522 253 -1.0 15.0 15.5 12.5'
        >>> str(coin[0][2]) #event from second station (opava_su)
        'a 2011 10 20 21 26 55 880465455.8 123 1405 3775 737 827 718 -1.0 -1.0 -1.0 27.5'
        >>> str(coin[0][3]) #event from third station (opava_zsbn)
        'a 2011 10 20 21 26 55 880465503.7 4095 377 3820 548 867 800 17.5 -1.0 -1.0 24.5'
        >>> coin[0][4] #azimut and horizon of triple coincidence
        (229.07884216308594, 75.77786254882812)
        >>> coin[0][5] #Right Ascention and Declination
        (28.959278106689453, 57.71996307373047)

   .. autoattribute:: max_difference
   .. autoattribute:: number_of_coincidences
   .. autoattribute:: expected_value
   .. autoattribute:: chance
   .. autoattribute:: overlap_measure_time
   .. autoattribute:: overlap_normal_events
   .. autoattribute:: overlap_calibration_events
   .. autoattribute:: delta
   .. autoattribute:: stations
   .. autoattribute:: events
