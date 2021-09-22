# SPDX-License-Identifier: Zlib
cmake_minimum_required(VERSION 3.8)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

project(executile VERSION 0.1.0 LANGUAGES CXX)

find_package(Asciidoctor)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -pedantic -Werror -Wno-missing-field-initializers
    $<$<CONFIG:Release>:-O3>)
endif()

file(GLOB COMMON_SOURCES common/*.cpp)
add_library(executile-common STATIC ${COMMON_SOURCES})

add_executable(inet-socket-listen inet-socket-listen.cpp)
target_link_libraries(inet-socket-listen executile-common)

include(GNUInstallDirs)

install(
  TARGETS inet-socket-listen
  RUNTIME
  DESTINATION ${CMAKE_INSTALL_BINDIR})

if(ASCIIDOCTOR_FOUND)
  file(GLOB MAN_SOURCES *.adoc)

  add_asciidoctor_manpages_target(manpages MAN_OUTPUTS ${MAN_SOURCES})

  install(
    FILES ${MAN_OUTPUTS}
    DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
else()
  message(WARNING "Manpages generation is skipped due to missing asciidoctor")
endif()
