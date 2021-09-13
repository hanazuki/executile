# syntax = docker/dockerfile:experimental

ARG IMAGE=debian:bullseye

FROM ${IMAGE} as builder
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update -qq && apt-get install -y --no-install-recommends devscripts

WORKDIR /tmp/build/src
COPY debian/control debian/
RUN apt-get build-dep -y .
COPY . .
RUN debuild -us -uc

FROM ${IMAGE}
ARG DEBIAN_FRONTEND=noninteractive
RUN --mount=type=bind,target=/tmp/build,source=/tmp/build,from=builder \
    apt-get update -qq && \
    apt-get install -y --no-install-recommends /tmp/build/*.deb && \
    apt-get clean && rm -rf /var/lib/apt/lists/*
