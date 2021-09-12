cmake_minimum_required(VERSION 3.8)

project(executile VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB COMMON_SOURCES common/*.cpp)
add_library(executile-common STATIC ${COMMON_SOURCES})

add_executable(inet-socket-listen inet-socket-listen.cpp)
target_link_libraries(inet-socket-listen executile-common)

include(GNUInstallDirs)

install(
  TARGETS inet-socket-listen
  RUNTIME
    DESTINATION ${CMAKE_INSTALL_BINDIR}
)
