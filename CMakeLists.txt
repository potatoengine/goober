cmake_minimum_required(VERSION 3.16)
project(goober VERSION 0.1 LANGUAGES CXX C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

enable_testing()

add_subdirectory(tools)

set(goober_proggy_font ${CMAKE_CURRENT_SOURCE_DIR}/data/fonts/ProggyCrossed_Regular.ttf)
set(goober_proggy_source ${CMAKE_CURRENT_BINARY_DIR}/proggy_font.c)
add_custom_command(
    OUTPUT ${goober_proggy_source}
    DEPENDS make_header
    COMMAND make_header ${goober_proggy_font} ${goober_proggy_source} goober_proggy
    MAIN_DEPENDENCY ${goober_proggy_font}
    COMMENT Generating ${goober_proggy_source} from ${goober_proggy_font}
)

add_library(goober_core)
target_sources(goober_core PRIVATE
    include/goober/core.hh
    src/core.cc
    src/font.cc
    ${goober_proggy_source}
)
target_include_directories(goober_core PUBLIC include)
target_compile_features(goober_core PUBLIC cxx_std_17)

add_library(goober_widgets)
target_sources(goober_widgets PRIVATE
    include/goober/widgets.hh
    src/widgets.cc
)
target_link_libraries(goober_widgets PUBLIC goober_core goober_font)

add_subdirectory(test)
add_subdirectory(examples)
