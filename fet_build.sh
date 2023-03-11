#!/bin/bash
rm -fr build

mkdir -p build
mkdir -p build/translations
cd build
rm fet* -rf
qmake ../fet/fet.pro
make -j 8

# cp ../fet/fet/translations /usr/share/fet/ -r
cp ../fet/fet/translations build/ -r