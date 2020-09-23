find_package(SDL2)
find_package(OpenGL)

if(TARGET SDL2 AND TARGET OpenGL::GL)
    add_executable(goober_example_sdl2_opengl2 WIN32)
    target_sources(goober_example_sdl2_opengl2 PRIVATE
        main.cc
    )
    target_link_libraries(goober_example_sdl2_opengl2 PRIVATE
        goober_core
        goober_widgets
        SDL2
        SDL2main
        OpenGL::GL
    )

    add_custom_command(TARGET goober_example_sdl2_opengl2 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different  
            $<TARGET_FILE:SDL2>
            $<TARGET_FILE_DIR:goober_example_sdl2_opengl2>
    )
endif()
