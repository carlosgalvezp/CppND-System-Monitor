FROM ubuntu:20.04

ENV DEBIAN_FRONTEND="noninteractive"

RUN apt-get update && \
    apt-get install --assume-yes --no-install-recommends \
        build-essential \
        cmake \
        libncurses5-dev \
        libncursesw5-dev && \
    rm -rf /var/lib/apt/lists/*
