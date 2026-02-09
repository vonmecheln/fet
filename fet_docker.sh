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



docker build --pull -t vonmecheln/fet .

# Pass UTF-8 locale into container to avoid Qt warnings during build
mkdir -p .ccache
docker run --name fet -d -i -t \
	-e LANG=C.UTF-8 -e LC_ALL=C.UTF-8 \
	--mount type=bind,source="$(pwd)"/fet,target=/app \
	--mount type=bind,source="$(pwd)"/build,target=/build \
	--mount type=bind,source="$(pwd)"/.ccache,target=/root/.ccache \
	vonmecheln/fet /bin/sh

docker exec -it fet locale

docker exec -it fet /fet_build.sh

# docker logs -f fet

docker stop fet
docker rm fet