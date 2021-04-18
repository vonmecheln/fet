#!/bin/bash
mkdir -p build
cd build
rm fet*
qmake ../fet/fet.pro
make -j 8