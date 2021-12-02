# Hacking on executile

We welcome all your contributions to executile.

## Project files

- cmake/
  - modules/ -- CMake modules used to build this project
- common/ -- Common functions that will be statically linked to every command
- debian/ -- Files to build Debian packages
- *.cpp -- Each top-level cpp file corresponds to a command
- *.1.adoc -- Manpages for the commands

## Development

This project uses [CMake](https://cmake.org/) as the meta-build tool. Run `cmake -B build` to generate a buildsystem and `cmake --build build` to build the project. CMake also generates `compile_commands.json`, which can be consumed by [clangd](https://clangd.llvm.org/) and some IDE's for syntax checking and code completion.

[Asciidoctor](https://asciidoctor.org/) is required to generate manpages.

## Testing

TODO: Test suite is not available yet.

## Packaging

[`gbp buildpackage`](https://manpages.debian.org/stable/git-buildpackage/gbp-buildpackage.1.html) will generate source and binary Debian packages.

`docker build .` will build a container image containing the binaries. You need to enable [BuildKit](https://github.com/moby/buildkit) to build the image by setting `DOCKER_BUILDKIT=1` or activating [Docker buildx](https://github.com/docker/buildx)

## Compatibility policies

This project should be compatible with the latest major Debian release and the latest Ubuntu LTS release. It should only have the build dependency on the officially available packages from these distributions and should only have the runtime dependency on the packages usually available in the minimum configuration of these distributions (e.g., libc and libstdc++).

This project is licensed under the terms of The zlib License to make its binary distribution not require any copyright/license notices.
