find_package(SDL2)
if(NOT TARGET SDL2)
    include(FetchContent)
    FetchContent_Declare(goober_sdl2_vc URL https://www.libsdl.org/release/SDL2-devel-2.0.12-VC.zip)
    FetchContent_Populate(goober_sdl2_vc)
    set(SDL2_PATH ${goober_sdl2_vc_SOURCE_DIR})
    find_package(SDL2 REQUIRED)
endif()

if(NOT TARGET GLEW)
    include(FetchContent)
    FetchContent_Declare(goober_glew_win32 URL https://github.com/nigels-com/glew/releases/download/glew-2.1.0/glew-2.1.0-win32.zip)
    FetchContent_Populate(goober_glew_win32)
    add_library(GLEW STATIC IMPORTED)
    set_target_properties(GLEW PROPERTIES IMPORTED_LOCATION ${goober_glew_win32_SOURCE_DIR}/lib/Release/x64/glew32s.lib)
    target_compile_definitions(GLEW INTERFACE GLEW_STATIC)
    target_include_directories(GLEW INTERFACE ${goober_glew_win32_SOURCE_DIR}/include)
    target_link_options(GLEW INTERFACE /NODEFAULTLIB:LIBCMT)
endif()

find_package(OpenGL REQUIRED)

add_executable(goober_example_sdl2_opengl2 WIN32)
target_sources(goober_example_sdl2_opengl2 PRIVATE
    main.cc
)
target_link_libraries(goober_example_sdl2_opengl2 PRIVATE
    goober_core
    goober_widgets
    SDL2
    SDL2main
    GLEW
    OpenGL::GL
)

add_custom_command(TARGET goober_example_sdl2_opengl2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  
        $<TARGET_FILE:SDL2>
        $<TARGET_FILE_DIR:goober_example_sdl2_opengl2>
)
