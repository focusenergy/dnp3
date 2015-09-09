Copyright (c) 2010, 2011 Green Energy Corp.

Copyright (c) 2013 - 2015 Automatak LLC

Licensed under the terms of the [Apache 2.0 License](http://www.apache.org/licenses/LICENSE-2.0.html).

Documentation
=============

The documentation can be found [here](http://www.automatak.com/opendnp3)

If you want to help contribute to the official guide its in [this repo](https://github.com/automatak/dnp3-guide).
    
Overview
========

Opendnp3 is a portable, scalable, and rigorously tested implementation 
of the DNP3 (www.dnp.org) protocol stack written in C++11. The library 
can handle the largest front end processor loads, but can also be
ported to run on various microcontrollers.

Langauge bindings are available. Consult the documentation.

Ubuntu 14.04 Dependencies
=========================
```sudo apt-get install autoconf libtool cmake libasio-dev libboost-all-dev
```

Build Steps
===========
```bash
cd thirdparty
./build.sh
cd ../

autoreconf -f -i
./configure
make -j6
```
