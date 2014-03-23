#!/usr/bin/env python3
# vim: set fileencoding=UTF-8 :
from distutils.core import setup
from distutils.extension import Extension

name = 'czelta'
version = '0.1'
description = ''
author = 'Martin Quarda'
author_email = 'hinogary@gmail.com'
url = 'http://czelta.quarda.cz/'

try:
    from Cython.Distutils import build_ext

    ext_modules = [Extension('czelta', ['czelta.pyx','event.cpp','event_reader.cpp','common_func.cpp','station.cpp','coincidence.cpp'],
                language='c++',
                extra_compile_args=['-std=c++0x']
    )]
    setup(
      name = name,
      version = version,
      description = description,
      author = author,
      author_email = author_email,
      url = url,
      cmdclass = {'build_ext': build_ext},
      ext_modules = ext_modules
    )
except ImportError:
    print('Cython can\'t import, try to compile module with precompiled czelta.pyx')
    ext_modules = [Extension('czelta', ['czelta.cpp','event.cpp','event_reader.cpp','common_func.cpp','station.cpp','coincidence.cpp'],
                language='c++',
                extra_compile_args=['-std=c++0x']
    )]
    setup(
      name = name,
      version = version,
      description = description,
      author = author,
      author_email = author_email,
      url = url,
      ext_modules = ext_modules
    )
