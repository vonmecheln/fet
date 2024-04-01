#!/bin/bash

cd /build
rm * -rf
qmake6 ../app/fet.pro
make -j 7
cp ../app/translations . -r
