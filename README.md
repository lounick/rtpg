# RT-PG #

A real time property graph implementation.

## Description ##

RT-PG provides a basic property graph implementation that is capable of running
in real time. It depends on a [custom memory
allocator](https://github.com/lounick/handle_allocator.git). The project is
written in C++ and provides a Python interface.

## Building ##

To build the code use the following commands:

```console
$ mkdir build && cd build && cmake .. && make
```

This will automatically pull any required dependencies and build the project.

## Usage ##

RT-PG provides a class representing a property graph and methods to interact
with it (i.e. adding and removing nodes, connecting them with edges etc.).

For more information see the examples folder.

## TODO ##

As the project is in active development multiple capabilities are still
missing. Here is a non-exhaustive list:

  * Custom properties - allow the user to store any type as a property
  * Traversal engine - engine that mimics Apache Gremiln
  * Examples/Tutorials - add more documentation
  * Extended testing

