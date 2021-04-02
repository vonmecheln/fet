#!/bin/bash
mkdir -p build
cd build
qmake ../fet/fet.pro
make -j 8