Tutorials and examples
======================

Importing library
-----------------

Import of libraty is straight-foward::
    
    import czelta
    #or
    from czelta import event_reader, station

If you have some errors you can try this for more information::

    from ctypes import CDLL
    #path is based on OS and python version   
    CDLL('/usr/local/lib/python2.7/dist-packages/czelta.so')
    #for windows it is something like r'C:\Python27\Lib\site-packages\czelta.pyd'
    
Loading data
------------

Basic class for loading and iteration with data is :py:class:`czelta.event_reader`.
The path of data can be passed in argument of constructor or method :py:func:`czelta.event_reader.load`::

    import czelta
    
    #constructor
    er = czelta.event_reader('some_data.txt')
    
    #method
    er = czelta.event_reader()
    er.load('some_data.dat')

If data is in home directory, you can use `~` as home directory::

    #load data in directory like '/home/user_name/czelta_data/some_station.dat' on linux or 'C:\users\user_name\czelta_data\some_station.dat' on windows.
    er = czelta.event_reader('~/czelta_data/some_station.dat')

Saving data
-----------

The library uses 2 save formats: ``.txt`` and ``.dat``.

``.dat`` format is very compact format. Each event uses only 32B and completely file occupes only ``len(event_reader)*32+16`` bytes.

``.txt`` format is quite fat format. But it is easier to import to application like excel and so-on.

For saving data is there function :py:func:`czelta.event_reader.save`::
    
    #save to txt format with 'x events' (indicator of runs)
    er.save('~/data/my_data.txt')
    
    #save to txt format without 'x events'
    er.save('~/data/my_data.txt', x_events = False)
    
    #save to dat format
    er.save('~/data/my_data.dat')
    

Working with events
-------------------

Working with runs
-----------------

Using builtin filters
---------------------

Using custom filter function
----------------------------

There is a method :py:func:`czelta.event_reader.filter` for custom filtering.

The custom filter is just function with parameter :py:class:`czelta.event`. The function return True if want to filter out event (delete) or False if not::

    def custom_filter(event):
        #filter all calibrations
        return event.calibration
    
    #apply filter
    er.filter(custom_filter)
    
More examples in :py:func:`czelta.event_reader.filter`.
