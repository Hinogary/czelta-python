#!/usr/bin/env python3
# vim: set fileencoding=UTF-8 :
from distutils.core import setup
from distutils.extension import Extension
import distutils.sysconfig
import platform

try:
    from Cython.Build import cythonize
    USE_CYTHON = True
except ImportError:
    print("USE_CYTHON = False")
    USE_CYTHON = False

ext_modules = [Extension('czelta', ['czelta.pyx' if USE_CYTHON else 'czelta.cpp',
                                    'event.cpp',
                                    'event_reader.cpp',
                                    'common_func.c',
                                    'station.cpp',
                                    'coincidence.cpp'],
                language='c++',
                extra_compile_args=['-std=c++0x']
    )]
if USE_CYTHON:
    ext_modules = cythonize(ext_modules)

if platform.system()=='Windows':
    data_files = [(distutils.sysconfig.get_python_lib(), ['libgcc_s_dw2-1.dll','libstdc++-6.dll', 'config_data.JSON', 'LICENSE.txt'])]
else:
    data_files = [(distutils.sysconfig.get_python_lib(), ['config_data.JSON', 'LICENSE.txt'])]

setup(
  name = 'czelta',
  version = '0.1',
  description = 'Analysis libraty for project CZELTA (http://czelta.utef.cvut.cz/)',
  author = 'Martin Quarda',
  author_email = 'hinogary@gmail.com',
  url = 'http://czelta.quarda.cz/',
  license='LICENSE.txt',
  long_description = open('README.md').read(),
  ext_modules = ext_modules,
  data_files = data_files
)
