cmake_minimum_required(VERSION 3.16)
project(goober VERSION 0.1 LANGUAGES CXX C)

enable_testing()

add_library(goober_core)
target_sources(goober_core PRIVATE
    include/goober/array.hh
    include/goober/core.hh
    include/goober/goober.hh
    src/goober.cc
)
target_include_directories(goober_core PUBLIC include PRIVATE include/goober)
target_compile_features(goober_core PUBLIC cxx_std_17)

add_subdirectory(test)
