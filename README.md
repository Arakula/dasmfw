# dasmfw
The DisASseMbler FrameWork

Copyright (c) 2015-2021 Hermann Seib\
Parts Copyright (c) 2000 Arto Salmi\
Parts Copyright (c) 2013 Colin Bourassa\
Parts Copyright (c) 2014-2015 Rainer Buchty

Currently contains disassemblers for the following processor families:
6800,6801,6809,68HC11,6301,6309,AVR8,68000
(which also covers 6802,6803,6808,6303 and potentially more using the same instruction set as one of the above).

Documentation is in [dasmfw.htm](dasmfw.htm)


## Building

`dasmfw`is a command line tool written in
fairly generic C++ that should compile on a wide range of systems.

For Microsoft Visual Studio, `.sln`, and `.vcproj` files (VS2008) are provided.

A `Makefile` is provided for Linux systems; this may also work on
other systems using GNU Make. You'll need to make sure you have the
appropriate development packages (`make`, `gcc`, etc.) installed. This
was tested on Debian 9.

