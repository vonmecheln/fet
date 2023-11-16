FROM debian:testing-slim

LABEL Description="Build environment"

ENV HOME /root

SHELL ["/bin/bash", "-c"]

RUN apt-get update && apt-get -y --no-install-recommends install \
    build-essential \
    clang \
    cmake \
    qt6-base-dev \
    qmake6 \
    gdb

RUN cd ${HOME}

    