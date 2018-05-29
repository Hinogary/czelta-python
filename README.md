Czelta file loading and analysis library
========================================

Compilation requirement:
------------------------
* C++11 compiler for use with C++ code
* Cython compiler and Python itself for use with Python code

Example Python code:
--------------------
```
import czelta

#init eventreader object
reader = czelta.event_reader()

#load data from file, usally downloaded from czelta website
reader.load("test.dat")

print("Events loaded: %i"%reader.number_of_events())

t_sum = 0
for event in reader:
    t_sum += sum(event.temps()[:2])/3

t_avg = t_sum/len(reader)
print("Average tempeature is %f *C"%t_avg)
```
