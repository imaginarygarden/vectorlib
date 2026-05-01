add_subdirectory(unity)

add_executable(test_vector test_vector.c)

target_link_libraries(test_vector
        PRIVATE
        unity
        vectorlib
)

add_test(NAME vector_tests COMMAND test_vector)
