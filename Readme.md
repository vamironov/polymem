PolyMem: memory performance benchmark
=====================================

Benchmark CPU and memory subsystem together
by computing polynomials of specified degree
over large in-memory arrays of data.

The idea of this test is to get STREAM-like
memory access pattern, but with variable
arithmetic intensity. Arithmetic
intensity is the ratio (CPU time)/(memory time).
By varying it one can find how balanced are
compute and memory subsystems of the computer.


REQUIREMENTS
------------
OpenMP-aware C compiler, GNU make

COMPILING
---------
Adjust Makefile to meet your system
configuration. Then run `make` in
the source code directory.

RUNNING
-------
Two types of memory access patterns are available:
 ### Read-only access patter
 The input data is stored in one large array in memory.
 The calculation results are discarded. To run benchmark, type

       ./poly_ro.x [polynomial degree] [array size] [repeats]

 - [int] Polynomial degree: the higher the polynomial degree,
 - the more is arithmetic intensity of the benchmark.
 - [int] Array size: number of elements of an array
 - [int] Repeats: number of times the test will run.


 ### Read-write access pattern
 The input data and the results are stored in two arrays
 in memory. The memory requirements are thus 2x bigger
 than in read-only test. To run the benchmark, type

       ./poly_rw.x [polynomial degree] [array size] [repeats]

 Options are same to read-only test.
