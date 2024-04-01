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


docker build -t vonmecheln/fet .

docker run --name fet -d -i -t \
--mount type=bind,source="$(pwd)"/fet,target=/app \
--mount type=bind,source="$(pwd)"/build,target=/build \
vonmecheln/fet /bin/sh

docker exec -it fet ./fet_build.sh

docker stop fet
docker rm fet