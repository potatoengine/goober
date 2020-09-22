add_executable(goober_test)
target_sources(goober_test PRIVATE
    catch.hpp
    main.cc
    test_array.cc
    test_core.cc
    test_drawlist.cc
    test_mouse.cc
)
target_link_libraries(goober_test PRIVATE goober_core)

add_test(NAME goober_tests COMMAND goober_test)
