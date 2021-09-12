# syntax = docker/dockerfile:experimental

ARG IMAGE=debian:bullseye

FROM ${IMAGE} as builder
RUN apt-get update -qq && apt-get install -y --no-install-recommends devscripts equivs git

WORKDIR /tmp/build/src
COPY debian/control debian/
RUN yes | mk-build-deps -i
COPY . .
RUN debuild -us -uc

FROM ${IMAGE}
RUN --mount=type=bind,target=/tmp/build,source=/tmp/build,from=builder \
    apt-get update -qq && \
    apt-get install -y --no-install-recommends /tmp/build/*.deb && \
    apt-get clean && rm -rf /var/lib/apt/lists/*
