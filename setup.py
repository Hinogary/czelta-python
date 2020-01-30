#!/usr/bin/env python3
# vim: set fileencoding=UTF-8 :
from distutils.core import setup
from distutils.extension import Extension
import distutils.sysconfig
import platform

from Cython.Build import cythonize

ext_modules = [
    Extension('czelta', ['czelta/czelta.pyx',
                         'czelta/event.cpp',
                         'czelta/event_reader.cpp',
                         'czelta/common_func.c',
                         'czelta/station.c',
                         'czelta/coincidence.cpp'],
              language='c++',
              extra_compile_args=['--std=c11'],
    )
]
ext_modules = cythonize(ext_modules)

if platform.system() == 'Windows':
    data_files = [(distutils.sysconfig.get_python_lib(), ['libgcc_s_dw2-1.dll', 'libstdc++-6.dll', 'config_data.JSON', 'LICENSE.txt'])]
else:
    data_files = [(distutils.sysconfig.get_python_lib(), ['config_data.JSON', 'LICENSE.txt'])]

setup(
  name='czelta',
  version='1.0',
  description='Analysis libraty for project CZELTA (http://czelta.utef.cvut.cz/)',
  classifiers=[
      'Framework :: Sphinx',
      'Framework :: Pytest',
      'Intended Audience :: Developers',
      'License :: OSI Approved :: GNU General Public License v3 (GPLv3)',
      'Programming Language :: Cython',
      'Programming Language :: C++',
      'Topic :: Scientific/Engineering :: Physics',
  ],
  keywords='czelta cosmic rays',
  author='Martin Quarda',
  author_email='hinogary@gmail.com',
  url='https://github.com/Hinogary/czelta-python/',
  license='LICENSE.txt',
  install_requires=['scipy', 'PyYAML'],
  long_description=open('README.md').read(),
  ext_modules=ext_modules,
  data_files=data_files,
)
