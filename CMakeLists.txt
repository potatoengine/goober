cmake_minimum_required(VERSION 3.16)
project(goober VERSION 0.1 LANGUAGES CXX C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

enable_testing()

add_library(goober_core)
target_sources(goober_core PRIVATE
    include/goober/core.hh
    src/core.cc
)
target_include_directories(goober_core PUBLIC include)
target_compile_features(goober_core PUBLIC cxx_std_17)

add_subdirectory(test)
add_subdirectory(examples)
