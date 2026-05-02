cmake_minimum_required(VERSION 3.10)
project(vectorlib C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(VECTORLIB_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

option(VECTORLIB_BUILD_TESTS "Build tests" OFF)
option(VECTORLIB_BUILD_BENCHMARK "Build benchmark" OFF)

add_library(vectorlib src/vectorlib.c)

target_include_directories(vectorlib
        PUBLIC
        $<BUILD_INTERFACE:${VECTORLIB_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

if(VECTORLIB_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(VECTORLIB_BUILD_BENCHMARK)
    add_executable(benchmark tests/benchmark.c)
    target_link_libraries(benchmark PRIVATE vectorlib)
endif()

target_compile_definitions(vectorlib
        PUBLIC
        $<$<CONFIG:Debug>:DEBUG_MODE>
        $<$<CONFIG:Release>:NDEBUG>
)

target_compile_options(vectorlib
        PRIVATE
        $<$<C_COMPILER_ID:MSVC>:
        $<$<CONFIG:Release>:/O2>
        >
        $<$<NOT:$<C_COMPILER_ID:MSVC>>:
        $<$<CONFIG:Debug>:-g -O0 -Wall -Wextra>
        $<$<CONFIG:Release>:-O2>
        >
)

include(GNUInstallDirs)

install(TARGETS vectorlib
        EXPORT vectorlibTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT vectorlibTargets
        FILE vectorlibTargets.cmake
        NAMESPACE vectorlib::
        DESTINATION lib/cmake/vectorlib
)
