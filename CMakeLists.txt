cmake_minimum_required(VERSION 3.16)
project(goober VERSION 0.1 LANGUAGES CXX C)

add_library(goober_core)
target_sources(goober_core PRIVATE
    include/goober.h
    include/goober.hh
    src/context.h
    src/goober.cc
)
target_include_directories(goober_core PUBLIC include)

add_subdirectory(test)
