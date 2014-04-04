#!/usr/bin/env python3
# vim: set fileencoding=UTF-8 :
from distutils.core import setup
from distutils.extension import Extension

try:
    from Cython.Build import cythonize
    USE_CYTHON = True
except ImportError:
    USE_CYTHON = False

ext_modules = [Extension('czelta', ['czelta.pyx' if USE_CYTHON else 'czelta.cpp',
                                    'event.cpp',
                                    'event_reader.cpp',
                                    'common_func.cpp',
                                    'station.cpp',
                                    'coincidence.cpp'],
                language='c++',
                extra_compile_args=['-std=c++0x']
    )]
if USE_CYTHON:
    ext_modules = cythonize(ext_modules)

setup(
  name = 'czelta',
  version = '0.1',
  description = 'Analysis libraty for project CZELTA (http://czelta.utef.cvut.cz/)',
  author = 'Martin Quarda',
  author_email = 'hinogary@gmail.com',
  url = 'http://czelta.quarda.cz/',
  license='LICENSE.txt',
  long_description = open('README.md').read(),
  ext_modules = ext_modules
)
