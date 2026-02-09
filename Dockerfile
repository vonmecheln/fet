FROM debian:stable

LABEL Description="Build environment"

ENV HOME /root
ENV DEBIAN_FRONTEND=noninteractive

SHELL ["/bin/bash", "-c"]

# Install essential packages and locales to avoid repeated Qt locale warnings
RUN apt-get update && apt-get -y --no-install-recommends install \
    locales \
    build-essential \
    clang \
    cmake \
    qt6-base-dev \
    qmake6 \
    gdb \
    ccache \
    lld && \
    locale-gen C.UTF-8 && \
    update-locale LANG=C.UTF-8 LC_ALL=C.UTF-8 && \
    mkdir -p /root/.ccache && \
    # Ensure calls to gcc/g++ go through ccache for better hit-rate
    mkdir -p /usr/local/bin && \
    ln -sf /usr/bin/ccache /usr/local/bin/gcc && \
    ln -sf /usr/bin/ccache /usr/local/bin/g++ && \
    ln -sf /usr/bin/ccache /usr/local/bin/cc && \
    ln -sf /usr/bin/ccache /usr/local/bin/c++ && \
    /usr/bin/ccache -M 5G || true && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

ENV LANG=C.UTF-8
ENV LC_ALL=C.UTF-8

# ccache configuration
ENV CCACHE_DIR=/root/.ccache
ENV CCACHE_MAXSIZE=5G
ENV PATH=/usr/local/bin:$PATH

WORKDIR /root

ADD fet_build.sh /
RUN chmod +x /fet_build.sh


    