#author: Martin Quarda

import datetime
import json
import sys
import traceback
import os
from os.path import expanduser

__version__ = '0.1'
__all__ = ['station','event','event_reader','date_to_timestamp','coincidence']
__author__ = 'Martin Quarda <hinogary@gmail.com>'
system_encoding = sys.getfilesystemencoding()

cpdef int date_to_timestamp(d):
    "Convert string or datetime object to timestamp. Example date: '27.4.2013 17:44:53'. Datetime is interpreted as UTC."
    if type(d)==datetime.datetime:
        return date(d.year, d.month, d.day, d.hour, d.minute, d.second)
    else:
        return char_date((<string>d.encode(system_encoding)).c_str())

include "station.pxi"
include "event.pxi"
include "coincidence.pxi"
include "event_reader.pxi"

#autoload config_data.JSON if avaible
try:
    import distutils.sysconfig
    station.load(open(distutils.sysconfig.get_python_lib()+os.sep+"config_data.JSON"))
except:
    try:
        station.load(open("config_data.JSON"))
    except:
        pass
