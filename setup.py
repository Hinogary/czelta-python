#!/usr/bin/env python
# vim: set fileencoding=UTF-8 :
from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

ext_modules = [Extension("czelta", ["czelta.pyx","event.cpp","event_reader.cpp","common_func.cpp","station.cpp","coincidence.cpp"],
            language="c++",
            extra_compile_args=["-std=c++0x"]
)]

setup(
  name = "czelta",
  cmdclass = {"build_ext": build_ext},
  ext_modules = ext_modules,
)