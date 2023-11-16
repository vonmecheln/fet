#!/bin/bash
# rm -fr build

# mkdir -p build
# mkdir -p build/translations
# cd build
# rm fet* -rf
# qmake ../fet/fet.pro
# make -j 8

# cd ..
# # cp ../fet/fet/translations /usr/share/fet/ -r
# cp ../fet/fet/translations build/ -r


# docker build -t vonmecheln/fet:0.1 .
# docker run -it vonmecheln/fet:0.1
docker run -it \
--mount type=bind,source="$(pwd)"/fet,target=/app \
--mount type=bind,source="$(pwd)"/build,target=/build \
vonmecheln/fet:0.2
