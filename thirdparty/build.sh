#!/bin/sh
cd rapidjson
git submodule update --init
mkdir build
cd build
cmake ..
make -j6
