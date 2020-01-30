#author: Martin Quarda

import datetime
import json
import yaml
import sys
import traceback
import os
from os.path import expanduser

__version__ = '0.1'
__all__ = ['station', 'event', 'event_reader', 'date_to_timestamp', 'coincidence']
__author__ = 'Martin Quarda <martin@quarda.cz>'
system_encoding = sys.getfilesystemencoding()

cpdef date_to_timestamp(d):
    "Convert string or datetime object to timestamp. On integer it is assuming it's already date, so it just returns int. Example date: '27.4.2013 17:44:53'. Datetime is interpreted as UTC or timezone in tzinfo."
    if isinstance(d, datetime.datetime):
        if d.tzinfo is not None:
            d = d.astimezone(datetime.timezone.utc)
        return date(d.year, d.month, d.day, d.hour, d.minute, d.second)
    elif isinstance(d, str):
        return char_date((<string>d.encode(system_encoding)).c_str())
    elif isinstance(d, bytes):
        return char_date((<string>d).c_str())
    elif isinstance(d, int):
        return int(d)
    else:
        raise ValueError('Unkown date: %s'%d)


def timestamp_to_date(t):
    date = datetime.datetime.utcfromtimestamp(t)
    return date.strftime(r'%d.%m.%Y %H:%M')



include "station.pxi"
include "event.pxi"
include "coincidence.pxi"
include "event_reader.pxi"

#autoload config_data.JSON if avaible
try:
    import distutils.sysconfig
    station.load(open(distutils.sysconfig.get_python_lib()+os.sep+"config_data.JSON"), 'json')
except:
    try:
        station.load(open("config_data.JSON"), 'json')
    except:
        pass
