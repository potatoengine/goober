find_package(SDL2 REQUIRED)

add_executable(goober_example_sdl2_opengl2 WIN32)
target_sources(goober_example_sdl2_opengl2 PRIVATE
    main.cc
)
target_link_libraries(goober_example_sdl2_opengl2 PRIVATE
    goober_core
    SDL2
    SDL2main
    opengl32
)

add_custom_command(TARGET goober_example_sdl2_opengl2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  
        $<TARGET_FILE:SDL2>
        $<TARGET_FILE_DIR:goober_example_sdl2_opengl2>
)
